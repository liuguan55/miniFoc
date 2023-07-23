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
// Created by 86189 on 2023/5/21.
//

#include "console/miniConsole.h"
#include <stdio.h>
#include <inttypes.h>
#include "console/lettershell/shell.h"
#include "kernel/FreeRTOS/FreeRTOS.h"
#include "kernel/FreeRTOS/task.h"
#include "kernel/FreeRTOS/CMSIS_RTOS_V2/cmsis_os.h"
#include "common/framework/MiniCommon.h"
#include "fs/vfs/vfs.h"
#include "driver/hal/hal_uart.h"
#include "driver/hal/hal_usb.h"
#include "driver/hal/hal_os.h"
#include "MiniShell.h"
#include "sys/MiniDebug.h"

ConsoleDev_t *gConsoleDev = NULL;

/**
 * @brief Read data from console
 * @param data recv data
 * @param dataSize recv data size
 * @return real recv data size
 */
signed short ShellRead(char *data, unsigned short dataSize) {
//    return MiniUart_readData(MINI_SHELL_USART_IDX, data, dataSize);
    uint32_t recvSize = 0;

    HAL_usbCdcRecv((uint8_t *)data, dataSize, &recvSize);
    if (recvSize > 0) {
        return (signed short)recvSize;
    }

    HAL_UartRecv(gConsoleDev->uartId, (uint8_t *)data, dataSize, &recvSize);

    return (signed short)recvSize;
}

/**
 * @brief Write data to console
 * @param data Write data
 * @param dataSize write data size
 * @return HAL_STATUS_OK if success , else if failed
 */
signed short ShellWrite(char *data, unsigned short dataSize) {
    HAL_usbCdcSend((uint8_t *)data, dataSize);
    return HAL_UartSend(gConsoleDev->uartId, (uint8_t *)data, dataSize);
}

/**
 * @brief shell lock
 * @param shell shell handle
 * @return
 */
int ShellLock(Shell *shell) {
    MINI_ASSERT_MSG(shell != NULL, "shell is null");
    HAL_MutexLock(&gConsoleDev->shellMutex, HAL_OS_WAIT_FOREVER);

    return 0;
}

/**
 * @brief shell unlock
 * @param shell
 * @return
 */
int ShellUnlock(Shell *shell) {
    MINI_ASSERT_MSG(shell != NULL, "shell is null");
    HAL_MutexUnlock(&gConsoleDev->shellMutex);

    return 0;
}

/**
 * @brief shell log write
 * @param buffer Pointer to the buffer containing the data to be written.
 * @param len Number of bytes to write.
 */
void terminalLogWrite(char *buffer, short len) {
    MINI_ASSERT_MSG(buffer != NULL, "buffer is null");
    MINI_ASSERT_MSG(gConsoleDev != NULL, "gConsoleDev is null");

    if (gConsoleDev->shellLog.shell) {
        shellWriteEndLine(gConsoleDev->shellLog.shell, buffer, len);
    }
}


void miniConsoleInit(MiniConsoleConfig_t *config) {
    if (gConsoleDev != NULL) {
        printf("console already initialized\n");
        return;
    }

    gConsoleDev = (ConsoleDev_t *)HAL_Malloc(sizeof(ConsoleDev_t));
    if (gConsoleDev == NULL) {
        printf("console malloc failed\n");
        return;
    }

    memset(gConsoleDev, 0, sizeof(ConsoleDev_t));

    gConsoleDev->uartId = config->uartId;
    gConsoleDev->state = CONSOLE_STATE_START;

    HAL_MutexInit(&gConsoleDev->shellMutex);


    gConsoleDev->shell.read = ShellRead;
    gConsoleDev->shell.write = ShellWrite;
    gConsoleDev->shell.lock = ShellLock;
    gConsoleDev->shell.unlock = ShellUnlock;
    gConsoleDev->shellLog.write = terminalLogWrite;

    MiniShell_Init(gConsoleDev);

    HAL_ThreadCreate(shellTask , "shellTask", config->stackSize, &gConsoleDev->shell, HAL_OS_PRIORITY_LOW, &gConsoleDev->shellThread);
}

void miniConsoleDeinit(void){
    if (gConsoleDev == NULL) {
        printf("console not initialized\n");
        return;
    }

    HAL_ThreadDelete(gConsoleDev->shellThread);
    HAL_MutexDeinit(&gConsoleDev->shellMutex);

    HAL_Free(gConsoleDev);
    gConsoleDev = NULL;
}

void miniConsoleWrite(uint8_t *data, uint32_t dataSize) {
    if (gConsoleDev == NULL) {
        printf("console not initialized\n");
        return;
    }

    HAL_UartSend(gConsoleDev->uartId, data, dataSize);
}
