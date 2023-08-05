// /* Copyright (C) 2023 dengzhijun. All rights reserved.
//  *
//  *  Redistribution and use in source and binary forms, with or without
//  *  modification, are permitted provided that the following conditions
//  *  are met:
//  *    1. Redistributions of source code must retain the above copyright
//  *       notice, this list of conditions and the following disclaimer.
//  *    2. Redistributions in binary form must reproduce the above copyright
//  *       notice, this list of conditions and the following disclaimer in the
//  *       documentation and/or other materials provided with the
//  *       distribution.
//  *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
//  *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
//  *  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
//  *  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
//  *  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
//  *  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
//  *  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
//  *  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
//  *  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
//  *  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
//  *  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//  *

//
// Created by 86189 on 2023/5/28.
//

#include "driver/hal/hal_spi.h"
#include "driver/driver_chip.h"
#include "driver/hal/hal_os.h"
#include "driver/hal/hal_board.h"
#include "driver/hal/hal_def.h"
#include "sys/MiniDebug.h"
#include "driver/hal/hal_gpio.h"

//#undef  USE_RTOS_SYSTEM
typedef struct {
#ifdef USE_RTOS_SYSTEM
    HAL_Semaphore spiSem;
    HAL_Mutex spiMutex;
#endif
    SPI_HandleTypeDef hspi;
    board_pinmux_info_t csPinmuxCfg;
    GPIO_PinMuxParam *csPin ;
} SpiPrivate_t;

static SpiPrivate_t *gSpiPrivate[HAL_SPI_NR];

static SpiPrivate_t *SPIPrivateGet(HAL_SPI_ID spiId) {
    if (spiId >= HAL_SPI_NR)
        return NULL;
    return gSpiPrivate[spiId];
}

static void SPIPrivateSet(HAL_SPI_ID spiId, SpiPrivate_t *spiPrivate) {
    if (spiId >= HAL_SPI_NR)
        return;
    gSpiPrivate[spiId] = spiPrivate;
}

static SPI_TypeDef *SPIInstanceGet(HAL_SPI_ID spiId) {
    switch (spiId) {
        case HAL_SPI_1:
            return SPI1;
        case HAL_SPI_2:
            return SPI2;
#ifdef TARGET_MCU_STM32F4
        case HAL_SPI_3:
            return SPI3;
#endif
        default:
            return NULL;
    }
}

static uint32_t SPISpeedGet(uint32_t speed) {
    uint32_t spiSpeed = SPI_BAUDRATEPRESCALER_2;
    if (speed >= 240000000)
        spiSpeed = SPI_BAUDRATEPRESCALER_2;
    else if (speed >= 12000000)
        spiSpeed = SPI_BAUDRATEPRESCALER_4;
    else if (speed >= 6000000)
        spiSpeed = SPI_BAUDRATEPRESCALER_8;
    else if (speed >= 3000000)
        spiSpeed = SPI_BAUDRATEPRESCALER_16;
    else if (speed >= 1500000)
        spiSpeed = SPI_BAUDRATEPRESCALER_32;
    else if (speed >= 750000)
        spiSpeed = SPI_BAUDRATEPRESCALER_64;
    else if (speed >= 400000)
        spiSpeed = SPI_BAUDRATEPRESCALER_128;
    else
        spiSpeed = SPI_BAUDRATEPRESCALER_256;
    return spiSpeed;
}

static void SPIClockEnable(HAL_SPI_ID spiId) {
    switch (spiId) {
        case HAL_SPI_1:
            __HAL_RCC_SPI1_CLK_ENABLE();
            #ifdef TARGET_MCU_STM32F1
            __HAL_AFIO_REMAP_SPI1_ENABLE();
            #endif
            break;
        case HAL_SPI_2:
            __HAL_RCC_SPI2_CLK_ENABLE();
            break;
#ifdef TARGET_MCU_STM32F4
        case HAL_SPI_3:
            __HAL_RCC_SPI3_CLK_ENABLE();
            break;
#endif
        default:
            break;
    }
}

static void SPIClockDisable(HAL_SPI_ID spiId) {
    switch (spiId) {
        case HAL_SPI_1:
            __HAL_RCC_SPI1_CLK_DISABLE();
            break;
        case HAL_SPI_2:
            __HAL_RCC_SPI2_CLK_DISABLE();
            break;
#ifdef TARGET_MCU_STM32F4
        case HAL_SPI_3:
            __HAL_RCC_SPI3_CLK_DISABLE();
            break;
#endif
        default:
            break;
    }
}

