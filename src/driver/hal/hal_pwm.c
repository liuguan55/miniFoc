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
#include "driver/hal/hal_os.h"

typedef struct {
    TIM_HandleTypeDef *htim;
} PwmPrivate_t;

static PwmPrivate_t *pwmPrivate[HAL_PWM_ID_NR];

static TIM_TypeDef * const timInstance[HAL_PWM_ID_NR] = {
#if defined(TARGET_MCU_STM32F1) || defined(TARGET_MCU_STM32F4)
        [HAL_PWM_ID_2] = TIM2,
        [HAL_PWM_ID_3] = TIM3,
#elif defined(TARGET_MCU_STM32G0)
        [HAL_PWM_ID_1] = TIM1,
        [HAL_PWM_ID_3] = TIM3,
        [HAL_PWM_ID_14] = TIM14,
        [HAL_PWM_ID_16] = TIM16,
        [HAL_PWM_ID_17] = TIM17,
#endif
};

static void timClockEnable(HAL_PWM_ID ch) {
    switch (ch) {
        case HAL_PWM_ID_1:
#if defined(TARGET_MCU_STM32F1) || defined(TARGET_MCU_STM32F4)
            __HAL_RCC_TIM1_CLK_ENABLE();
#elif defined(TARGET_MCU_STM32G0)
            __HAL_RCC_TIM1_CLK_ENABLE();
#endif
            break;
        case HAL_PWM_ID_2:
            break;
        case HAL_PWM_ID_3:
            __HAL_RCC_TIM3_CLK_ENABLE();
            break;
        case HAL_PWM_ID_4:
            break;
        case HAL_PWM_ID_5:
            break;
        case HAL_PWM_ID_6:
            break;
        case HAL_PWM_ID_7:
            break;
        case HAL_PWM_ID_8:
            break;
        case HAL_PWM_ID_9:
            break;
        case HAL_PWM_ID_10:
            break;
        case HAL_PWM_ID_11:
            break;
        case HAL_PWM_ID_12:
            break;
        case HAL_PWM_ID_13:
            break;
        case HAL_PWM_ID_14:
#if defined(TARGET_MCU_STM32G0)
            __HAL_RCC_TIM14_CLK_ENABLE();
#endif
            break;
        case HAL_PWM_ID_15:
            break;
        case HAL_PWM_ID_16:
#if defined(TARGET_MCU_STM32G0)
            __HAL_RCC_TIM16_CLK_ENABLE();
#endif
            break;
        case HAL_PWM_ID_17:
#if defined(TARGET_MCU_STM32G0)
            __HAL_RCC_TIM17_CLK_ENABLE();
#endif
            break;
        default:
            MINI_ASSERT_MSG(0, "not support pwm channel");
            break;
    }
}

static void timClockDisable(HAL_PWM_ID ch) {
    switch (ch) {
        case HAL_PWM_ID_1:
#if defined(TARGET_MCU_STM32F1) || defined(TARGET_MCU_STM32F4)
            __HAL_RCC_TIM1_CLK_DISABLE();
#elif defined(TARGET_MCU_STM32G0)
            __HAL_RCC_TIM1_CLK_DISABLE();
#endif
            break;
        case HAL_PWM_ID_2:

            break;
        case HAL_PWM_ID_3:
            __HAL_RCC_TIM3_CLK_DISABLE();
            break;
        case HAL_PWM_ID_4:
            break;
        case HAL_PWM_ID_5:
            break;
        case HAL_PWM_ID_6:
            break;
        case HAL_PWM_ID_7:
            break;
        case HAL_PWM_ID_8:
            break;
        case HAL_PWM_ID_9:
            break;
        case HAL_PWM_ID_10:
            break;
        case HAL_PWM_ID_11:
            break;
        case HAL_PWM_ID_12:
            break;
        case HAL_PWM_ID_13:
            break;
        case HAL_PWM_ID_14:
#if defined(TARGET_MCU_STM32G0)
            __HAL_RCC_TIM14_CLK_DISABLE();
#endif
            break;
        case HAL_PWM_ID_15:
            break;
        case HAL_PWM_ID_16:
#if defined(TARGET_MCU_STM32G0)
            __HAL_RCC_TIM16_CLK_DISABLE();
#endif
            break;
        case HAL_PWM_ID_17:
#if defined(TARGET_MCU_STM32G0)
            __HAL_RCC_TIM17_CLK_DISABLE();
#endif
            break;
        default:
            assert(NULL);
    }
}


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

static uint32_t timGetChannel(HAL_PWM_CH channel){
    switch (channel){
        case HAL_PWM_CH1:
            return TIM_CHANNEL_1;
        case HAL_PWM_CH2:
            return TIM_CHANNEL_2;
        case HAL_PWM_CH3:
            return TIM_CHANNEL_3;
        case HAL_PWM_CH4:
            return TIM_CHANNEL_4;
#if defined(TARGET_MCU_STM32G0)
        case HAL_PWM_CH5:
            return TIM_CHANNEL_5;
        case HAL_PWM_CH6:
            return TIM_CHANNEL_6;
#endif
        default:
            assert(NULL);
    }
}

static TIM_TypeDef * PWMInstanceGet(HAL_PWM_ID id) {
    return timInstance[id];
}

