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

#include "driver/hal/hal_i2c.h"
#include "driver/driver_chip.h"
#include "driver/hal/hal_board.h"
#include "driver/hal/hal_def.h"
#include "driver/hal/hal_dev.h"
#include "driver/hal/hal_os.h"
#include "sys/MiniDebug.h"

typedef struct {
    I2C_HandleTypeDef hi2c;
    HAL_Semaphore semaphore;
    HAL_Mutex mutex;
}I2CPrivate_t;

static I2CPrivate_t *gI2cPrivate[HAL_I2C_NR] = {0};

static I2C_TypeDef *gI2cInstance[HAL_I2C_NR] = {
        I2C1,
        I2C2,
        I2C3,
};

static uint32_t gI2cAddrModeMap[HAL_I2C_ADDR_MODE_NR] = {
        I2C_ADDRESSINGMODE_7BIT,
        I2C_ADDRESSINGMODE_10BIT,
};

__STATIC_INLINE uint32_t I2CAddrModeGet(HAL_I2C_ADDR_MODE addrMode)
{
    if(addrMode >= HAL_I2C_ADDR_MODE_NR)
        return 0;

    return gI2cAddrModeMap[addrMode];
}

__STATIC_INLINE I2C_TypeDef *I2CInstanceGet(HAL_I2C_ID i2cId)
{
    if(i2cId >= HAL_I2C_NR)
        return NULL;

    return gI2cInstance[i2cId];
}

__STATIC_INLINE  I2CPrivate_t * I2CPrivateGet(HAL_I2C_ID i2cId)
{
    if(i2cId >= HAL_I2C_NR)
        return NULL;

    return gI2cPrivate[i2cId];
}

__STATIC_INLINE void I2CPrivateSet(HAL_I2C_ID i2cId, I2CPrivate_t *i2cPrivate)
{
    if(i2cId >= HAL_I2C_NR)
        return;

    gI2cPrivate[i2cId] = i2cPrivate;
}

/**
 * @brief enable i2c clock according to i2cId
 * @param i2cId ID of specified i2c
 */
static void I2CClockEnable(HAL_I2C_ID i2cId)
{
    if(i2cId >= HAL_I2C_NR)
        return;

    switch(i2cId) {
        case HAL_I2C_1:
            __HAL_RCC_I2C1_CLK_ENABLE();
            break;
        case HAL_I2C_2:
            __HAL_RCC_I2C2_CLK_ENABLE();
            break;
        case HAL_I2C_3:
            __HAL_RCC_I2C3_CLK_ENABLE();
            break;
        default:
            break;
    }
}

/**
 * @brief disable i2c clock according to i2cId
 * @param i2cId  ID of specified i2c
 */
static void I2CClockDisable(HAL_I2C_ID i2cId)
{
    if(i2cId >= HAL_I2C_NR)
        return;

    switch(i2cId) {
        case HAL_I2C_1:
            __HAL_RCC_I2C1_CLK_DISABLE();
            break;
        case HAL_I2C_2:
            __HAL_RCC_I2C2_CLK_DISABLE();
            break;
        case HAL_I2C_3:
            __HAL_RCC_I2C3_CLK_DISABLE();
            break;
        default:
            break;
    }
}

/**
 * @brief initialize i2c hardware according to i2cId and i2CConfig
 * @param i2cId ID of specified i2c
 * @param i2CConfig Pointer to I2C_Config structure
 * @return
 */
static HAL_Status I2CHwInit(HAL_I2C_ID i2cId, I2C_Config *i2CConfig)
{
    if(i2cId >= HAL_I2C_NR)
        return HAL_STATUS_ERROR;

    I2CPrivate_t *pI2CPrivate = I2CPrivateGet(i2cId);
    if(pI2CPrivate == NULL)
        return HAL_STATUS_ERROR;

    I2CClockEnable(i2cId);
    HAL_BoardIoctl(HAL_BIR_PINMUX_INIT, HAL_MKDEV(HAL_DEV_MAJOR_I2C, i2cId), 0);

    I2C_HandleTypeDef *pI2c = &pI2CPrivate->hi2c;
    pI2c->Instance = I2CInstanceGet(i2cId);
    pI2c->Init.ClockSpeed = i2CConfig->clockSpeed;
    pI2c->Init.DutyCycle = I2C_DUTYCYCLE_2;
    pI2c->Init.OwnAddress1 = 0;
    pI2c->Init.AddressingMode = I2CAddrModeGet(i2CConfig->addrMode);
    pI2c->Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
    pI2c->Init.OwnAddress2 = 0;
    pI2c->Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
    pI2c->Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
    if (HAL_I2C_Init(pI2c) != HAL_OK) {
        assert(NULL);
    }

    return HAL_STATUS_OK;
}

