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
//
///* Includes ------------------------------------------------------------------*/
//#include "driver/dma.h"
//
///* USER CODE BEGIN 0 */
//
///* USER CODE END 0 */
//
///*----------------------------------------------------------------------------*/
///* Configure DMA                                                              */
///*----------------------------------------------------------------------------*/
//
///* USER CODE BEGIN 1 */
//
///* USER CODE END 1 */
//
///**
//  * Enable DMA controller clock
//  */
//void MX_DMA_Init(void) {
//
//  /* DMA controller clock enable */
//  __HAL_RCC_DMA2_CLK_ENABLE();
//
//  /* DMA interrupt init */
//    /* DMA2_Stream0_IRQn interrupt configuration */
//    HAL_NVIC_SetPriority(DMA2_Stream0_IRQn, 5, 0);
//    HAL_NVIC_EnableIRQ(DMA2_Stream0_IRQn);
//
//  /* DMA2_Stream3_IRQn interrupt configuration */
//  HAL_NVIC_SetPriority(DMA2_Stream3_IRQn, 6, 0);
//  HAL_NVIC_EnableIRQ(DMA2_Stream3_IRQn);
//  /* DMA2_Stream6_IRQn interrupt configuration */
//  HAL_NVIC_SetPriority(DMA2_Stream6_IRQn, 6, 0);
//  HAL_NVIC_EnableIRQ(DMA2_Stream6_IRQn);
//
//}
//
///* USER CODE BEGIN 2 */
//
///* USER CODE END 2 */
//