HAL_Status HAL_pwmHwInit(HAL_PWM_ID id){

    PwmPrivate_t *pPwmPrivate = pwmPrivate[id];
    TIM_HandleTypeDef *pTim = pPwmPrivate->htim;
    if (pTim == NULL) {
        HAL_ERR("pwm %d not init", id);
        return HAL_STATUS_INVALID;
    }


    timClockEnable(id);
    HAL_BoardIoctl(HAL_BIR_PINMUX_INIT, HAL_MKDEV(HAL_DEV_MAJOR_PWM, id), 0);

    board_pwm_info_t boardPwmInfo;
    HAL_BoardIoctl(HAL_BIR_GET_CFG, HAL_MKDEV(HAL_DEV_MAJOR_PWM, id), (uint32_t)&boardPwmInfo);
    if(boardPwmInfo.count <= 0){
        printf("pwm %d not support\n", id);
        return HAL_STATUS_INVALID;
    }

    for (int i = 0; i < boardPwmInfo.count; ++i) {
        if (boardPwmInfo.config[i].id != id) {
            break;
        }

        TIM_MasterConfigTypeDef sMasterConfig = {0};
        TIM_OC_InitTypeDef sConfigOC = {0};

        pTim->Instance = PWMInstanceGet(id);
        pTim->Init.Prescaler = boardPwmInfo.config[i].prescaler;
        pTim->Init.CounterMode = timGetPwmMode(boardPwmInfo.config[i].mode);
        pTim->Init.Period = boardPwmInfo.config[i].period;
        pTim->Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
        pTim->Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
        if (HAL_TIM_Base_Init(pTim) != HAL_OK) {
            assert(NULL);
        }

        sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
        sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
        if (HAL_TIMEx_MasterConfigSynchronization(pTim, &sMasterConfig) != HAL_OK) {
            assert(NULL);
        }

        PWM_channel_t *channel = boardPwmInfo.config[i].channel;
        for (int j = 0; j < boardPwmInfo.config->channelNum; ++j) {
            sConfigOC.OCMode = TIM_OCMODE_PWM1;
            sConfigOC.Pulse = channel[j].duty;
            sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
            sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;

            uint32_t pwmChannel = timGetChannel(channel[j].channel);
            if (HAL_TIM_PWM_ConfigChannel(pTim, &sConfigOC, pwmChannel) != HAL_OK) {
                assert(NULL);
            }

            if (HAL_TIM_PWM_Start(pTim, pwmChannel) != HAL_OK) {
                assert(NULL);
            }
        }
    }
}

/**
 * @brief Pwm deinit
 * @param ch
 */
void HAL_pwmHwDeinit(HAL_PWM_ID ch){
    PwmPrivate_t *pPwmPrivate = pwmPrivate[ch];
    if (pPwmPrivate == NULL) {
        return;
    }

    TIM_HandleTypeDef *pTim = pPwmPrivate->htim;
    if (pTim == NULL) {
        return;
    }

    timClockDisable(ch);
    HAL_BoardIoctl(HAL_BIR_PINMUX_DEINIT, HAL_MKDEV(HAL_DEV_MAJOR_PWM, ch), 0);

    HAL_TIM_PWM_DeInit(pPwmPrivate->htim);
    HAL_TIM_Base_DeInit(pPwmPrivate->htim);
}



HAL_Status HAL_pwmInit(HAL_PWM_ID id) {
    PwmPrivate_t *pPwmPrivate = pwmPrivate[id];
    if (pPwmPrivate == NULL) {
        pPwmPrivate = (PwmPrivate_t *) HAL_Malloc(sizeof(PwmPrivate_t));
        if (pPwmPrivate == NULL) {
            return HAL_STATUS_MEM_FAILED;
        }
        pwmPrivate[id] = pPwmPrivate;
    }

    memset(pPwmPrivate, 0, sizeof(PwmPrivate_t));

    TIM_HandleTypeDef *pPwm = (TIM_HandleTypeDef *) HAL_Malloc(sizeof(TIM_HandleTypeDef));
    if (pPwm == NULL) {
        return HAL_STATUS_MEM_FAILED;
    }
    memset(pPwm, 0, sizeof(TIM_HandleTypeDef));
    pPwmPrivate->htim = pPwm;

    return HAL_pwmHwInit(id);
}

void HAL_pwmDeinit(HAL_PWM_ID id) {
    PwmPrivate_t *pPwmPrivate = pwmPrivate[id];
    if (pPwmPrivate == NULL) {
        return;
    }

    HAL_pwmHwDeinit(id);

    HAL_Free(pPwmPrivate->htim);
    HAL_Free(pPwmPrivate);
    pwmPrivate[id] = NULL;
}


HAL_Status HAL_pwmOutput(HAL_PWM_ID ch, HAL_PWM_CH channel, uint32_t value){
    PwmPrivate_t *pPwmPrivate = pwmPrivate[ch];
    if (pPwmPrivate == NULL) {
        return HAL_STATUS_MEM_FAILED;
    }

    TIM_HandleTypeDef *pTim = pPwmPrivate->htim;
    if (pTim == NULL) {
        return HAL_STATUS_MEM_FAILED;
    }

    board_pwm_info_t boardPwmInfo;
    HAL_BoardIoctl(HAL_BIR_GET_CFG, HAL_MKDEV(HAL_DEV_MAJOR_PWM, ch), (uint32_t)&boardPwmInfo);
    for (int i = 0; i < boardPwmInfo.count; ++i) {
        if (boardPwmInfo.config[i].id != ch) {
            break;
        }

        TIM_OC_InitTypeDef sConfigOC = {0};
        PWM_channel_t *chan = boardPwmInfo.config[i].channel;
        for (int j = 0; j < boardPwmInfo.config->channelNum; ++j) {
            if (chan[j].channel != channel) {
                continue;
            }

            uint32_t pwmChannel = timGetChannel(chan[j].channel);
            __HAL_TIM_SetCompare(pTim, pwmChannel, value);
        }

    }

    return HAL_STATUS_OK;
}