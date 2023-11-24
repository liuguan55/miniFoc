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
// Created by 86189 on 2023/11/24.
//

#ifndef MINIFOC_V1_HAL_CAN_H
#define MINIFOC_V1_HAL_CAN_H
#include <stdint.h>
#include "driver/hal/hal_os.h"

typedef enum {
    HAL_CAN_125K = 0,
    HAL_CAN_250K,
    HAL_CAN_500K,
    HAL_CAN_1000K,
} HAL_CAN_BAUDRATE;

typedef enum {
    HAL_CAN_MODE_NORMAL = 0,
    HAL_CAN_MODE_LOOPBACK,
    HAL_CAN_MODE_SILENT,
    HAL_CAN_MODE_SILENT_LOOPBACK,
} HAL_CAN_MODE;

typedef enum {
    HAL_CAN_0 = 0,
    HAL_CAN_1,
    HAL_CAN_NR,
}HAL_CAN_ID;

typedef struct {
    HAL_CAN_BAUDRATE baudrate;
    HAL_CAN_MODE mode;
    uint32_t filter_id;
    uint32_t filter_mask;
} CAN_CONFIG;

typedef struct {
    uint32_t id;
    uint8_t *data;
    uint8_t len;
    uint32_t timestamp;
} CAN_MSG;

typedef void (*CAN_RX_CALLBACK)(HAL_CAN_ID id, CAN_MSG *msg);

/**
 * @brief Initialize CAN module
 * @param id CAN module id
 * @param pconfig CAN configuration
 * @return HAL_OK if success
 */
HAL_Status HAL_CANinit(HAL_CAN_ID id, CAN_CONFIG *pconfig);

/**
 * @brief Deinitialize CAN module
 * @param id CAN module id
 * @return HAL_OK if success
 */
HAL_Status HAL_CANDeInit(HAL_CAN_ID id);

/**
 * @brief Set CAN receive callback
 * @param id CAN module id
 * @param callback callback function
 * @return HAL_OK if success
 */
HAL_Status HAL_CANSetRxCallback(HAL_CAN_ID id, CAN_RX_CALLBACK callback);

/**
 * @brief Send CAN message
 * @param id CAN module id
 * @param msg CAN message
 * @return <0 if error occurred, otherwise return the number of bytes sent
 */
int32_t HAL_CANSendMsg(HAL_CAN_ID id, CAN_MSG *msg);


#endif //MINIFOC_V1_HAL_CAN_H
