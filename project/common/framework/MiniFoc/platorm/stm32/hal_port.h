//
// Created by 邓志君 on 2023/3/5.
//

#ifndef MINIFOC_F4_LIBS_SIMPLEFOC_PLATORM_STM32_HAL_PORT_H_
#define MINIFOC_F4_LIBS_SIMPLEFOC_PLATORM_STM32_HAL_PORT_H_
#include <stdint.h>
#include "stm32f4xx_hal.h"
#include "driver/adc.h"
#include "kernel/FreeRTOS/FreeRTOS.h"
#include "kernel/FreeRTOS/task.h"
#include "driver/main.h"
#include "kernel/FreeRTOS/CMSIS_RTOS_V2/cmsis_os.h"

/**
 * brief: delay ms
 * @param ms
 */
static inline void MiniFoc_delayMs(uint32_t ms)
{
  osDelay(ms);
}

/**
 * brief: Get current time of ms
 * @return
 */
static inline uint32_t MiniFoc_millis(void)
{
  return osKernelGetTickCount();
}

/**
 * brief:Get elapsed time according to now
 * @param ms
 * @return
 */
static  inline uint32_t MiniFoc_elapsed(uint32_t ms){
    uint32_t now = MiniFoc_millis();
    uint32_t diff = 0;
    if (ms <= now){
        diff = now - ms ;
    }

    return diff ;
}

float MiniFoc_getVbus(void);

static inline uint32_t MiniFoc_micros(void)
{
	return SysTick->VAL;
}
#endif //MINIFOC_F4_LIBS_SIMPLEFOC_PLATORM_STM32_HAL_PORT_H_