static HAL_Status SPIHwInit(HAL_SPI_ID spiId, SpiConfig_t *spiConfig) {
    SPIClockEnable(spiId);
    HAL_BoardIoctl(HAL_BIR_PINMUX_INIT, HAL_MKDEV(HAL_DEV_MAJOR_SPI, spiId), 0);

    SPI_HandleTypeDef *pSpi = &SPIPrivateGet(spiId)->hspi;

    uint32_t clkPol = 0;
    uint32_t clkPha = 0;
    if (spiConfig->mode == HAL_SPI_MODE_0) {
        clkPol = SPI_POLARITY_LOW;
        clkPha = SPI_PHASE_1EDGE;
    } else if (spiConfig->mode == HAL_SPI_MODE_1) {
        clkPol = SPI_POLARITY_LOW;
        clkPha = SPI_PHASE_2EDGE;
    } else if (spiConfig->mode == HAL_SPI_MODE_2) {
        clkPol = SPI_POLARITY_HIGH;
        clkPha = SPI_PHASE_1EDGE;
    } else if (spiConfig->mode == HAL_SPI_MODE_3) {
        clkPol = SPI_POLARITY_HIGH;
        clkPha = SPI_PHASE_2EDGE;
    } else {
        assert(NULL);
    }

    uint32_t firstBit = SPI_FIRSTBIT_MSB;
    if (spiConfig->bit_order == HAL_SPI_MSB_FIRST) {
        firstBit = SPI_FIRSTBIT_MSB;
    } else if (spiConfig->bit_order == HAL_SPI_LSB_FIRST) {
        firstBit = SPI_FIRSTBIT_LSB;
    }


    pSpi->Instance = SPIInstanceGet(spiId);
    pSpi->Init.Mode = SPI_MODE_MASTER;
    pSpi->Init.Direction = SPI_DIRECTION_2LINES;
    pSpi->Init.DataSize = SPI_DATASIZE_8BIT;
    pSpi->Init.CLKPolarity = clkPol;
    pSpi->Init.CLKPhase = clkPha;
    pSpi->Init.NSS = SPI_NSS_SOFT;
    pSpi->Init.BaudRatePrescaler = SPISpeedGet(spiConfig->speed);
    pSpi->Init.FirstBit = firstBit;
    pSpi->Init.TIMode = SPI_TIMODE_DISABLE;
    pSpi->Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
    pSpi->Init.CRCPolynomial = 10;
    if (HAL_SPI_Init(pSpi) != HAL_OK) {
        assert(NULL);
    }

    HAL_BoardIoctl(HAL_BIR_GET_CFG, HAL_MKDEV(HAL_DEV_MAJOR_SPI, spiId), (uint32_t)&SPIPrivateGet(spiId)->csPinmuxCfg);

    return HAL_STATUS_OK;
}

static void SPIHwDeinit(HAL_SPI_ID spiId) {
    HAL_SPI_DeInit(&SPIPrivateGet(spiId)->hspi);
    HAL_BoardIoctl(HAL_BIR_PINMUX_DEINIT, HAL_MKDEV(HAL_DEV_MAJOR_SPI, spiId), 0);
    SPIClockDisable(spiId);
}


HAL_Status HAL_spiInit(HAL_SPI_ID spiId, SpiConfig_t *spiConfig) {
    SpiPrivate_t *spiPrivate = SPIPrivateGet(spiId);
    if (spiPrivate == NULL) {
        spiPrivate = (SpiPrivate_t *) HAL_Malloc(sizeof(SpiPrivate_t));
        if (spiPrivate == NULL) {
            return HAL_STATUS_ERROR;
        }

        SPIPrivateSet(spiId, spiPrivate);
    }

    memset(spiPrivate, 0, sizeof(SpiPrivate_t));
    SPIHwInit(spiId, spiConfig);

#ifdef USE_RTOS_SYSTEM
    HAL_SemaphoreInit(&spiPrivate->spiSem, 1, 1);
    HAL_MutexInit(&spiPrivate->spiMutex);
#endif

    return HAL_STATUS_OK;
}


void HAL_spiDeinit(HAL_SPI_ID spiId) {
    if (SPIPrivateGet(spiId) == NULL) {
        return;
    }

#ifdef USE_RTOS_SYSTEM
    HAL_SemaphoreDeinit(&SPIPrivateGet(spiId)->spiSem);
    HAL_MutexDeinit(&SPIPrivateGet(spiId)->spiMutex);
#endif
    SPIHwDeinit(spiId);
    HAL_Free(SPIPrivateGet(spiId));
    SPIPrivateSet(spiId, NULL);
}


HAL_Status HAL_spiOpen(HAL_SPI_ID spiId, HAL_SPI_TCTRL_SS_SEL spiCs,uint32_t msecs){
    SpiPrivate_t *spiPrivate = SPIPrivateGet(spiId);
#ifdef USE_RTOS_SYSTEM
    HAL_Status res = HAL_SemaphoreWait(&spiPrivate->spiSem, msecs);
    if (res != HAL_STATUS_OK) {
        return res ;
    }
#endif
    board_pinmux_info_t *boardPinmuxInfo = &spiPrivate->csPinmuxCfg;
    if (spiCs > boardPinmuxInfo->count){
#ifdef USE_RTOS_SYSTEM
        HAL_SemaphoreRelease(&spiPrivate->spiSem);
#endif
        return  HAL_STATUS_INVALID;
    }

    const GPIO_PinMuxParam *csPin = &boardPinmuxInfo->pinmux[spiCs] ;
    spiPrivate->csPin = (GPIO_PinMuxParam *)csPin ;


    return HAL_STATUS_OK;
}

