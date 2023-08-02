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
// Created by 86189 on 2023/5/30.
//

#include "driver/hal/hal_timer.h"
#include "driver/driver_chip.h"
#include "kernel/FreeRTOS/FreeRTOS.h"
#include "kernel/FreeRTOS/CMSIS_RTOS_V2/cmsis_os2.h"
#include "driver/hal/hal_def.h"
#include "sys/MiniDebug.h"

TIM_HandleTypeDef htim4;
volatile long long FreeRTOSRunTimeTicks;

/**
  * @brief This function handles TIM4 global interrupt.
  */
void TIM4_IRQHandler(void) {
    /* USER CODE BEGIN TIM4_IRQn 0 */

    /* USER CODE END TIM4_IRQn 0 */
    HAL_TIM_IRQHandler(&htim4);
    /* USER CODE BEGIN TIM4_IRQn 1 */

    /* USER CODE END TIM4_IRQn 1 */
}

HAL_Status HAL_timerInit(void){

    __HAL_RCC_TIM4_CLK_ENABLE();

    TIM_ClockConfigTypeDef sClockSourceConfig = {0};
    TIM_MasterConfigTypeDef sMasterConfig = {0};

    /* USER CODE BEGIN TIM4_Init 1 */

    /* USER CODE END TIM4_Init 1 */
    htim4.Instance = TIM4;
    htim4.Init.Prescaler = 83;
    htim4.Init.CounterMode = TIM_COUNTERMODE_UP;
    htim4.Init.Period = 49;
    htim4.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    htim4.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
    if (HAL_TIM_Base_Init(&htim4) != HAL_OK) {
        assert(NULL);
    }
    sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
    if (HAL_TIM_ConfigClockSource(&htim4, &sClockSourceConfig) != HAL_OK) {
        assert(NULL);
    }
    sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
    sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
    if (HAL_TIMEx_MasterConfigSynchronization(&htim4, &sMasterConfig) != HAL_OK) {
        assert(NULL);
    }
    /* USER CODE BEGIN TIM4_Init 2 */
    HAL_TIM_Base_Start_IT(&htim4);
    /* USER CODE END TIM4_Init 2 */

    HAL_NVIC_SetPriority(TIM4_IRQn, 3, 0);
    HAL_NVIC_EnableIRQ(TIM4_IRQn);
}


void HAl_timerDeinit(void){
    __HAL_RCC_TIM4_CLK_DISABLE();
    HAL_TIM_Base_DeInit(&htim4);

    HAL_NVIC_DisableIRQ(TIM4_IRQn);
}

static void HAL_timerCallback(void){
    FreeRTOSRunTimeTicks++;

    static int timerCnt = 0;

    if (++timerCnt >= 20){
        timerCnt = 0;
        HAL_IncTick();
    }
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim){
    if(htim->Instance == TIM4){
        HAL_timerCallback();
    }
}


uint32_t HAL_millis(void){
    return osKernelGetTickCount();
}

/**
 * @brief Get the elapesd millis from start
 * @param start
 * @return
 */
uint32_t HAL_elapesdMillis(uint32_t start){
    return HAL_millis() - start;
}

/**
 * @brief Delay ms
 * @param ms
 */
void HAL_msleep(uint32_t ms){
    osDelay(ms);
}

/**
 * @brief Delay s
 * @param s
 */
void HAL_sleep(uint32_t s){
    osDelay(s * 1000);
}


void ConfigureTimeForRunTimeStats(void) {
    FreeRTOSRunTimeTicks = 0;
}

