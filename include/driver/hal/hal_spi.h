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

#ifndef MINIFOC_F4_HAL_SPI_H
#define MINIFOC_F4_HAL_SPI_H
#include "driver/hal/hal_def.h"

#ifdef __cplusplus
extern "C" {
#endif

enum {
    HAL_SPI_TCTRL_SS_SEL_SS1 = 0,
    HAL_SPI_TCTRL_SS_SEL_SS2,
    HAL_SPI_TCTRL_SS_SEL_NR
};
typedef uint8_t HAL_SPI_TCTRL_SS_SEL;

enum {
    HAL_SPI_1 = 0,
    HAL_SPI_2,
    HAL_SPI_3,
    HAL_SPI_NR
} ;
typedef uint8_t HAL_SPI_ID ;

enum {
    HAL_SPI_MODE_0 = 0,
    HAL_SPI_MODE_1,
    HAL_SPI_MODE_2,
    HAL_SPI_MODE_3,
    HAL_SPI_MODE_NR
} ;
typedef uint8_t HAL_SPI_MODE ;

enum {
    HAL_SPI_MSB_FIRST = 0,
    HAL_SPI_LSB_FIRST,
    HAL_SPI_BIT_ORDER_NR
} ;
typedef uint8_t HAL_SPI_BIT_ORDER ;

enum {
    HAL_SPI_8BIT = 0,
    HAL_SPI_16BIT,
    HAL_SPI_WORD_SIZE_NR
} ;
typedef uint8_t HAL_SPI_WORD_SIZE ;

typedef struct {
    HAL_SPI_MODE mode ;
    HAL_SPI_BIT_ORDER bit_order ;
    HAL_SPI_WORD_SIZE word_size ;
    uint32_t speed ;
} SpiConfig_t;


/**
 * @brief Initialize SPI according to the specified parameters in the SpiConfig_t.
 * @param spiId ID of the SPI Interface
 * @param spiConfig Pointer to a SpiConfig_t structure that contains
 * @return HAL_Status , HAL_OK on success.
 */
HAL_Status HAL_spiInit(HAL_SPI_ID spiId, SpiConfig_t *spiConfig);

/**
 * @brief DeInitialize SPI
 * @param spiId ID of the SPI Interface
 */
void HAL_spiDeinit(HAL_SPI_ID spiId);

/**
 * @brief Open SPI device and set CS pin
 * @param spiId  ID of SPI device
 * @param spiCs  select cs pin
 * @param spiConfig Pointer to SpiConfig_t
 * @param msecs timeout of ms
 * @return HAL_Status_ok success, otherwise failed
 */
HAL_Status HAL_spiOpen(HAL_SPI_ID spiId, HAL_SPI_TCTRL_SS_SEL spiCs, uint32_t msecs);

/**
 * @brief close spi handle
 * @param spiId ID of specify spi
 * @return
 */
HAL_Status HAL_spiClose(HAL_SPI_ID spiId);


/**
 * @brief Enable cs
 * @param spiId
 */
void HAL_spiCsEnable(HAL_SPI_ID spiId);

/**
 * @brief Disable cs
 * @param spiId
 */
void HAL_spiCsDisable(HAL_SPI_ID spiId);

/**
 * @brief Transmit an amount of data in blocking mode
 * @param spiId ID of the SPI Interface
 * @param pData Pointer to data buffer
 * @param size Amount of data to be sent
 * @param timeout Timeout duration
 * @return HAL_Status , HAL_OK on success.
 */
HAL_Status HAL_spiTransmit(HAL_SPI_ID spiId, uint8_t *pData, uint16_t size, uint32_t timeout);

/**
 * @brief Receive an amount of data in blocking mode
 * @param spiId ID of the SPI Interface
 * @param pData Pointer to data buffer
 * @param size Amount of data to be received
 * @param timeout Timeout duration
 * @return HAL_Status , HAL_OK on success.
 */
HAL_Status HAL_spiReceive(HAL_SPI_ID spiId, uint8_t *pData, uint16_t size, uint32_t timeout);

/**
 * @brief Transmit and Receive an amount of data in blocking mode
 * @param spiId ID of the SPI Interface
 * @param pTxData Pointer to data buffer to be transmitted
 * @param pRxData Pointer to data buffer to be received
 * @param size Amount of data to be sent/received
 * @param timeout Timeout duration
 * @return HAL_Status , HAL_OK on success.
 */
HAL_Status HAL_spiTransmitReceive(HAL_SPI_ID spiId, uint8_t *pTxData, uint8_t *pRxData, uint16_t size, uint32_t timeout);

#ifdef __cplusplus
}
#endif

#endif //MINIFOC_F4_HAL_SPI_H
