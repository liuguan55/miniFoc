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
// Created by 86189 on 2023/6/9.
//

#ifndef MINIFOC_F4_HAL_CRC_H
#define MINIFOC_F4_HAL_CRC_H
#include "stdint.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief initialize crc peripheral
 */
void hal_crcInit(void);

/**
 * @brief deinitialize crc peripheral
 */
void hal_crcDeinit(void);

/**
 * @brief calculate crc32 of uint32_t data
 * @param data pointer to the input data buffer.
 * @param dataSize pointer to the input data size.
 * @return crc32 value
 */
uint32_t hal_crc32(uint32_t *data, uint32_t dataSize);

/**
 * @brief calculate crc8 of uint8_t data
 * @param data pointer to the input data buffer.
 * @param dataSize pointer to the input data size.
 * @return crc8 value
 */
uint8_t hal_crc8(uint8_t *data, uint32_t dataSize);

#ifdef __cplusplus
}
#endif
#endif //MINIFOC_F4_HAL_CRC_H
