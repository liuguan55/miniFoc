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
// Created by 86189 on 2023/5/20.
//
#include "driver/hal/hal_uart.h"
#include "driver/hal/hal_base.h"
#include "driver/hal/hal_os.h"
#include "driver/driver_chip.h"
#include "core/lwrb/lwrb.h"

#define USART_RXBUFFER_SIZE 1
#define RINGBUFFER_BUFFER_SIZE  64

typedef struct {
    UART_HandleTypeDef huart;
    uint8_t rxbuffer[USART_RXBUFFER_SIZE];
    lwrb_t ringHandle;
    uint8_t ringBuffer[RINGBUFFER_BUFFER_SIZE];
    HAL_Semaphore rxSem;
    void (*rx_callback)(uint8_t *buf, uint16_t len);
    void (*tx_callback)(uint8_t *buf, uint16_t len);
} HalUartPrivate_t;

static HalUartPrivate_t *gHalUartPrivates[HAL_UART_NR] ;

static uint8_t gUartIrqHandles[HAL_UART_NR] = {
        [HAL_UART_1] = USART1_IRQn,
        [HAL_UART_2] = USART2_IRQn,
#ifdef TARGET_MCU_STM32F4
        [HAL_UART_3] = USART3_IRQn,
        [HAL_UART_4] = UART4_IRQn,
        [HAL_UART_5] = UART5_IRQn,
        [HAL_UART_6] = USART6_IRQn,
#endif
};

static uint32_t gBoudrateTable[HAL_UART_BAUDRATE_NR] = {
        [HAL_UART_BAUDRATE_9600] = 9600,
        [HAL_UART_BAUDRATE_19200] = 19200,
        [HAL_UART_BAUDRATE_38400] = 38400,
        [HAL_UART_BAUDRATE_57600] = 57600,
        [HAL_UART_BAUDRATE_115200] = 115200,
        [HAL_UART_BAUDRATE_230400] = 230400,
        [HAL_UART_BAUDRATE_460800] = 460800,
        [HAL_UART_BAUDRATE_921600] = 921600,
        [HAL_UART_BAUDRATE_3000000] = 3000000,
};

static uint32_t gDatabitsTable[HAL_UART_DATABITS_NR] = {
        [HAL_UART_DATABITS_8] = UART_WORDLENGTH_8B,
        [HAL_UART_DATABITS_9] = UART_WORDLENGTH_9B,
};

static uint32_t gStopbitsTable[HAL_UART_STOPBITS_NR] = {
        [HAL_UART_STOPBITS_1] = UART_STOPBITS_1,
        [HAL_UART_STOPBITS_2] = UART_STOPBITS_2,
};

static uint32_t gParityTable[HAL_UART_PARITY_NR] = {
        [HAL_UART_PARITY_NONE] = UART_PARITY_NONE,
        [HAL_UART_PARITY_EVEN] = UART_PARITY_EVEN,
        [HAL_UART_PARITY_ODD] = UART_PARITY_ODD,
};

static uint32_t gFlowcontrolTable[HAL_UART_FLOWCONTROL_NR] = {
        [HAL_UART_FLOWCONTROL_NONE] = UART_HWCONTROL_NONE,
        [HAL_UART_FLOWCONTROL_RTS_CTS] = UART_HWCONTROL_RTS,
        [HAL_UART_FLOWCONTROL_XON_XOFF] = UART_HWCONTROL_CTS,
};

static HalUartPrivate_t *HAL_UartGetPrivate(HAL_UART_ID uartId) {
    if (uartId >= HAL_UART_NR) {
        return NULL;
    }
    return gHalUartPrivates[uartId];
}

static void HAL_UartSetPrivate(HAL_UART_ID uartId, HalUartPrivate_t *p) {
    if (uartId >= HAL_UART_NR) {
        return;
    }
    gHalUartPrivates[uartId] = p;
}

/**
 * @brief Get uart instance for uart id
 * @param id ID of specified uart
 * @return Pointer to uart instance
 */
static USART_TypeDef *UartInstanceGet(HAL_UART_ID id) {
    switch (id) {
        case HAL_UART_1:
            return USART1;
        case HAL_UART_2:
            return USART2;
#ifdef TARGET_MCU_STM32F4
        case HAL_UART_3:
            return USART3;

        case HAL_UART_4:
            return UART4;
        case HAL_UART_5:
            return UART5;
        case HAL_UART_6:
            return USART6;
#endif
        default:
            return NULL;
    }
}

