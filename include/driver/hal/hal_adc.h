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
#include "driver/driver_chip.h"


#ifdef __cplusplus
extern "C" {
#endif

enum {
    HAL_ADC_1 = 0,
    HAL_ADC_2,
    HAL_ADC_3,
    HAL_ADC_4,
    HAL_ADC_5,
    HAL_ADC_6,
    HAL_ADC_7,
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


typedef struct {
    uint8_t  channel;
    uint32_t rank;
    uint8_t  sampleTime;
}ADC_Channel_t;

typedef struct {
    ADC_Channel_t *channels;
    uint32_t channelCount;
    uint32_t externalTrigConv;
    TIM_TypeDef *timer;
    uint32_t timerChannel;
    uint32_t prescaler;
    uint32_t period;
    DMA_Channel_TypeDef *dmaChannel;
}ADC_Config_t;

typedef struct  {
    ADC_Config_t *config;
    uint32_t count;
}board_adc_info_t;

typedef void (*HAL_ADC_Callback_t)(uint16_t *data, uint32_t size);

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
 * @brief adc start
 * @param id
 * @return
 */
int8_t HAL_adcStart(HAL_ADC_ID id);

/**
 * @brief adc stop
 * @param id
 * @return
 */
int8_t HAL_adcStop(HAL_ADC_ID id);
/**
 * @brief adc read value from channel
 * @param id adc id
 * @param ch adc channel
 * @param msec timeout in millisecond
 * @return adc value
 */
uint16_t HAL_adcRead(HAL_ADC_ID id, HAL_ADC_CH ch, uint32_t msec);

/**
 * @brief read adc channel length
 * @param id adc id
 * @return adc channel length
 */
uint32_t HAL_adcGetChannelCount(HAL_ADC_ID id);

/**
 * @brief adc read value from channel
 * @param id adc id
 * @param ch adc channel
 * @param msec timeout in millisecond
 * @return adc value
 */
int32_t HAL_adcReadMulti(HAL_ADC_ID id, uint16_t *buffer, uint16_t size, uint32_t msec);

/**
 * @brief set adc callback
 * @param id adc id
 * @param callback adc callback
 */
void HAL_adcSetCallback(HAL_ADC_ID id, HAL_ADC_Callback_t callback);
#ifdef __cplusplus
}
#endif

#endif //MINIFOC_F4_HAL_ADC_H
