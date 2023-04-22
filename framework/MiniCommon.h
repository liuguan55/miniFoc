//
// Created by 邓志君 on 2023/1/14.
//

#ifndef MINI_FOC_CORE_SRC_MINICOMMON_H_
#define MINI_FOC_CORE_SRC_MINICOMMON_H_
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"
#include <stdio.h>
#include <string.h>
#include "elog.h"
#include "easyflash.h"
#include "elog_flash.h"
#include "usart.h"
#include "tim.h"
#include "i2c.h"
#include "MiniFoc/MiniFoc.h"
#include "cm_backtrace.h"
#include "gui.h"
#include "MiniButton.h"
#include "MiniShell.h"
#include "sfud.h"
#include "fatfs.h"
#include "usbd_cdc_if.h"
#include "debug/heap_trace.h"
#include "platform_init.h"
#include "MiniMotor.h"

#define FIRMWARE_NAME                   "MiNIFoc_f4"
#define HARDWARE_VERSION               "V1.0.2"
#define SOFTWARE_VERSION               SDK_VERSION_STR

enum {
  LED_OFF = 0,
  LED_ON,
};

#define MODULE_INIT  __attribute__((constructor))

#define offsetof(TYPE, MEMBER) ((size_t) &((TYPE *)0)->MEMBER)
#define  container_of(ptr, type, member) ({    \
     const typeof( ((type *)0)->member ) *__mptr = (ptr); \
     (type *)( (char *)__mptr - offsetof(type,member) );})

#define MINI_CONSOLEBUF_SIZE    128

#define MINI_CONSOLE_USART_IDX (1)
#define MINI_SHELL_USART_IDX MINI_CONSOLE_USART_IDX
#define MINI_SHELL_BUFFER_SIZE (128 * 4)
#define MINI_SHELL_TASK_STACK_SIZE (512 * 4)

void MiniCommon_led(int on);
void MiniCommon_delayMs(uint32_t ms);
uint32_t MiniCommon_millis(void);
void MiniCommon_led1On(int on);
void MiniCommon_led1Toggle(void);
void MiniCommon_led2On(int on);
void MiniCommon_led2Toggle(void);
void MiniCommon_led3On(int on);
void MiniCommon_led3Toggle(void);
void *MiniCommon_malloc(size_t size);
void *MiniCommon_calloc(size_t memb, size_t size);
void MiniCommon_free(void *ptr);
#endif //MINI_FOC_CORE_SRC_MINICOMMON_H_
