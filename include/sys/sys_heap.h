#ifndef _SYS_HEAP_H_
#define _SYS_HEAP_H_

#include <stdio.h>
#include <stdint.h>
#include "kernel/FreeRTOS/FreeRTOS.h"
#include "kernel/FreeRTOS/task.h"
#include "sys/interrupt.h"


static __inline void malloc_mutex_lock(void)
{
  if (!arch_irq_get_flags()){
    vTaskSuspendAll();
  }
}

static __inline void malloc_mutex_unlock(void)
{
  if (!arch_irq_get_flags()){
    xTaskResumeAll();
  }
}

#define RANGEOF(num, start, end) (((num) <= (end)) && ((num) >= (start)))


#endif

