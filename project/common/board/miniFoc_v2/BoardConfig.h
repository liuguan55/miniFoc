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
// Created by 86189 on 2023/5/22.
//

#ifndef MINIFOC_F4_BOARD_CONFIG_H
#define MINIFOC_F4_BOARD_CONFIG_H


#include "driver/hal/hal_uart.h"
#include "driver/hal/hal_board.h"
#include "driver/hal/hal_dev.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
 * board configuration
 */
#define BOARD_NAME              "miniFoc_v2"

/* uart */
#define BOARD_MAIN_UART_ID      HAL_UART_1    /* debug and console */

#define BOARD_UART_BAUD_RATE    HAL_UART_BAUDRATE_115200
#define BOARD_UART_PARITY       HAL_UART_PARITY_NONE
#define BOARD_UART_STOP_BITS    HAL_UART_STOPBITS_1
#define BOARD_UART_DATA_BITS    HAL_UART_DATABITS_8
#define BOARD_UART_HW_FLOW_CTRL HAL_UART_FLOWCONTROL_NONE

/* spi */
#define BOARD_FLASH_SPI_ID           HAL_SPI_2
#define BOARD_FLASH_SPI_MODE         HAL_SPI_MODE_0
#define BOARD_FLASH_SPI_SS_SEL       HAL_SPI_TCTRL_SS_SEL_SS2
#define BOARD_FLASH_SPI_SPEED        (1*1000*1000)
#define BOARD_FLASH_SPI_DATA_WIDTH   HAL_SPI_8BIT
#define BOARD_FLASH_SPI_ORDER        HAL_SPI_MSB_FIRST

#ifdef __cplusplus
}
#endif

#endif //MINIFOC_F4_BOARD_CONFIG_H
