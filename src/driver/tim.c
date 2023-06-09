//// /* Copyright (C) 2023 dengzhijun. All rights reserved.
////  *
////  *  Redistribution and use in source and binary forms, with or without
////  *  modification, are permitted provided that the following conditions
////  *  are met:
////  *    1. Redistributions of source code must retain the above copyright
////  *       notice, this list of conditions and the following disclaimer.
////  *    2. Redistributions in binary form must reproduce the above copyright
////  *       notice, this list of conditions and the following disclaimer in the
////  *       documentation and/or other materials provided with the
////  *       distribution.
////  *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
////  *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
////  *  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
////  *  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
////  *  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
////  *  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
////  *  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
////  *  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
////  *  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
////  *  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
////  *  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
////  *
//
///* USER CODE BEGIN Header */
///* USER CODE END Header */
///* Includes ------------------------------------------------------------------*/
//#include "driver/tim.h"
//
///* USER CODE BEGIN 0 */
////freertos 时钟需要的时钟定时器
//volatile long long FreeRTOSRunTimeTicks;
///* USER CODE END 0 */
//
//TIM_HandleTypeDef htim2;
//TIM_HandleTypeDef htim4;
//
///* TIM2 init function */
//void MX_TIM2_Init(void) {
//
//  /* USER CODE BEGIN TIM2_Init 0 */
//
//  /* USER CODE END TIM2_Init 0 */
//
//  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
//  TIM_MasterConfigTypeDef sMasterConfig = {0};
//  TIM_OC_InitTypeDef sConfigOC = {0};
//
//  /* USER CODE BEGIN TIM2_Init 1 */
//
//  /* USER CODE END TIM2_Init 1 */
//  htim2.Instance = TIM2;
//  htim2.Init.Prescaler = 0;
//  htim2.Init.CounterMode = TIM_COUNTERMODE_CENTERALIGNED2;
//  htim2.Init.Period = 4199;
//  htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
//  htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
//  if (HAL_TIM_Base_Init(&htim2) != HAL_OK) {
//    Error_Handler();
//  }
//  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
//  if (HAL_TIM_ConfigClockSource(&htim2, &sClockSourceConfig) != HAL_OK) {
//    Error_Handler();
//  }
//  if (HAL_TIM_PWM_Init(&htim2) != HAL_OK) {
//    Error_Handler();
//  }
//  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
//  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
//  if (HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig) != HAL_OK) {
//    Error_Handler();
//  }
//  sConfigOC.OCMode = TIM_OCMODE_PWM1;
//  sConfigOC.Pulse = 0;
//  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
//  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
//  if (HAL_TIM_PWM_ConfigChannel(&htim2, &sConfigOC, TIM_CHANNEL_2) != HAL_OK) {
//    Error_Handler();
//  }
//  if (HAL_TIM_PWM_ConfigChannel(&htim2, &sConfigOC, TIM_CHANNEL_3) != HAL_OK) {
//    Error_Handler();
//  }
//  if (HAL_TIM_PWM_ConfigChannel(&htim2, &sConfigOC, TIM_CHANNEL_4) != HAL_OK) {
//    Error_Handler();
//  }
//  /* USER CODE BEGIN TIM2_Init 2 */
//
//  /* USER CODE END TIM2_Init 2 */
//  HAL_TIM_MspPostInit(&htim2);
//}
///* TIM4 init function */
//void MX_TIM4_Init(void) {
//
//  /* USER CODE BEGIN TIM4_Init 0 */
//
//  /* USER CODE END TIM4_Init 0 */
//
//  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
//  TIM_MasterConfigTypeDef sMasterConfig = {0};
//
//  /* USER CODE BEGIN TIM4_Init 1 */
//
//  /* USER CODE END TIM4_Init 1 */
//  htim4.Instance = TIM4;
//  htim4.Init.Prescaler = 83;
//  htim4.Init.CounterMode = TIM_COUNTERMODE_UP;
//  htim4.Init.Period = 49;
//  htim4.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
//  htim4.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
//  if (HAL_TIM_Base_Init(&htim4) != HAL_OK) {
//    Error_Handler();
//  }
//  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
//  if (HAL_TIM_ConfigClockSource(&htim4, &sClockSourceConfig) != HAL_OK) {
//    Error_Handler();
//  }
//  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
//  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
//  if (HAL_TIMEx_MasterConfigSynchronization(&htim4, &sMasterConfig) != HAL_OK) {
//    Error_Handler();
//  }
//  /* USER CODE BEGIN TIM4_Init 2 */
//  HAL_TIM_Base_Start_IT(&htim4);
//  /* USER CODE END TIM4_Init 2 */
//
//}
//
//void HAL_TIM_Base_MspInit(TIM_HandleTypeDef *tim_baseHandle) {
//
//  if (tim_baseHandle->Instance == TIM2) {
//    /* USER CODE BEGIN TIM2_MspInit 0 */
//
//    /* USER CODE END TIM2_MspInit 0 */
//    /* TIM2 clock enable */
//    __HAL_RCC_TIM2_CLK_ENABLE();
//    /* USER CODE BEGIN TIM2_MspInit 1 */
//
//    /* USER CODE END TIM2_MspInit 1 */
//  } else if (tim_baseHandle->Instance == TIM4) {
//    /* USER CODE BEGIN TIM4_MspInit 0 */
//
//    /* USER CODE END TIM4_MspInit 0 */
//    /* TIM4 clock enable */
//    __HAL_RCC_TIM4_CLK_ENABLE();
//
//    /* TIM4 interrupt Init */
//    HAL_NVIC_SetPriority(TIM4_IRQn, 3, 0);
//    HAL_NVIC_EnableIRQ(TIM4_IRQn);
//    /* USER CODE BEGIN TIM4_MspInit 1 */
//
//    /* USER CODE END TIM4_MspInit 1 */
//  }
//}
//void HAL_TIM_MspPostInit(TIM_HandleTypeDef *timHandle) {
//
//  GPIO_InitTypeDef GPIO_InitStruct = {0};
//  if (timHandle->Instance == TIM2) {
//    /* USER CODE BEGIN TIM2_MspPostInit 0 */
//
//    /* USER CODE END TIM2_MspPostInit 0 */
//
//    __HAL_RCC_GPIOA_CLK_ENABLE();
//    /**TIM2 GPIO Configuration
//    PA1     ------> TIM2_CH2
//    PA2     ------> TIM2_CH3
//    PA3     ------> TIM2_CH4
//    */
//    GPIO_InitStruct.Pin = GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3;
//    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
//    GPIO_InitStruct.Pull = GPIO_NOPULL;
//    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
//    GPIO_InitStruct.Alternate = GPIO_AF1_TIM2;
//    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
//
//    /* USER CODE BEGIN TIM2_MspPostInit 1 */
//
//    /* USER CODE END TIM2_MspPostInit 1 */
//  }
//
//}
//
//void HAL_TIM_Base_MspDeInit(TIM_HandleTypeDef *tim_baseHandle) {
//
//  if (tim_baseHandle->Instance == TIM2) {
//    /* USER CODE BEGIN TIM2_MspDeInit 0 */
//
//    /* USER CODE END TIM2_MspDeInit 0 */
//    /* Peripheral clock disable */
//    __HAL_RCC_TIM2_CLK_DISABLE();
//    /* USER CODE BEGIN TIM2_MspDeInit 1 */
//
//    /* USER CODE END TIM2_MspDeInit 1 */
//  } else if (tim_baseHandle->Instance == TIM4) {
//    /* USER CODE BEGIN TIM4_MspDeInit 0 */
//
//    /* USER CODE END TIM4_MspDeInit 0 */
//    /* Peripheral clock disable */
//    __HAL_RCC_TIM4_CLK_DISABLE();
//
//    /* TIM4 interrupt Deinit */
//    HAL_NVIC_DisableIRQ(TIM4_IRQn);
//    /* USER CODE BEGIN TIM4_MspDeInit 1 */
//
//    /* USER CODE END TIM4_MspDeInit 1 */
//  }
//}
//
///* USER CODE BEGIN 1 */
//void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
//  if (htim->Instance == TIM4) {
//    FreeRTOSRunTimeTicks++;
//
//    static int timerCnt = 0;
//
//    if (++timerCnt >= 20){
//      timerCnt = 0;
//      HAL_IncTick();
//    }
//  }
//}
//
//void ConfigureTimeForRunTimeStats(void) {
//  FreeRTOSRunTimeTicks = 0;
//}
///* USER CODE END 1 */
