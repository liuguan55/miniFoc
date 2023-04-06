//
// Created by 邓志君 on 2023/3/5.
//

#ifndef MINIFOC_F4_LIBS_SIMPLEFOC_PLATORM_STM32_HAL_PORT_H_
#define MINIFOC_F4_LIBS_SIMPLEFOC_PLATORM_STM32_HAL_PORT_H_
#include <stdint.h>
#include "stm32f4xx_hal.h"
#include "adc.h"
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

static inline void MiniFoc_delayMs(uint32_t ms)
{
  osDelay(ms);
}

static inline uint32_t MiniFoc_millis(void)
{
  return osKernelGetTickCount();
}

float MiniFoc_getVbus(void);

static inline uint32_t MiniFoc_micros(void)
{
	return SysTick->VAL;
}
#endif //MINIFOC_F4_LIBS_SIMPLEFOC_PLATORM_STM32_HAL_PORT_H_