static void UARTClockEnable(HAL_UART_ID id)
{
    switch (id) {
        case HAL_UART_1:
            __HAL_RCC_USART1_CLK_ENABLE();
            break;
        case HAL_UART_2:
            __HAL_RCC_USART2_CLK_ENABLE();
            break;
#ifdef TARGET_MCU_STM32F4
        case HAL_UART_3:
            __HAL_RCC_USART3_CLK_ENABLE();
            break;

        case HAL_UART_4:
            __HAL_RCC_UART4_CLK_ENABLE();
            break;
        case HAL_UART_5:
            __HAL_RCC_UART5_CLK_ENABLE();
            break;
        case HAL_UART_6:
            __HAL_RCC_USART6_CLK_ENABLE();
            break;
#endif
        default:
            break;
    }
}

static void UARTClockDisable(HAL_UART_ID id)
{
    switch (id) {
        case HAL_UART_1:
            __HAL_RCC_USART1_CLK_DISABLE();
            break;
        case HAL_UART_2:
            __HAL_RCC_USART2_CLK_DISABLE();
            break;
#ifdef TARGET_MCU_STM32F4
        case HAL_UART_3:
            __HAL_RCC_USART3_CLK_DISABLE();
            break;

        case HAL_UART_4:
            __HAL_RCC_UART4_CLK_DISABLE();
            break;
        case HAL_UART_5:
            __HAL_RCC_UART5_CLK_DISABLE();
            break;
        case HAL_UART_6:
            __HAL_RCC_USART6_CLK_DISABLE();
            break;
#endif
        default:
            break;
    }
}

/**
 * @brief UART1 IRQ Handler
 */
void USART1_IRQHandler(void) {
    HalUartPrivate_t *p = HAL_UartGetPrivate(HAL_UART_1);
    if (p){
        HAL_UART_IRQHandler(&p->huart);
    }
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
    /* Prevent unused argument(s) compilation warning */
    if (huart->Instance == USART1) {
        HalUartPrivate_t *p = HAL_UartGetPrivate(HAL_UART_1);
        lwrb_write(&p->ringHandle, p->rxbuffer, sizeof(p->rxbuffer));
        HAL_SemaphoreRelease(&p->rxSem);
        HAL_UART_Receive_IT(huart, p->rxbuffer, sizeof(p->rxbuffer));

        if (p->rx_callback){
            p->rx_callback(p->rxbuffer, sizeof(p->rxbuffer));
        }
    }
}

void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart) {
    if (__HAL_UART_GET_FLAG(huart, UART_FLAG_ORE)) {
        __HAL_UART_CLEAR_OREFLAG(huart);

        if (huart->Instance == USART1){
            HalUartPrivate_t *p = HAL_UartGetPrivate(HAL_UART_1);
            HAL_UART_Receive_IT(huart, p->rxbuffer, sizeof(p->rxbuffer));
        }
    }
}

static HAL_Status Hal_UartHwInit(HAL_UART_ID uartId, HAL_UartCfg_t *pCfg) {
    if (uartId >= HAL_UART_NR) {
        return HAL_STATUS_ERROR;
    }

    if (pCfg == NULL) {
        return HAL_STATUS_ERROR;
    }

    UARTClockEnable(uartId);
    USART_TypeDef *uart = UartInstanceGet(uartId);

    HAL_BoardIoctl(HAL_BIR_PINMUX_INIT, HAL_MKDEV(HAL_DEV_MAJOR_UART,uartId), 0);

    HalUartPrivate_t *p = HAL_UartGetPrivate(uartId);
    p->huart.Instance = uart;
    p->huart.Init.BaudRate = gBoudrateTable[pCfg->baudrate];
    p->huart.Init.WordLength = gDatabitsTable[pCfg->databits];
    p->huart.Init.StopBits = gStopbitsTable[pCfg->stopbits];
    p->huart.Init.Parity = gParityTable[pCfg->parity];
    p->huart.Init.Mode = UART_MODE_TX_RX;
    p->huart.Init.HwFlowCtl = gFlowcontrolTable[pCfg->flowControl];
    p->huart.Init.OverSampling = UART_OVERSAMPLING_16;
    if (HAL_UART_Init(&p->huart) != HAL_OK) {
        return HAL_STATUS_ERROR;
    }

    HAL_NVIC_SetPriority(gUartIrqHandles[uartId], 7, 0);
    HAL_NVIC_EnableIRQ(gUartIrqHandles[uartId]);

    HAL_UART_Receive_IT(&p->huart, p->rxbuffer, sizeof(p->rxbuffer));

    return HAL_STATUS_OK;
}

