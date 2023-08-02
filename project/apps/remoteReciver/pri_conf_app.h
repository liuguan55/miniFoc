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
// Created by 86189 on 2023/5/25.
//

#ifndef MINIFOC_F1_PRI_CONF_APP_H
#define MINIFOC_F1_PRI_CONF_APP_H



#ifdef __cplusplus
extern "C" {
#endif


/* main thread priority */
#define PRJCONF_MAIN_THREAD_PRIO        HAL_OS_PRIORITY_NORMAL

/* main thread stack size */
#define PRJCONF_MAIN_THREAD_STACK_SIZE  (4 * 128)

/* uart enable/disable */
#define PRJCONF_UART_EN                 1


/* console enable/disable */
#define PRJCONF_CONSOLE_EN              0
/* console stack size */
#define PRJCONF_CONSOLE_STACK_SIZE      (4 * 128)
/* console buffer size */
#define PROJCONF_CONSOLEBUF_SIZE        (128)

#ifdef USE_VFS
#define PRJCONF_VFS_FILE_SYSTEM_EN      1
#else
#define PRJCONF_VFS_FILE_SYSTEM_EN      0
#endif

#define PRJCONF_WATCHDOG_EN             0
#define PRjCONF_WATCHDOG_TIMEOUT        10

#ifdef USE_SPI_FLASH
#define PRJCONF_SPIFLASH_EN             1
#else
#define PRJCONF_SPIFLASH_EN             0
#endif

#ifdef  USE_LOG
#define PRJCONF_LOG_EN                  1
#else
#define PRJCONF_LOG_EN                  0
#endif

#define PRJCONF_BTN_EN                  1

#ifdef USE_FOC
#define PRJCONF_MOTOR_EN                  1
#else
#define PRJCONF_MOTOR_EN                0
#endif

#ifdef USE_USB
#define PRJCONF_USB_EN                  1
#else
#define PRJCONF_USB_EN                  0
#endif

#ifdef USE_SDCARD
#define PRJCONF_SDCARD_EN               1
#else
#define PRJCONF_SDCARD_EN               0
#endif

#define PRJCONF_UNITY_EN                0
#ifdef __cplusplus

}
#endif

#endif //MINIFOC_F4_PRI_CONF_APP_H