/**
 * @brief deinitialize i2c hardware
 * @param pI2c Pointer to I2C_HandleTypeDef structure
 * @return HAL_Status, HAL_STATUS_OK on success, else HAL_STATUS_ERROR
 */
static HAL_Status I2CHwDeinit(HAL_I2C_ID i2cId, I2C_HandleTypeDef *pI2c)
{
    if (HAL_I2C_DeInit(pI2c) != HAL_OK) {
        assert(NULL);
    }

    I2CClockDisable(i2cId);
    HAL_BoardIoctl(HAL_BIR_PINMUX_DEINIT, HAL_MKDEV(HAL_DEV_MAJOR_I2C, i2cId), 0);

    return HAL_STATUS_OK;
}


HAL_Status HAL_I2CInit(HAL_I2C_ID i2cId, I2C_Config *i2CConfig)
{
    if(i2cId >= HAL_I2C_NR)
        return HAL_STATUS_ERROR;

    I2CPrivate_t *pI2CPrivate = I2CPrivateGet(i2cId);
    if(pI2CPrivate == NULL)
    {
        pI2CPrivate = HAL_Malloc(sizeof(I2CPrivate_t));
        if(pI2CPrivate == NULL)
            return HAL_STATUS_ERROR;

        I2CPrivateSet(i2cId, pI2CPrivate);
    }

    if (I2CHwInit(i2cId, i2CConfig) != HAL_STATUS_OK) {
        return HAL_STATUS_ERROR;
    }

    HAL_SemaphoreInit(pI2CPrivate->semaphore, 1, 1);
    HAL_MutexInit(pI2CPrivate->mutex);

    return HAL_STATUS_OK;
}


HAL_Status HAL_I2CDeInit(HAL_I2C_ID i2cId)
{
    if(i2cId >= HAL_I2C_NR)
        return HAL_STATUS_ERROR;

    I2CPrivate_t *pI2CPrivate = I2CPrivateGet(i2cId);
    if(pI2CPrivate == NULL)
        return HAL_STATUS_ERROR;

    I2CHwDeinit(i2cId, &pI2CPrivate->hi2c);

    HAL_SemaphoreDeinit(pI2CPrivate->semaphore);
    HAL_MutexDeinit(pI2CPrivate->mutex);

    HAL_Free(pI2CPrivate);
    I2CPrivateSet(i2cId, NULL);

    return HAL_STATUS_OK;
}


HAL_Status HAL_I2CTransmit(HAL_I2C_ID i2cId, uint16_t slaveAddr, uint8_t *pData, uint16_t size, uint32_t timeout)
{
    if(i2cId >= HAL_I2C_NR)
        return HAL_STATUS_ERROR;

    I2CPrivate_t *pI2CPrivate = I2CPrivateGet(i2cId);
    if(pI2CPrivate == NULL)
        return HAL_STATUS_ERROR;

    HAL_MutexLock(&pI2CPrivate->mutex, timeout);
    if (HAL_I2C_Master_Transmit(&pI2CPrivate->hi2c, slaveAddr, pData, size, timeout) != HAL_OK) {
        HAL_MutexUnlock(&pI2CPrivate->mutex);
        return HAL_STATUS_TIMEOUT;
    }
    HAL_MutexUnlock(&pI2CPrivate->mutex);

    return HAL_STATUS_OK;
}

HAL_Status HAL_I2CReceive(HAL_I2C_ID i2cId, uint16_t slaveAddr, uint8_t *pData, uint16_t size, uint32_t timeout)
{
    if(i2cId >= HAL_I2C_NR)
        return HAL_STATUS_ERROR;

    I2CPrivate_t *pI2CPrivate = I2CPrivateGet(i2cId);
    if(pI2CPrivate == NULL)
        return HAL_STATUS_ERROR;

    HAL_MutexLock(&pI2CPrivate->mutex, timeout);
    if (HAL_I2C_Master_Receive(&pI2CPrivate->hi2c, slaveAddr, pData, size, timeout) != HAL_OK) {
        HAL_MutexUnlock(&pI2CPrivate->mutex);
        return HAL_STATUS_TIMEOUT;
    }
    HAL_MutexUnlock(&pI2CPrivate->mutex);

    return HAL_STATUS_OK;
}