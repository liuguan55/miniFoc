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

#ifdef __cplusplus
extern "C" {
#endif

enum {
    HAL_PWM_ID_1 = 0,
    HAL_PWM_ID_2,
    HAL_PWM_ID_NR,
};
typedef  uint8_t HAL_PWM_ID;

enum {
    HAL_PWM_CH1 = 0,
    HAL_PWM_CH2,
    HAL_PWM_CH3,
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

/**
 * @brief   Pwm init
 * @param ch ID of specific pwm
 * @param cfg Pointer to pwm config
 * @return HAL_Status_ok if success, else error code
 */
HAL_Status HAL_pwmInit(HAL_PWM_ID ch, HAL_PWM_CFG *cfg);


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
 */
void HAL_pwmOutput(HAL_PWM_ID ch, uint32_t channel, uint32_t value);


#ifdef __cplusplus
};
#endif


#endif //MINIFOC_F4_HAL_PWM_H
