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
//#include "driver/iwdg.h"
//
///* USER CODE BEGIN 0 */
//
///* USER CODE END 0 */
//
//IWDG_HandleTypeDef hiwdg;
//
///* IWDG init function */
//void MX_IWDG_Init(void) {
//
//  /* USER CODE BEGIN IWDG_Init 0 */
//
//  /* USER CODE END IWDG_Init 0 */
//
//  /* USER CODE BEGIN IWDG_Init 1 */
//
//  /* USER CODE END IWDG_Init 1 */
//  hiwdg.Instance = IWDG;
//  hiwdg.Init.Prescaler = IWDG_PRESCALER_4;
//  hiwdg.Init.Reload = 4095;
//  if (HAL_IWDG_Init(&hiwdg) != HAL_OK) {
//    Error_Handler();
//  }
//  /* USER CODE BEGIN IWDG_Init 2 */
//
//  /* USER CODE END IWDG_Init 2 */
//
//}
//
///* USER CODE BEGIN 1 */
//
///* USER CODE END 1 */
