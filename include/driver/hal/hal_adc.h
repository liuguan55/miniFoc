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

#ifndef MINIFOC_F4_HAL_ADC_H
#define MINIFOC_F4_HAL_ADC_H
#include "stdint.h"


#ifdef __cplusplus
extern "C" {
#endif

enum {
    HAL_ADC_1 = 0,
    HAL_ADC_2,
    HAL_ADC_NR,
};
typedef uint8_t HAL_ADC_ID;

enum {
    HAL_ADC_CH_0 = 0,
    HAL_ADC_CH_1,
    HAL_ADC_CH_2,
    HAL_ADC_CH_3,
    HAL_ADC_CH_4,
    HAL_ADC_CH_5,
    HAL_ADC_CH_6,
    HAL_ADC_CH_7,
    HAL_ADC_CH_NR,
};
typedef uint8_t HAL_ADC_CH;

/**
 * @brief adc init
 * @param id
 */
void HAL_adcInit(HAL_ADC_ID id);

/**
 * @brief adc deinit
 * @param id
 */
void HAL_adcDeinit(HAL_ADC_ID id);

/**
 * @brief adc read value from channel
 * @param id adc id
 * @param ch adc channel
 * @param msec timeout in millisecond
 * @return adc value
 */
uint16_t HAL_adcRead(HAL_ADC_ID id, HAL_ADC_CH ch, uint32_t msec);

#ifdef __cplusplus
}
#endif

#endif //MINIFOC_F4_HAL_ADC_H
