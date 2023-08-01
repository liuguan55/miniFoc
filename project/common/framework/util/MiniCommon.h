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
// Created by 邓志君 on 2023/1/14.
//

#ifndef MINI_FOC_CORE_SRC_MINICOMMON_H_
#define MINI_FOC_CORE_SRC_MINICOMMON_H_
#include "kernel/FreeRTOS/FreeRTOS.h"
#include "kernel/FreeRTOS/task.h"
#include "kernel/FreeRTOS/CMSIS_RTOS_V2/cmsis_os.h"
#include <stdio.h>
#include <string.h>
#include "log/easylogger/elog.h"
#include "common/framework/easyflash/easyflash.h"
#include "log/easylogger/elog_flash.h"
#include "common/framework/MiniFoc/MiniFoc.h"
#include "debug/cm_backtrace/cm_backtrace.h"
#include "common/framework/gui/gui.h"
#include "common/framework/multiButton/MiniButton.h"
#include "../../../../src/console/MiniShell.h"
#include "debug/heap_trace.h"
#include "common/framework/platform/platform_init.h"
#include "common/framework/MiniFoc/MiniMotor.h"
#include "common/framework/UnityManager/UnityManager.h"


#define FIRMWARE_NAME                   "MiNIFoc_f4"
#define HARDWARE_VERSION               "V1.0.2"
#define SOFTWARE_VERSION               SDK_VERSION_STR

enum {
  LED_OFF = 0,
  LED_ON,
};


#define  container_of(ptr, type, member) ({    \
     const typeof( ((type *)0)->member ) *__mptr = (ptr); \
     (type *)( (char *)__mptr - offsetof(type,member) );})


static  inline void MiniCommon_delayMs(uint32_t ms) {
    osDelay(ms);
}

static  inline uint32_t MiniCommon_millis(void) {
    return osKernelGetTickCount();
}
static  inline uint32_t MiniCommon_elapsed(uint32_t ms){
    uint32_t now = MiniFoc_millis();
    uint32_t diff = 0;
    if (ms <= now){
        diff = now - ms ;
    }

    return diff ;
}

void MiniCommon_led1On(int on);
void MiniCommon_led1Toggle(void);
void *MiniCommon_malloc(size_t size);
void *MiniCommon_calloc(size_t memb, size_t size);
void MiniCommon_free(void *ptr);
#endif //MINI_FOC_CORE_SRC_MINICOMMON_H_