HAL_Status HAL_spiClose(HAL_SPI_ID spiId){
    SpiPrivate_t *spiPrivate = SPIPrivateGet(spiId);

#ifdef USE_RTOS_SYSTEM
    HAL_SemaphoreRelease(&spiPrivate->spiSem);
#endif

    spiPrivate->csPin = NULL;

    return HAL_STATUS_OK;
}

void HAL_spiCsEnable(HAL_SPI_ID spiId){
    SpiPrivate_t *spiPrivate = SPIPrivateGet(spiId);
    assert(spiPrivate != NULL);
    assert(spiPrivate->csPin != NULL);

    HAL_GpioWritePin(spiPrivate->csPin->port, spiPrivate->csPin->pin, GPIO_PIN_LOW);
}

void HAL_spiCsDisable(HAL_SPI_ID spiId){
    SpiPrivate_t *spiPrivate = SPIPrivateGet(spiId);

    HAL_GpioWritePin(spiPrivate->csPin->port, spiPrivate->csPin->pin, GPIO_PIN_HIGH);
}

HAL_Status HAL_spiTransmit(HAL_SPI_ID spiId, uint8_t *pData, uint16_t size, uint32_t timeout) {
    HAL_Status ret = HAL_STATUS_OK;
    SpiPrivate_t *spiPrivate = SPIPrivateGet(spiId);
    if (spiPrivate == NULL) {
        return HAL_STATUS_ERROR;
    }

    if (spiPrivate->csPin == NULL){
        return HAL_STATUS_ERROR;
    }

    SPI_HandleTypeDef *pSpi = &spiPrivate->hspi;

#ifdef USE_RTOS_SYSTEM
    HAL_MutexLock(&spiPrivate->spiMutex, timeout);
    if (HAL_SPI_Transmit(pSpi, pData, size, timeout) != HAL_OK) {
        ret = HAL_STATUS_ERROR;
    }
    HAL_MutexUnlock(&spiPrivate->spiMutex);
#else
    if (HAL_SPI_Transmit(pSpi, pData, size, timeout) != HAL_OK) {
        ret = HAL_STATUS_ERROR;
    }
#endif

    return ret;
}

HAL_Status HAL_spiReceive(HAL_SPI_ID spiId,uint8_t *pData, uint16_t size, uint32_t timeout) {
    HAL_Status ret = HAL_STATUS_OK;
    SpiPrivate_t *spiPrivate = SPIPrivateGet(spiId);
    if (spiPrivate == NULL) {
        return HAL_STATUS_ERROR;
    }

    if (spiPrivate->csPin == NULL){
        return HAL_STATUS_ERROR;
    }

    SPI_HandleTypeDef *pSpi = &spiPrivate->hspi;

#ifdef USE_RTOS_SYSTEM
    HAL_MutexLock(&spiPrivate->spiMutex, timeout);
    if (HAL_SPI_Receive(pSpi, pData, size, timeout) != HAL_OK) {
        ret = HAL_STATUS_ERROR;
    }
    HAL_MutexUnlock(&spiPrivate->spiMutex);
#else
    if (HAL_SPI_Receive(pSpi, pData, size, timeout) != HAL_OK) {
        ret = HAL_STATUS_ERROR;
    }
#endif

    return ret;
}


HAL_Status HAL_spiTransmitReceive(HAL_SPI_ID spiId, uint8_t *pTxData, uint8_t *pRxData, uint16_t size, uint32_t timeout) {
    HAL_Status ret = HAL_STATUS_OK;
    SpiPrivate_t *spiPrivate = SPIPrivateGet(spiId);
    if (spiPrivate == NULL) {
        return HAL_STATUS_ERROR;
    }

    if (spiPrivate->csPin == NULL){
        return HAL_STATUS_ERROR;
    }

    SPI_HandleTypeDef *pSpi = &spiPrivate->hspi;

#ifdef USE_RTOS_SYSTEM
    HAL_MutexLock(&spiPrivate->spiMutex, timeout);
    if (HAL_SPI_TransmitReceive(pSpi, pTxData, pRxData, size, timeout) != HAL_OK) {
        ret = HAL_STATUS_ERROR;
    }
    HAL_MutexUnlock(&spiPrivate->spiMutex);
#else
    if (HAL_SPI_TransmitReceive(pSpi, pTxData, pRxData, size, timeout) != HAL_OK) {
        ret = HAL_STATUS_ERROR;
    }
#endif

    return ret;
}

