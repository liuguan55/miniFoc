
#include "sys/MiniDebug.h"
#include "driver/hal/hal_os.h"
#include "driver/hal/hal_gpio.h"
#include "driver/hal/hal_spi.h"
#include "driver/hal/hal_board.h"
#include "main.h"
#include "driver/component/wireless/nRF24L01/nRF24L01.h"
#include "log/easylogger/elog.h"
#include "driver/hal/hal_os.h"
#include "remote.h"
#include "wireless.h"

#undef LOG_TAG
#define LOG_TAG "WIRELESS"

#define NRF24L01_SPI_PORT HAL_SPI_1
#define NRF24L01_SPI_MODE HAL_SPI_MODE_0
#define NRF24L01_SPI_CS HAL_SPI_TCTRL_SS_SEL_SS1
#define NRF24L01_SPI_SPEED 3000000
#define NRF24L01_SPI_MODE HAL_SPI_MODE_0
#define NRF24L01_SPI_DATASIZE HAL_SPI_8BIT
#define NRF24L01_SPI_FIRSTBIT HAL_SPI_MSB_FIRST


static uint8_t spiWriteAndRead(uint8_t data) {
    uint8_t rx = 0;
    HAL_spiTransmitReceive(NRF24L01_SPI_PORT, &data, &rx, 1, 1000);

    return rx;
}

static void wirelessSpiInit(void){
    SpiConfig_t spiConfig = {
        .mode = NRF24L01_SPI_MODE,
        .word_size = NRF24L01_SPI_DATASIZE,
        .bit_order = NRF24L01_SPI_FIRSTBIT,
        .speed = NRF24L01_SPI_SPEED,
    };

    HAL_spiInit(NRF24L01_SPI_PORT, &spiConfig);

    HAL_spiCsDisable(NRF24L01_SPI_PORT);
}

static void wirelessWriteReg(uint8_t reg, uint8_t value){
    if (HAL_spiOpen(NRF24L01_SPI_PORT, NRF24L01_SPI_CS, 100) != HAL_STATUS_OK){
        return ;
    }

    HAL_spiCsEnable(NRF24L01_SPI_PORT);
    spiWriteAndRead(reg);
    spiWriteAndRead(value);
    HAL_spiCsDisable(NRF24L01_SPI_PORT);

    HAL_spiClose(NRF24L01_SPI_PORT);
}

static uint8_t wirelessReadReg(uint8_t reg){
    if (HAL_spiOpen(NRF24L01_SPI_PORT, NRF24L01_SPI_CS, 100) != HAL_STATUS_OK){
        return 0;
    }

    HAL_spiCsEnable(NRF24L01_SPI_PORT);
    uint8_t ret = spiWriteAndRead(reg);
    HAL_spiCsDisable(NRF24L01_SPI_PORT);

    HAL_spiClose(NRF24L01_SPI_PORT);

    return ret;
}

static uint8_t wirelessReadBuf(uint8_t reg, uint8_t *pBuf, uint8_t len){
    if (HAL_spiOpen(NRF24L01_SPI_PORT, NRF24L01_SPI_CS, 100) != HAL_STATUS_OK){
        return 0;
    }

    HAL_spiCsEnable(NRF24L01_SPI_PORT);
    spiWriteAndRead(reg);
    for (uint8_t i = 0; i < len; i++){
        pBuf[i] = spiWriteAndRead(0xFF);
    }

    HAL_spiCsDisable(NRF24L01_SPI_PORT);

    HAL_spiClose(NRF24L01_SPI_PORT);

    return len;
}

static uint8_t wirelessWriteBuf(uint8_t reg, uint8_t *pBuf, uint8_t len){
    if (HAL_spiOpen(NRF24L01_SPI_PORT, NRF24L01_SPI_CS, 100) != HAL_STATUS_OK){
        return 0;
    }

    HAL_spiCsEnable(NRF24L01_SPI_PORT);
    spiWriteAndRead(reg);
    for (uint8_t i = 0; i < len; i++){
        spiWriteAndRead(pBuf[i]);
    }

    HAL_spiCsDisable(NRF24L01_SPI_PORT);

    HAL_spiClose(NRF24L01_SPI_PORT);

    return len;
}

