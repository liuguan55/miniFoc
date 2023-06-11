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
//#include "kernel/FreeRTOS/FreeRTOS.h"
//#include "kernel/FreeRTOS/task.h"
//#include "driver/main.h"
//#include "kernel/FreeRTOS/CMSIS_RTOS_V2/cmsis_os.h"
//#include "common/framework/platform_init.h"
//#include "common/framework/MiniCommon.h"
//#include "apps/BldcController/app.h"
///* Private includes ----------------------------------------------------------*/
///* USER CODE BEGIN Includes */
//
///* USER CODE END Includes */
//
///* Private typedef -----------------------------------------------------------*/
///* USER CODE BEGIN PTD */
//
///* USER CODE END PTD */
//
///* Private define ------------------------------------------------------------*/
///* USER CODE BEGIN PD */
///* USER CODE END Variables */
///* Private function prototypes -----------------------------------------------*/
///* USER CODE BEGIN FunctionPrototypes */
//
///* USER CODE END PD */
//
///* Private macro -------------------------------------------------------------*/
///* USER CODE BEGIN PM */
///* USER CODE END PM */
//
///* Private variables ---------------------------------------------------------*/
///* USER CODE BEGIN Variables */
//
///* USER CODE END Variables */
///* Definitions for defaultTask */
//osThreadId_t appTaskHandle;
//const osThreadAttr_t appAttributes = {
//        .name = "app",
//        .stack_size = 512 * 4,
//        .priority = (osPriority_t) osPriorityNormal,
//};
//
///* Private function prototypes -----------------------------------------------*/
///* USER CODE BEGIN FunctionPrototypes */
//
///* USER CODE END FunctionPrototypes */
//
//extern void MX_USB_DEVICE_Init(void);
//
//
///* USER CODE BEGIN Header_StartDefaultTask */
///**
//  * @brief  Function implementing the defaultTask thread.
//  * @param  argument: Not used
//  * @retval None
//  */
///* USER CODE END Header_StartDefaultTask */
//void StartApptTask(void *argument) {
//    UNUSED(argument);
//    /* init code for USB_DEVICE */
//
//    /* USER CODE BEGIN StartDefaultTask */
//    platform_init();
//
////    app_init();
//    /* Infinite loop */
//    for (;;) {
//        MiniCommon_led1Toggle();
//        MiniCommon_delayMs(1000);
//    }
//
//
//    /* USER CODE END StartDefaultTask */
//}
//
///**
//  * @brief  FreeRTOS initialization
//  * @param  None
//  * @retval None
//  */
//void MX_FREERTOS_Init(void) {
//    appTaskHandle = osThreadNew(StartApptTask, NULL, &appAttributes);
//}
//
//
///* USER CODE END Application */
//
