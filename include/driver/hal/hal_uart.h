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

#ifndef MINIFOC_F4_HAL_UART_H
#define MINIFOC_F4_HAL_UART_H
#include "stdint.h"
#include "driver/hal/hal_def.h"
#ifdef __cplusplus
extern "C" {
#endif

enum{
    HAL_UART_1 = 0,
    HAL_UART_2,
    HAL_UART_3,
#ifdef TARGET_MCU_STM32F4
    HAL_UART_4,
    HAL_UART_5,
    HAL_UART_6,
#endif
    HAL_UART_NR,
    HAL_UART_INVALID_ID = 0Xff,
};

typedef uint8_t HAL_UART_ID;

enum {
    HAL_UART_BAUDRATE_9600 = 0,
    HAL_UART_BAUDRATE_19200,
    HAL_UART_BAUDRATE_38400,
    HAL_UART_BAUDRATE_57600,
    HAL_UART_BAUDRATE_115200,
    HAL_UART_BAUDRATE_230400,
    HAL_UART_BAUDRATE_460800,
    HAL_UART_BAUDRATE_921600,
    HAL_UART_BAUDRATE_3000000,
    HAL_UART_BAUDRATE_NR,
};

typedef  uint8_t HAL_UART_BAUDRATE;

enum {
    HAL_UART_DATABITS_8 = 0,
    HAL_UART_DATABITS_9,
    HAL_UART_DATABITS_NR,
};

typedef  uint8_t HAL_UART_DATABITS;

enum {
    HAL_UART_STOPBITS_1 = 0,
    HAL_UART_STOPBITS_2,
    HAL_UART_STOPBITS_NR,
};

typedef uint8_t HAL_UART_STOPBITS;

enum {
    HAL_UART_PARITY_NONE = 0,
    HAL_UART_PARITY_EVEN,
    HAL_UART_PARITY_ODD,
    HAL_UART_PARITY_NR,
};

typedef uint8_t HAL_UART_PARITY;

enum {
    HAL_UART_FLOWCONTROL_NONE = 0,
    HAL_UART_FLOWCONTROL_RTS_CTS,
    HAL_UART_FLOWCONTROL_XON_XOFF,
    HAL_UART_FLOWCONTROL_NR,
};

typedef uint8_t HAL_UART_FLOWCONTROL;

typedef struct {
    HAL_UART_BAUDRATE baudrate;
    HAL_UART_DATABITS databits;
    HAL_UART_STOPBITS stopbits;
    HAL_UART_PARITY parity;
    HAL_UART_FLOWCONTROL flowControl;
}HAL_UartCfg_t;

/**
 * @brief uart init
 * @param uartId
 * @param pCfg
 * @return
 */
HAL_Status HAL_UartInit(HAL_UART_ID uartId, HAL_UartCfg_t *pCfg);

/**
 * @brief uart deinit
 * @param uartId
 * @return
 */
HAL_Status HAL_UartDeInit(HAL_UART_ID uartId);

/**
 * @brief uart send
 * @param uartId
 * @param buf
 * @param len
 * @return
 */
HAL_Status HAL_UartSend(HAL_UART_ID uartId, uint8_t *buf, uint16_t len);

/**
 * @brief uart recv
 * @param uartId
 * @param buf
 * @param len
 * @param recvLen
 * @return
 */
HAL_Status HAL_UartRecv(HAL_UART_ID uartId, uint8_t *buf, uint16_t len, uint32_t *recvLen);

/**
 * @brief uart set callback
 * @param uartId
 * @param rx_callback
 * @param tx_callback
 * @return
 */
HAL_Status HAL_UartSetCallback(HAL_UART_ID uartId, void (*rx_callback)(uint8_t *buf, uint16_t len),
                               void (*tx_callback)(uint8_t *buf, uint16_t len));

/**
 * @brief uart set baudrate
 * @param uartId
 * @param baudrate
 * @return
 */
HAL_Status HAL_UartSetBaudrate(HAL_UART_ID uartId, uint32_t baudrate);
#ifdef __cplusplus
}
#endif
#endif //MINIFOC_F4_HAL_UART_H