static void HAL_UartHwDeinit(HAL_UART_ID uartId) {
    if (uartId >= HAL_UART_NR) {
        return;
    }

    HalUartPrivate_t *p = HAL_UartGetPrivate(uartId);
    if (p == NULL) {
        return;
    }

    UARTClockDisable(uartId);
    HAL_BoardIoctl(HAL_BIR_PINMUX_DEINIT, HAL_MKDEV(HAL_DEV_MAJOR_UART,uartId), 0);

    HAL_UART_DeInit(&p->huart);
    HAL_NVIC_DisableIRQ(gUartIrqHandles[uartId]);
    lwrb_free(&p->ringHandle);
    HAL_SemaphoreDeinit(&p->rxSem);
}

HAL_Status HAL_UartInit(HAL_UART_ID uartId, HAL_UartCfg_t *pCfg) {
    HAL_Status res = HAL_STATUS_OK;

    if (uartId >= HAL_UART_NR) {
        return HAL_STATUS_ERROR;
    }

    if (pCfg == NULL) {
        return HAL_STATUS_ERROR;
    }

    HalUartPrivate_t *p = HAL_UartGetPrivate(uartId);
    if (p == NULL) {
        p = (HalUartPrivate_t *) HAL_Malloc(sizeof(HalUartPrivate_t));
        if (p == NULL) {
            return HAL_STATUS_ERROR;
        }
        HAL_UartSetPrivate(uartId, p);
    }
    memset(p, 0, sizeof(HalUartPrivate_t));

    HAL_SemaphoreInit(&p->rxSem, 1, 1);
    HAL_SemaphoreWait(&p->rxSem, 100);
    lwrb_init(&p->ringHandle, p->ringBuffer, sizeof(p->ringBuffer));
    res = Hal_UartHwInit(uartId, pCfg);

    return res;
}

HAL_Status HAL_UartDeInit(HAL_UART_ID uartId) {
    if (uartId >= HAL_UART_NR) {
        return HAL_STATUS_ERROR;
    }

    HalUartPrivate_t *p = HAL_UartGetPrivate(uartId);
    if (p == NULL) {
        return HAL_STATUS_ERROR;
    }

    HAL_UartHwDeinit(uartId);

    HAL_Free(p);
    HAL_UartSetPrivate(uartId, NULL);

    return HAL_STATUS_OK;
}

HAL_Status HAL_UartSend(HAL_UART_ID uartId, uint8_t *buf, uint16_t len) {
    HalUartPrivate_t *p = HAL_UartGetPrivate(uartId);
    if (p == NULL) {
        return HAL_STATUS_ERROR;
    }

    if (HAL_UART_Transmit(&p->huart, buf, len, 0xFFFF)){
        return HAL_STATUS_ERROR;
    }

    return HAL_STATUS_OK;
}

HAL_Status HAL_UartRecv(HAL_UART_ID uartId, uint8_t *buf, uint16_t len, uint32_t *recvLen) {
    HalUartPrivate_t *p = HAL_UartGetPrivate(uartId);
    if (p == NULL || recvLen == NULL) {
        return HAL_STATUS_ERROR;
    }

    HAL_SemaphoreWait(&p->rxSem, 100);

    *recvLen = lwrb_read(&p->ringHandle, buf, len);
    if (*recvLen == 0) {
        HAL_UART_Receive_IT(&p->huart, p->rxbuffer, sizeof(p->rxbuffer));
    }

    return HAL_STATUS_OK;
}

HAL_Status HAL_UartSetCallback(HAL_UART_ID uartId, void (*rx_callback)(uint8_t *buf, uint16_t len),
                               void (*tx_callback)(uint8_t *buf, uint16_t len)) {
    HalUartPrivate_t *p = HAL_UartGetPrivate(uartId);
    if (p == NULL) {
        return HAL_STATUS_ERROR;
    }

    p->rx_callback = rx_callback;
    p->tx_callback = tx_callback;

    return HAL_STATUS_OK;
}

HAL_Status HAL_UartSetBaudrate(HAL_UART_ID uartId, uint32_t baudrate) {
    HalUartPrivate_t *p = HAL_UartGetPrivate(uartId);
    if (p == NULL) {
        return HAL_STATUS_ERROR;
    }

    p->huart.Init.BaudRate = baudrate;
    if (HAL_UART_Init(&p->huart) != HAL_OK) {
        return HAL_STATUS_ERROR;
    }
    HAL_UART_Receive_IT(&p->huart, p->rxbuffer, sizeof(p->rxbuffer));

    return HAL_STATUS_OK;
}



