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
// Created by 86189 on 2023/5/29.
//

#ifndef MINIFOC_F4_HAL_PWM_H
#define MINIFOC_F4_HAL_PWM_H

#include "stdint.h"
#include "driver/hal/hal_def.h"
#include "driver/driver_chip.h"

#ifdef __cplusplus
extern "C" {
#endif

enum {
    HAL_PWM_ID_1 = 0,
    HAL_PWM_ID_2,
    HAL_PWM_ID_3,
    HAL_PWM_ID_4,
    HAL_PWM_ID_5,
    HAL_PWM_ID_6,
    HAL_PWM_ID_7,
    HAL_PWM_ID_8,
    HAL_PWM_ID_9,
    HAL_PWM_ID_10,
    HAL_PWM_ID_11,
    HAL_PWM_ID_12,
    HAL_PWM_ID_13,
    HAL_PWM_ID_14,
    HAL_PWM_ID_15,
    HAL_PWM_ID_16,
    HAL_PWM_ID_17,
    HAL_PWM_ID_NR,
};
typedef  uint8_t HAL_PWM_ID;

enum {
    HAL_PWM_CH1 = 0,
    HAL_PWM_CH2,
    HAL_PWM_CH3,
    HAL_PWM_CH4,
    HAL_PWM_CH5,
    HAL_PWM_CH6,
    HAL_PWM_CH7,
    HAL_PWM_CH8,
    HAL_PWM_CH_NR,
};
typedef uint8_t HAL_PWM_CH;

enum {
    HAL_PWM_COUNTER_MODE_CENTER= 0,
    HAL_PWM_COUNTER_MODE_UP,
    HAL_PWM_COUNTER_MODE_DOWN,
    HAL_PWM_COUNTER_MODE_NR,
};
typedef uint8_t HAL_PWM_COUNTER_MODE;

typedef struct {
    uint32_t period; // unit: us
    uint32_t duty;
    HAL_PWM_COUNTER_MODE mode;
} HAL_PWM_CFG;

typedef struct{
    HAL_PWM_CH channel;
    uint32_t duty;
}PWM_channel_t;

typedef struct {
    HAL_PWM_ID id;
    PWM_channel_t *channel;
    uint8_t channelNum;
    HAL_PWM_COUNTER_MODE mode;
    uint32_t prescaler;
    uint32_t period;
}PWM_Config_t;

typedef struct  {
    PWM_Config_t *config;
    uint32_t count;
}board_pwm_info_t;
/**
 * @brief   Pwm init
 * @param ch ID of specific pwm
 * @return HAL_Status_ok if success, else error code
 */
HAL_Status HAL_pwmInit(HAL_PWM_ID id);


/**
 * @brief Pwm deinit
 * @param ch
 */
void HAL_pwmDeinit(HAL_PWM_ID ch);

/**
 * @brief configutation pwm output
 * @param ch ID of pwm
 * @param channel channel of pwm
 * @param value uint32_t value of pwm
* @return HAL_Status_ok if success, else error code
 */
HAL_Status HAL_pwmOutput(HAL_PWM_ID ch, HAL_PWM_CH channel, uint32_t value);


#ifdef __cplusplus
};
#endif


#endif //MINIFOC_F4_HAL_PWM_H

