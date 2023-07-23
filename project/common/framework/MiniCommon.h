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
#include "MiniFoc/MiniFoc.h"
#include "debug/cm_backtrace/cm_backtrace.h"
#include "gui.h"
#include "MiniButton.h"
#include "../../../src/console/MiniShell.h"
#include "sfud/sfud.h"
#include "fs/FatFs/App/fatfs.h"
#include "usbd_cdc_if.h"
#include "debug/heap_trace.h"
#include "platform_init.h"
#include "MiniMotor.h"
#include "UnityManager.h"
#include "fs/vfs/vfs.h"

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
