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

/* USER CODE BEGIN Header */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "driver/usart.h"

/* USER CODE BEGIN 0 */
#include "core/lwrb/lwrb.h"

#define USART_RXBUFFER_SIZE 1
#define LWRB_BUFFER_SIZE  64

static uint8_t usart1_Rxbuffer[USART_RXBUFFER_SIZE] = {0};

static lwrb_t usart1_lwrbHandle;
static uint8_t usart1_lwrbBuffer[LWRB_BUFFER_SIZE] = {0};

lwrb_t *Mini_getlwrbHandle(uint8_t usart_num) {
    return &usart1_lwrbHandle;
}

/* USER CODE END 0 */

UART_HandleTypeDef huart1;

/* USART1 init function */

void MX_USART1_UART_Init(void) {

    /* USER CODE BEGIN USART1_Init 0 */

    /* USER CODE END USART1_Init 0 */

    /* USER CODE BEGIN USART1_Init 1 */

    /* USER CODE END USART1_Init 1 */
    huart1.Instance = USART1;
    huart1.Init.BaudRate = 115200;
    huart1.Init.WordLength = UART_WORDLENGTH_8B;
    huart1.Init.StopBits = UART_STOPBITS_1;
    huart1.Init.Parity = UART_PARITY_NONE;
    huart1.Init.Mode = UART_MODE_TX_RX;
    huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
    huart1.Init.OverSampling = UART_OVERSAMPLING_16;
    if (HAL_UART_Init(&huart1) != HAL_OK) {
        Error_Handler();
    }
    /* USER CODE BEGIN USART1_Init 2 */
    lwrb_init(&usart1_lwrbHandle, usart1_lwrbBuffer, sizeof(usart1_lwrbBuffer));
//    __HAL_UART_ENABLE_IT(&huart1, UART_IT_IDLE);
    HAL_UART_Receive_IT(&huart1, usart1_Rxbuffer, sizeof(usart1_Rxbuffer));
    /* USER CODE END USART1_Init 2 */

}

void HAL_UART_Enable_Rx_IT(UART_HandleTypeDef *huart) {
    HAL_UART_Receive_IT(huart, usart1_Rxbuffer, sizeof(usart1_Rxbuffer));
}

void HAL_UART_MspInit(UART_HandleTypeDef *uartHandle) {

    GPIO_InitTypeDef GPIO_InitStruct = {0};
    if (uartHandle->Instance == USART1) {
        /* USER CODE BEGIN USART1_MspInit 0 */

        /* USER CODE END USART1_MspInit 0 */
        /* USART1 clock enable */
        __HAL_RCC_USART1_CLK_ENABLE();

        __HAL_RCC_GPIOA_CLK_ENABLE();
        /**USART1 GPIO Configuration
        PA9     ------> USART1_TX
        PA10     ------> USART1_RX
        */
        GPIO_InitStruct.Pin = GPIO_PIN_9 | GPIO_PIN_10;
        GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
        GPIO_InitStruct.Alternate = GPIO_AF7_USART1;
        HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

        /* USART1 interrupt Init */
        HAL_NVIC_SetPriority(USART1_IRQn, 5, 0);
        HAL_NVIC_EnableIRQ(USART1_IRQn);
        /* USER CODE BEGIN USART1_MspInit 1 */

        /* USER CODE END USART1_MspInit 1 */
    }
}

void HAL_UART_MspDeInit(UART_HandleTypeDef *uartHandle) {

    if (uartHandle->Instance == USART1) {
        /* USER CODE BEGIN USART1_MspDeInit 0 */

        /* USER CODE END USART1_MspDeInit 0 */
        /* Peripheral clock disable */
        __HAL_RCC_USART1_CLK_DISABLE();

        /**USART1 GPIO Configuration
        PA9     ------> USART1_TX
        PA10     ------> USART1_RX
        */
        HAL_GPIO_DeInit(GPIOA, GPIO_PIN_9 | GPIO_PIN_10);

        /* USART1 interrupt Deinit */
        HAL_NVIC_DisableIRQ(USART1_IRQn);
        /* USER CODE BEGIN USART1_MspDeInit 1 */

        /* USER CODE END USART1_MspDeInit 1 */
    }
}

/* USER CODE BEGIN 1 */

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
    /* Prevent unused argument(s) compilation warning */
    if (huart->Instance == USART1) {
        lwrb_write(&usart1_lwrbHandle, usart1_Rxbuffer, huart->RxXferSize);
        HAL_UART_Receive_IT(huart, usart1_Rxbuffer, sizeof(usart1_Rxbuffer));
    }
}

void HAL_UART_RXIdleCallback(UART_HandleTypeDef *huart){
    if (huart->Instance == USART1) {
        uint32_t len = huart->RxXferSize - huart->RxXferCount;
        lwrb_write(&usart1_lwrbHandle, usart1_Rxbuffer, len);
        HAL_UART_Receive_IT(huart, usart1_Rxbuffer, sizeof(usart1_Rxbuffer));
    }
}

void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart) {
    if (__HAL_UART_GET_FLAG(huart, UART_FLAG_ORE)) {
        __HAL_UART_CLEAR_OREFLAG(huart);
    }
}

signed short MiniUart_readData(int uartCh, char *data, unsigned short dataSize) {
    lwrb_t *lwrb = NULL;

    if (uartCh == 1) {
        lwrb = &usart1_lwrbHandle;
    } else {
        lwrb = &usart1_lwrbHandle;
    }

    return lwrb_read(lwrb, data, dataSize);
}

signed short MiniUart_writeData(int uartCh, char *data, unsigned short dataSize) {
    UART_HandleTypeDef *handle = NULL;

    if (uartCh == 1) {
        handle = &huart1;
    } else {
        handle = &huart1;
    }

    HAL_StatusTypeDef status = HAL_UART_Transmit(handle, (const uint8_t *) data, dataSize, 100);
    if (status != HAL_OK) {
        return -1;
    }

    return dataSize;
}
/* USER CODE END 1 */
