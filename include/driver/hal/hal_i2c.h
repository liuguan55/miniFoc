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

#ifndef MINIFOC_F4_HAL_I2C_H
#define MINIFOC_F4_HAL_I2C_H
#include "stdint.h"
#include "driver/hal/hal_def.h"

#ifdef __cplusplus
extern "C" {
#endif

enum {
    HAL_I2C_1,
    HAL_I2C_2,
#ifdef TARGET_MCU_STM32F4
    HAL_I2C_3,
#endif
    HAL_I2C_NR
};
typedef uint8_t HAL_I2C_ID;

enum {
    HAL_I2C_ADDR_MODE_7BIT = 0,
    HAL_I2C_ADDR_MODE_10BIT,
    HAL_I2C_ADDR_MODE_NR
};
typedef uint8_t HAL_I2C_ADDR_MODE;

typedef struct {
    HAL_I2C_ADDR_MODE addrMode;
    uint32_t clockSpeed ;
}I2C_Config;

/**
 * @brief initialize i2c  according to i2cId and i2CConfig
 * @param i2cId ID of specified i2c
 * @param i2CConfig Pointer to I2C_Config structure
 * @return HAL_Status, HAL_STATUS_OK on success, else HAL_STATUS_ERROR
 */
HAL_Status HAL_I2CInit(HAL_I2C_ID i2cId, I2C_Config *i2CConfig);

/**
 * @brief deinitialize i2c
 * @param i2cId ID of specified i2c
 * @return HAL_Status, HAL_STATUS_OK on success, else HAL_STATUS_ERROR
 */
HAL_Status HAL_I2CDeInit(HAL_I2C_ID i2cId);

/**
 * @brief read data from i2c
 * @param i2cId ID of specified i2c
 * @param slaveAddr slave address
 * @param regAddr register address
 * @param data pointer to data buffer
 * @param size size of data to read
 * @return HAL_Status, HAL_STATUS_OK on success, else HAL_STATUS_ERROR
 */
HAL_Status HAL_I2CTransmit(HAL_I2C_ID i2cId, uint16_t slaveAddr, uint8_t *pData, uint16_t size, uint32_t timeout);

/**
 * @brief receive data from i2c slave
 * @param i2cId ID of specified i2c
 * @param slaveAddr slave address
 * @param pData Pointer to data buffer
 * @param size size of data buffer
 * @param timeout timeout in ms
 * @return HAL_Status, HAL_STATUS_OK on success, else HAL_STATUS_ERROR
 */
HAL_Status HAL_I2CReceive(HAL_I2C_ID i2cId, uint16_t slaveAddr, uint8_t *pData, uint16_t size, uint32_t timeout);

#ifdef __cplusplus
}
#endif

#endif //MINIFOC_F4_HAL_I2C_H
