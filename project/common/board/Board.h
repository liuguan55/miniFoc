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

#ifndef MINIFOC_F4_BOARD_H
#define MINIFOC_F4_BOARD_H

#include "driver/hal/hal_base.h"
#include "driver/hal/hal_uart.h"
#include "common/ProjectConf.h"
#include "driver/hal/hal_spi.h"

#include "common/board/miniFoc_v2/BoardConfig.h"

#ifdef __cplusplus
extern "C" {
#endif

HAL_Status board_ioctl(HAL_BoardIoctlReq req, uint32_t param0, uint32_t param1);

#if PRJCONF_UART_EN
/* uart */
HAL_Status board_uart_init(HAL_UART_ID uart_id);

static __inline HAL_Status board_uart_deinit(HAL_UART_ID uart_id)
{
	return HAL_UartDeInit(uart_id);
}

static __inline int32_t board_uart_write(HAL_UART_ID uart_id, const char *buf, int len)
{
	return HAL_UartSend(uart_id, (uint8_t *)buf, len);
}
#endif

#if PRJCONF_SPIFLASH_EN
HAL_Status board_spiFlash_init(HAL_SPI_ID spiId);
#endif

#ifdef __cplusplus
}
#endif

#endif //MINIFOC_F4_BOARD_H
