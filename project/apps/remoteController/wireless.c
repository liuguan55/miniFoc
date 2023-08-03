
#include "sys/MiniDebug.h"
#include "driver/hal/hal_os.h"
#include "driver/hal/hal_spi.h"
#include "driver/component/wireless/nRf24l01/nRf24l01.h"
#undef LOG_TAG
#define LOG_TAG "WIRELESS"

#define NRF24L01_SPI_PORT HAL_SPI_1
#define NRF24L01_SPI_MODE HAL_SPI_MODE_0
#define NRF24L01_SPI_CS HAL_SPI_TCTRL_SS_SEL_SS1
#define NRF24L01_SPI_SPEED 1000000
#define NRF24L01_SPI_MODE HAL_SPI_MODE_0
#define NRF24L01_SPI_DATASIZE HAL_SPI_DATASIZE_8BIT
#define NRF24L01_SPI_FIRSTBIT HAL_SPI_FIRSTBIT_MSB


static uint8_t spiWriteAndRead(uint8_t data) {
    uint8_t rx = 0;
    HAL_spiTransmitReceive(NRF24L01_SPI_PORT, &data, &rx, 1, 1000);

    return rx;
}

static void wirelessSpiInit(void){
    SpiConfig_t spiConfig = {
        .mode = NRF24L01_SPI_MODE,
        .dataSize = NRF24L01_SPI_DATASIZE,
        .firstBit = NRF24L01_SPI_FIRSTBIT,
        .speed = NRF24L01_SPI_SPEED,
    }

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

    return ret;
}

static uint8_t wirelessWriteBuf(uint8_t reg, uint8_t *pBuf, uint8_t len){
    if (HAL_spiOpen(NRF24L01_SPI_PORT, NRF24L01_SPI_CS, 100) != HAL_STATUS_OK){
        return 0;
    }

    HAL_spiCsEnable(NRF24L01_SPI_PORT);

    uint8_t  status = spiWriteAndRead(reg);
    for (uint8_t i = 0; i < len; i++){
        spiWriteAndRead(PBuf[i]);
    }

    HAL_spiCsDisable(NRF24L01_SPI_PORT);

    HAL_spiClose(NRF24L01_SPI_PORT);

    return ret;
}

static uint8_t wirelessDataReady(void){
    
}

static void wirelessCeEnable(uint8_t enable){
    
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
/**
  * @brief  The wireless task.
  *
  * @retval int
  */
void wirelessInit(void){
    NRF24L01_Init(&nrf24l01, &ops);
    NRF24L01_RX_Mode(&nrf24l01);
}
