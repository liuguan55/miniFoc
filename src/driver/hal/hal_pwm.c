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

#include "driver/hal/hal_pwm.h"
#include "driver/driver_chip.h"
#include "sys/MiniDebug.h"
#include "driver/hal/hal_board.h"
#include "driver/hal/hal_dev.h"

TIM_HandleTypeDef htim2;

static TIM_TypeDef * const timHandle[HAL_PWM_ID_NR] = {
        TIM2,
        TIM3,
};

static uint32_t timGetPwmMode(HAL_PWM_COUNTER_MODE mode){
    switch (mode){
        case HAL_PWM_COUNTER_MODE_CENTER:
            return TIM_COUNTERMODE_CENTERALIGNED2;
        case HAL_PWM_COUNTER_MODE_UP:
            return TIM_COUNTERMODE_UP;
        case HAL_PWM_COUNTER_MODE_DOWN:
            return TIM_COUNTERMODE_DOWN;
        default:
            assert(NULL);
    }
}

HAL_Status HAL_pwmInit(HAL_PWM_ID ch, HAL_PWM_CFG *cfg){
    __HAL_RCC_TIM2_CLK_ENABLE();

    HAL_BoardIoctl(HAL_BIR_PINMUX_INIT, HAL_MKDEV(HAL_DEV_MAJOR_PWM, ch), 0);
    TIM_ClockConfigTypeDef sClockSourceConfig = {0};
    TIM_MasterConfigTypeDef sMasterConfig = {0};
    TIM_OC_InitTypeDef sConfigOC = {0};

    /* USER CODE BEGIN TIM2_Init 1 */

    /* USER CODE END TIM2_Init 1 */
    htim2.Instance = timHandle[ch];
    htim2.Init.Prescaler = 83;
    htim2.Init.CounterMode = timGetPwmMode(cfg->mode);
    htim2.Init.Period = cfg->period;
    htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
    if (HAL_TIM_Base_Init(&htim2) != HAL_OK) {
        assert(NULL);
    }
    sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
    if (HAL_TIM_ConfigClockSource(&htim2, &sClockSourceConfig) != HAL_OK) {
        assert(NULL);
    }
    if (HAL_TIM_PWM_Init(&htim2) != HAL_OK) {
        assert(NULL);
    }
    sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
    sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
    if (HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig) != HAL_OK) {
        assert(NULL);
    }
    sConfigOC.OCMode = TIM_OCMODE_PWM1;
    sConfigOC.Pulse = 0;
    sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
    sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
    if (HAL_TIM_PWM_ConfigChannel(&htim2, &sConfigOC, TIM_CHANNEL_2) != HAL_OK) {
        assert(NULL);
    }
    if (HAL_TIM_PWM_ConfigChannel(&htim2, &sConfigOC, TIM_CHANNEL_3) != HAL_OK) {
        assert(NULL);
    }
    if (HAL_TIM_PWM_ConfigChannel(&htim2, &sConfigOC, TIM_CHANNEL_4) != HAL_OK) {
        assert(NULL);
    }
}

/**
 * @brief Pwm deinit
 * @param ch
 */
void HAL_pwmDeinit(HAL_PWM_ID ch){
    HAL_TIM_PWM_DeInit(&htim2);
    HAL_TIM_Base_DeInit(&htim2);
    __HAL_RCC_TIM2_CLK_DISABLE();
    HAL_BoardIoctl(HAL_BIR_PINMUX_DEINIT, HAL_MKDEV(HAL_DEV_MAJOR_PWM, ch), 0);
}

void HAL_pwmOutput(HAL_PWM_ID ch, uint32_t channel, uint32_t value){
    TIM_OC_InitTypeDef sConfigOC = {0};
    sConfigOC.OCMode = TIM_OCMODE_PWM1;
    sConfigOC.Pulse = value;
    sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
    sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
    switch (channel){
        case 2:
            if (HAL_TIM_PWM_ConfigChannel(&htim2, &sConfigOC, TIM_CHANNEL_2) != HAL_OK) {
                assert(NULL);
            }
            break;
        case 3:
            if (HAL_TIM_PWM_ConfigChannel(&htim2, &sConfigOC, TIM_CHANNEL_3) != HAL_OK) {
                assert(NULL);
            }
            break;
        case 4:
            if (HAL_TIM_PWM_ConfigChannel(&htim2, &sConfigOC, TIM_CHANNEL_4) != HAL_OK) {
                assert(NULL);
            }
            break;
        default:
            assert(NULL);
    }
    if (HAL_TIM_PWM_Start(&htim2, channel) != HAL_OK) {
        assert(NULL);
    }
}