
#include "sys/MiniDebug.h"
#include "driver/hal/hal_os.h"
#include "driver/hal/hal_gpio.h"
#include "driver/hal/hal_spi.h"
#include "driver/hal/hal_board.h"
#include "main.h"
#include "driver/component/wireless/nRF24L01/nRF24L01.h"
#include "log/easylogger/elog.h"
#undef LOG_TAG
#define LOG_TAG "WIRELESS"

#define NRF24L01_SPI_PORT HAL_SPI_1
#define NRF24L01_SPI_MODE HAL_SPI_MODE_0
#define NRF24L01_SPI_CS HAL_SPI_TCTRL_SS_SEL_SS1
#define NRF24L01_SPI_SPEED 1000000
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

    uint8_t  status = spiWriteAndRead(reg);
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

static void wirelessTask(void *args){
    uint8_t buf[32] = {0};
    uint8_t len = 0;

    NRF24L01_RX_Mode(&nrf24l01);
    while (1){
        if (NRF24L01_RxPacket(&nrf24l01, buf, len)){
            log_i("wireless recv: %s", buf);
        }
    }
}

void wirelessInit(void){
    NRF24L01_Init(&nrf24l01, &ops);

    HAL_ThreadCreate(wirelessTask, "wireless", 256, 0, HAL_OS_PRIORITY_NORMAL , NULL);

    log_i("wireless init success");
}

int wirelessSend(uint8_t *buf, uint8_t len){
    return NRF24L01_TxPacket(&nrf24l01, buf, len);
}