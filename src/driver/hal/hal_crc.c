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

#include "driver/hal/hal_crc.h"
#include "driver/driver_chip.h"
#include "sys/MiniDebug.h"

CRC_HandleTypeDef hcrc;


void hal_crcInit(void) {
    hcrc.Instance = CRC;
    if (HAL_CRC_Init(&hcrc) != HAL_OK) {
        assert(NULL);
    }
}


void hal_crcDeinit(void) {
    HAL_CRC_DeInit(&hcrc);
}

uint32_t hal_crc32(uint32_t *data, uint32_t dataSize) {
    HAL_CRC_Calculate(&hcrc, data, dataSize);
}


uint8_t hal_crc8(uint8_t *data, uint32_t dataSize) {
    uint8_t crc = 0xFF;
    uint8_t i, j;

    for (j = dataSize; j; j--, data++)
    {
        crc ^= *data;
        for (i = 8; i; i--)
        {
            crc = (crc & 0x80) ? (crc << 1) ^ 0x31 : (crc << 1);
        }
    }

    return crc;
}

uint16_t hal_crc16(uint16_t *data, uint32_t len) {
    uint16_t crc = 0xFFFF;
    uint16_t i;

    if (len == 0)
        return (~crc);

    do
    {
        for (i = 0, crc ^= ((uint16_t)data[i] << 8); i < 8; i++)
        {
            if (crc & 0x8000)
                crc = (crc << 1) ^ 0x1021;
            else
                crc <<= 1;
        }
    } while (--len);

    crc = ~crc;
    crc = (uint16_t)((crc << 8) | (crc >> 8)); // Reverses the byte order of the CRC.
    return crc;
}
