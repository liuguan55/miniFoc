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
//#include "driver/adc.h"
//
///* USER CODE BEGIN 0 */
//
///* USER CODE END 0 */
//
//ADC_HandleTypeDef hadc1;
//
///* ADC1 init function */
//void MX_ADC1_Init(void) {
//
//  /* USER CODE BEGIN ADC1_Init 0 */
//
//  /* USER CODE END ADC1_Init 0 */
//
//  ADC_ChannelConfTypeDef sConfig = {0};
//
//  /* USER CODE BEGIN ADC1_Init 1 */
//
//  /* USER CODE END ADC1_Init 1 */
//
//  /** Configure the global features of the ADC (Clock, Resolution, Data Alignment and number of conversion)
//  */
//  hadc1.Instance = ADC1;
//  hadc1.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV4;
//  hadc1.Init.Resolution = ADC_RESOLUTION_12B;
//  hadc1.Init.ScanConvMode = DISABLE;
//  hadc1.Init.ContinuousConvMode = DISABLE;
//  hadc1.Init.DiscontinuousConvMode = DISABLE;
//  hadc1.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
//  hadc1.Init.ExternalTrigConv = ADC_SOFTWARE_START;
//  hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
//  hadc1.Init.NbrOfConversion = 1;
//  hadc1.Init.DMAContinuousRequests = DISABLE;
//  hadc1.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
//  if (HAL_ADC_Init(&hadc1) != HAL_OK) {
//    Error_Handler();
//  }
//
//  /** Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time.
//  */
//  sConfig.Channel = ADC_CHANNEL_4;
//  sConfig.Rank = 1;
//  sConfig.SamplingTime = ADC_SAMPLETIME_3CYCLES;
//  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK) {
//    Error_Handler();
//  }
//  /* USER CODE BEGIN ADC1_Init 2 */
//  /* USER CODE END ADC1_Init 2 */
//
//}
//
//void HAL_ADC_MspInit(ADC_HandleTypeDef *adcHandle) {
//
//  GPIO_InitTypeDef GPIO_InitStruct = {0};
//  if (adcHandle->Instance == ADC1) {
//    /* USER CODE BEGIN ADC1_MspInit 0 */
//
//    /* USER CODE END ADC1_MspInit 0 */
//    /* ADC1 clock enable */
//    __HAL_RCC_ADC1_CLK_ENABLE();
//
//    __HAL_RCC_GPIOA_CLK_ENABLE();
//    /**ADC1 GPIO Configuration
//    PA4     ------> ADC1_IN4
//    */
//    GPIO_InitStruct.Pin = GPIO_PIN_4;
//    GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
//    GPIO_InitStruct.Pull = GPIO_NOPULL;
//    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
//
//    /* USER CODE BEGIN ADC1_MspInit 1 */
//
//    /* USER CODE END ADC1_MspInit 1 */
//  }
//}
//
//void HAL_ADC_MspDeInit(ADC_HandleTypeDef *adcHandle) {
//
//  if (adcHandle->Instance == ADC1) {
//    /* USER CODE BEGIN ADC1_MspDeInit 0 */
//
//    /* USER CODE END ADC1_MspDeInit 0 */
//    /* Peripheral clock disable */
//    __HAL_RCC_ADC1_CLK_DISABLE();
//
//    /**ADC1 GPIO Configuration
//    PA4     ------> ADC1_IN4
//    */
//    HAL_GPIO_DeInit(GPIOA, GPIO_PIN_4);
//
//    /* USER CODE BEGIN ADC1_MspDeInit 1 */
//
//    /* USER CODE END ADC1_MspDeInit 1 */
//  }
//}
//
///* USER CODE BEGIN 1 */
//
//unsigned short analogRead(unsigned char ch, uint32_t SamplingTime, int timeout) {
//  ADC_ChannelConfTypeDef ADC1_ChanConf;
//
//  ADC1_ChanConf.Channel = ch;                                   //通道
//  ADC1_ChanConf.Rank = 1;                                       //第1个序列，序列1
//  ADC1_ChanConf.SamplingTime = SamplingTime;      //采样时间
//  HAL_ADC_ConfigChannel(&hadc1, &ADC1_ChanConf);        //通道配置
//
//  HAL_ADC_Start(&hadc1);                               //开启ADC
//
//  HAL_ADC_PollForConversion(&hadc1, timeout);
//
//  return (uint16_t)HAL_ADC_GetValue(&hadc1);
//}
///* USER CODE END 1 */
