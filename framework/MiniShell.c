/*
 * @Author: liuguan
 * @Date: 2022-06-14 15:48:49
 * @LastEditors: liuguan
 * @LastEditTime: 2022-07-16 23:00:21
 * @FilePath: /esp8266-rtos/src/espShell.c
 * @Description: 
 * 
 * Copyright (c) 2022 by liuguan, All Rights Reserved. 
 */
/*
 * @Author: liuguan
 * @Date: 2022-06-14 15:48:49
 * @LastEditors: liuguan
 * @LastEditTime: 2022-06-14 15:48:50
 * @FilePath: /esp8266-rtos/src/espShell.c
 * @Description: 
 * 
 * Copyright (c) 2022 by liuguan, All Rights Reserved. 
 */
/**
 * Copyright 2022 dengzhijun
 * 
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 * 
 *     http://www.apache.org/licenses/LICENSE-2.0
 * 
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include "lettershell/shell.h"
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"
#include <stdio.h>
#include "usart.h"
#include "MiniCommon.h"

#undef LOG_TAG
#define LOG_TAG "MINISHELL"

static char shellBuffer[MINI_SHELL_BUFFER_SIZE] = {0};
static Shell miniShell;
ShellFs shellFs;
char shellPathBuffer[512] = "/";
Log shellLog = {
    .active = 1,
    .level = LOG_DEBUG
};

static osMutexId_t shellMutex;
const osMutexAttr_t shellMutex_attributes = {
    .name = "shellMutex",
    .attr_bits = osMutexRecursive,
};

osThreadId_t shellTaskHandle;;
const osThreadAttr_t shellTask_attributes = {
    .name = "miniShell",
    .stack_size = MINI_SHELL_TASK_STACK_SIZE,
    .priority = (osPriority_t)osPriorityLow,
};

signed short ShellRead(char *data, unsigned short dataSize) {
  return MiniUart_readData(MINI_SHELL_USART_IDX, data, dataSize);
}

signed short ShellWrite(char *data, unsigned short dataSize) {
  usb_write_data(data, dataSize);

  return MiniUart_writeData(MINI_SHELL_USART_IDX, data, dataSize);
}

int ShellLock(Shell *shell) {
  osMutexAcquire(shellMutex, portMAX_DELAY);

  return 0;
}

int ShellUnlock(Shell *shell) {
  osMutexRelease(shellMutex);

  return 0;
}

/**
 * @brief 日志写函数实现
 *
 * @param buffer 数据
 * @param len 数据长度
 *
 */
void terminalLogWrite(char *buffer, short len) {
  if (shellLog.shell) {
    shellWriteEndLine(shellLog.shell, buffer, len);
  }
}

/**
 * @brief 列出文件
 *
 * @param path 路径
 * @param buffer 结果缓冲
 * @param maxLen 最大缓冲长度
 * @return size_t 0
 */
size_t userShellListDir(char *path, char *buffer, size_t maxLen) {
  DIR dir;
  FRESULT res;
  FILINFO fno;

  res = f_opendir(&dir, path);
  if (res != FR_OK) {
    log_e("opendir failed,err:%d", res);
    return -1;
  }

  memset(buffer, 0, maxLen);

  do {
    res = f_readdir(&dir, &fno);
    if (res != FR_OK || fno.fname[0] == 0) {
      break;
    }

    if (fno.fattrib & AM_DIR) {
      strcat(buffer, CSI(31));
      strcat(buffer, fno.fname);
    } else {
      strcat(buffer, CSI(32));
      strcat(buffer, fno.fname);
    }

    strcat(buffer, CSI(39));
    strcat(buffer, "\t");
  } while (1);

  f_closedir(&dir);

  return 0;
}
/**
 * @brief  init shell handle
 * 
 */
void MiniShell_Init(void) {
  miniShell.read = ShellRead;
  miniShell.write = ShellWrite;
#if SHELL_USING_LOCK
  shellMutex = osMutexNew(&shellMutex_attributes);
  miniShell.lock = ShellLock;
  miniShell.unlock = ShellUnlock;
#endif

  shellSetPath(&miniShell, shellPathBuffer);
  shellInit(&miniShell, shellBuffer, sizeof(shellBuffer));

  shellFs.getcwd = (size_t (*)(char *, size_t))f_getcwd;
  shellFs.chdir = (size_t (*)(char *))f_chdir;
  shellFs.listdir = userShellListDir;
  shellFs.mkdir = (size_t (*)(char *))f_mkdir;
  shellFs.unlink = (size_t (*)(char *))f_unlink;
  shellFsInit(&shellFs, shellPathBuffer, 512);
  shellCompanionAdd(&miniShell, SHELL_COMPANION_ID_FS, &shellFs);

  shellLog.write = terminalLogWrite;
  logRegister(&shellLog, &miniShell);

//  logDebug("hello world");
//  logHexDump(LOG_ALL_OBJ, LOG_DEBUG, (void *)&miniShell, sizeof(miniShell));
}

/**
 * @brief execute shell task
 * 
 */

void MiniShell_Run(void) {
  shellTaskHandle = osThreadNew(shellTask, &miniShell, &shellTask_attributes);
}