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
//#include "driver/crc.h"
//
///* USER CODE BEGIN 0 */
//
///* USER CODE END 0 */
//
//CRC_HandleTypeDef hcrc;
//
///* CRC init function */
//void MX_CRC_Init(void) {
//
//  /* USER CODE BEGIN CRC_Init 0 */
//
//  /* USER CODE END CRC_Init 0 */
//
//  /* USER CODE BEGIN CRC_Init 1 */
//
//  /* USER CODE END CRC_Init 1 */
//  hcrc.Instance = CRC;
//  if (HAL_CRC_Init(&hcrc) != HAL_OK) {
//    Error_Handler();
//  }
//  /* USER CODE BEGIN CRC_Init 2 */
//
//  /* USER CODE END CRC_Init 2 */
//
//}
//
//void HAL_CRC_MspInit(CRC_HandleTypeDef *crcHandle) {
//
//  if (crcHandle->Instance == CRC) {
//    /* USER CODE BEGIN CRC_MspInit 0 */
//
//    /* USER CODE END CRC_MspInit 0 */
//    /* CRC clock enable */
//    __HAL_RCC_CRC_CLK_ENABLE();
//    /* USER CODE BEGIN CRC_MspInit 1 */
//
//    /* USER CODE END CRC_MspInit 1 */
//  }
//}
//
//void HAL_CRC_MspDeInit(CRC_HandleTypeDef *crcHandle) {
//
//  if (crcHandle->Instance == CRC) {
//    /* USER CODE BEGIN CRC_MspDeInit 0 */
//
//    /* USER CODE END CRC_MspDeInit 0 */
//    /* Peripheral clock disable */
//    __HAL_RCC_CRC_CLK_DISABLE();
//    /* USER CODE BEGIN CRC_MspDeInit 1 */
//
//    /* USER CODE END CRC_MspDeInit 1 */
//  }
//}
//
///* USER CODE BEGIN 1 */
//
///* USER CODE END 1 */
