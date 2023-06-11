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

#pragma once
#include "console/lettershell/shell.h"
#include "console/lettershell/shell_fs.h"
#include "console/lettershell/log.h"
#include "console/lettershell/shell.h"
#include "kernel/FreeRTOS/FreeRTOS.h"
#include "kernel/FreeRTOS/task.h"
#include "driver/main.h"
#include "kernel/FreeRTOS/CMSIS_RTOS_V2/cmsis_os.h"
#include "driver/usart.h"
#include "common/framework/MiniCommon.h"
#include "fs/vfs/vfs.h"
#include "driver/hal/hal_uart.h"
#include "driver/hal/hal_os.h"

#define  CONSOLE_BUFFER_SIZE            (512)
typedef enum {
    CONSOLE_STATE_STOP = 0,
    CONSOLE_STATE_START,
    CONSOLE_STATE_TERMINATE,    /* to be terminated */
} console_state;

typedef struct console_priv {
    HAL_UART_ID     uartId;
    console_state   state;

    char buffer[CONSOLE_BUFFER_SIZE];
    char shellPathBuffer[CONSOLE_BUFFER_SIZE];
    Shell shell;
    ShellFs shellFs;
    Log shellLog;

    HAL_Mutex shellMutex;
    HAL_Thread shellThread;
} ConsoleDev_t;

void MiniShell_Init(ConsoleDev_t *console);