static uint8_t wirelessDataReady(void){
    board_pinmux_info_t pinmux_info;
    HAL_BoardIoctl(HAL_BIR_GET_CFG, HAL_MKDEV(HAL_DEV_MAJOR_GPIO, REMOTE_NRF24L01_IRQ_ID), (uint32_t)&pinmux_info);

    return HAL_GpioReadPin(pinmux_info.pinmux[0].port, pinmux_info.pinmux[0].pin);
}

static void wirelessCeEnable(uint8_t enable){
    board_pinmux_info_t pinmux_info;
    HAL_BoardIoctl(HAL_BIR_GET_CFG, HAL_MKDEV(HAL_DEV_MAJOR_GPIO, REMOTE_NRF24L01_CE_ID), (uint32_t)&pinmux_info);

    HAL_GpioWritePin(pinmux_info.pinmux[0].port, pinmux_info.pinmux[0].pin, enable);
}

static NRF24L01_ops_t ops = {
    .init = wirelessSpiInit,
    .deinit = NULL,
    .writeReg = wirelessWriteReg,
    .readReg = wirelessReadReg,
    .readBuf = wirelessReadBuf,
    .writeBuf = wirelessWriteBuf,
    .dataReady = wirelessDataReady,
    .ceEnable = wirelessCeEnable,
};

static NRF24L01_t nrf24l01 = {
    .ops = &ops,
    .address = {0x34, 0x43, 0x10, 0x10, 0x01},
};

static HAL_Mutex g_wirelessMutex = NULL;
static int8_t g_initailized = 0;
static int8_t g_wirelessState = 0;

static void wirelessTask(void *args){
    UNUSED(args);

    uint8_t recvBuffer[32] = {0};
    uint8_t recvLen = sizeof (recvBuffer);

    while (1){
        if (wirelessRecv(recvBuffer, recvLen)){
            printf("wireless recv: %s recvLen %d\n", recvBuffer,recvLen);
            remote_parsePacket(recvBuffer, recvLen);
        }

        HAL_msleep(500);
    }
}

void wirelessInit(void){
    if (NRF24L01_Init(&nrf24l01, &ops) < 0){
        printf("wireless init failed\n");
        return ;
    }

    HAL_MutexInit(&g_wirelessMutex);
//    HAL_ThreadCreate(wirelessTask, "wireless", 4*128, 0, HAL_OS_PRIORITY_NORMAL , NULL);

    NRF24L01_TX_Mode(&nrf24l01);
    g_initailized = 1;
    printf("wireless init success\n");
}

int wirelessRecv(uint8_t *buf, uint8_t len){
    if (!g_initailized) return 0;
    int recvLen = 0;

    HAL_MutexLock(&g_wirelessMutex, HAL_OS_WAIT_FOREVER);
    g_wirelessState = 1;
    NRF24L01_RX_Mode(&nrf24l01);
    recvLen =  NRF24L01_RxPacket(&nrf24l01, buf, len);
    HAL_MutexUnlock(&g_wirelessMutex);

    g_wirelessState = 0;

    return recvLen;
}

int wirelessSend(uint8_t *buf, uint8_t len){
    if (!g_initailized) return 0;

    int sendLen = 0;

    HAL_MutexLock(&g_wirelessMutex,HAL_OS_WAIT_FOREVER);
    g_wirelessState = 1;

    sendLen = NRF24L01_TxPacket(&nrf24l01, buf, 32);
    HAL_MutexUnlock(&g_wirelessMutex);

    g_wirelessState = 0;

    return sendLen;
}

int8_t wirelessState(void){
    return g_wirelessState;
}