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


#include <stdio.h>
#include <sys/unistd.h> /* for STDOUT_FILENO and STDERR_FILENO */
#include "compiler.h"
#include "common/board/board.h"
#include "driver/hal/hal_os.h"
#include "driver/hal/hal_base.h"
#include "driver/hal/hal_uart.h"
#include "driver/hal/hal_usb.h"
#include "libc/stdio.h"
#include "cmsis_gcc.h"
#include "common/ProjectConf.h"

/*
 * retarget for standard output/error
 */

#define STDOUT_WAIT_UART_TX_DONE 1

static uint8_t g_stdout_enable = 1;
static HAL_UART_ID g_stdout_uart_id = HAL_UART_NR;

#if PRJCONF_UART_EN

void stdout_enable(uint8_t en)
{
	g_stdout_enable = en;
}

static int stdout_write(const char *buf, int len)
{
	if (!g_stdout_enable || g_stdout_uart_id >= HAL_UART_NR || len <= 0) {
		return 0;
	}

    HAL_usbCdcSend((uint8_t *)buf, len);

	return board_uart_write(g_stdout_uart_id, buf, len);
}

int __io_putchar(int ch) {
    stdout_write((const char *)&ch, 1);

    return 0;
}

int stdout_init(void)
{
	if (g_stdout_uart_id < HAL_UART_NR) {
		return 0;
	}

	if (board_uart_init(BOARD_MAIN_UART_ID) == HAL_STATUS_OK) {
		g_stdout_uart_id = BOARD_MAIN_UART_ID;

		stdio_set_write(stdout_write);

		return 0;
	}

	return -1;
}

int stdout_deinit(void)
{
	if (g_stdout_uart_id >= HAL_UART_NR) {
		return 0;
	}

	if (board_uart_deinit(g_stdout_uart_id) == HAL_STATUS_OK) {
		g_stdout_uart_id = HAL_UART_NR;
		stdio_set_write(NULL);
		return 0;
	}

	return -1;
}

#endif /* PRJCONF_UART_EN */