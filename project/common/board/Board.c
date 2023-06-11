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
// Created by 86189 on 2023/5/24.
//
#include <string.h>
#include "Board.h"
#include "common/board/BoardDebug.h"
#include "common/board/Board.h"
#include "driver/hal/hal_spi.h"
#include "driver/hal/hal_os.h"
#include "driver/hal/hal_def.h"
#include "driver/hal/hal_dev.h"


/* uart */
#if PRJCONF_UART_EN
HAL_Status board_uart_init(HAL_UART_ID uart_id)
{
  static const HAL_UartCfg_t board_uart_param = {
        .baudrate          = BOARD_UART_BAUD_RATE,
        .parity            = BOARD_UART_PARITY,
        .stopbits          = BOARD_UART_STOP_BITS,
        .databits          = BOARD_UART_DATA_BITS,
        .flowControl       = BOARD_UART_HW_FLOW_CTRL,
    };

    return HAL_UartInit(uart_id, (HAL_UartCfg_t *)&board_uart_param);
}

#if PRJCONF_SPIFLASH_EN
HAL_Status board_spiFlash_init(HAL_SPI_ID spiId) {
//    HAL_BoardIoctl(HAL_BIR_PINMUX_INIT, HAL_MKDEV(HAL_DEV_MAJOR_FLASHC, 0), 0);
    SpiConfig_t spiConfig = {
            .mode = BOARD_FLASH_SPI_MODE,
            .speed = BOARD_FLASH_SPI_SPEED,
            .bit_order = BOARD_FLASH_SPI_ORDER,
            .word_size = BOARD_FLASH_SPI_DATA_WIDTH,
    };
    HAL_spiInit(spiId, &spiConfig);

    return HAL_STATUS_OK;
}
#endif

#endif /* PRJCONF_UART_EN */