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

#ifndef MINIFOC_F4_HAL_OS_H
#define MINIFOC_F4_HAL_OS_H

#include <string.h>
#include <stdlib.h>
#include "sys/interrupt.h"
#include "driver/hal/hal_def.h"
#include "driver/driver_chip.h"

#ifdef USE_RTOS_SYSTEM
#include "kernel/FreeRTOS/FreeRTOS.h"
#include "kernel/FreeRTOS/CMSIS_RTOS_V2/cmsis_os2.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

/* IRQ disable/enable */
#define HAL_DisableIRQ()    arch_irq_disable()
#define HAL_EnableIRQ()     arch_irq_enable()

/* Check if IRQ is disabled */
#define HAL_IsIRQDisabled() __get_PRIMASK()

/* Check if in ISR context or not */
#define HAL_IsISRContext()  __get_IPSR()

/* Critical Sections */
#define HAL_EnterCriticalSection()      arch_irq_save()
#define HAL_ExitCriticalSection(flags)  arch_irq_restore(flags)

#ifdef USE_RTOS_SYSTEM
/* Semaphore */
typedef osSemaphoreId_t HAL_Semaphore;

static __inline HAL_Status HAL_SemaphoreInit(HAL_Semaphore *sem, uint32_t initCount, uint32_t maxCount)
{
    *sem = osSemaphoreNew(maxCount, initCount, NULL);
    return *sem ? HAL_STATUS_OK : HAL_STATUS_ERROR;
}

static __inline HAL_Status HAL_SemaphoreInitBinary(HAL_Semaphore *sem)
{
    return HAL_SemaphoreInit(sem, 1, 1);
}

static __inline HAL_Status HAL_SemaphoreDeinit(HAL_Semaphore *sem)
{
    return osSemaphoreDelete(*sem) == osOK ? HAL_STATUS_OK : HAL_STATUS_ERROR;
}

static __inline HAL_Status HAL_SemaphoreWait(HAL_Semaphore *sem, uint32_t msec)
{
    return osSemaphoreAcquire(*sem, msec) == osOK ? HAL_STATUS_OK : HAL_STATUS_ERROR;
}

static __inline HAL_Status HAL_SemaphoreTryWait(HAL_Semaphore *sem)
{
    return HAL_SemaphoreWait(sem, 0);
}

static __inline HAL_Status HAL_SemaphoreRelease(HAL_Semaphore *sem)
{
    return osSemaphoreRelease(*sem) == osOK ? HAL_STATUS_OK : HAL_STATUS_ERROR;
}

/* Mutex */
typedef osMutexId_t HAL_Mutex;

#define HAL_OS_WAIT_FOREVER portMAX_DELAY//0xFFFFFFFFU

static __inline HAL_Status HAL_MutexInit(HAL_Mutex *mtx)
{
    static const osMutexAttr_t attr = {
        .attr_bits = osMutexRecursive,
        .name = NULL,
        .cb_mem = NULL,
        .cb_size = 0,
    };
    *mtx = osMutexNew(&attr);
    return *mtx ? HAL_STATUS_OK : HAL_STATUS_ERROR;
}

static __inline HAL_Status HAL_MutexDeinit(HAL_Mutex *mtx)
{
    return osMutexDelete(*mtx) == osOK ? HAL_STATUS_OK : HAL_STATUS_ERROR;
}

static __inline HAL_Status HAL_MutexLock(HAL_Mutex *mtx, uint32_t msec)
{
    return osMutexAcquire(*mtx, msec) == osOK ? HAL_STATUS_OK : HAL_STATUS_ERROR;
}

static __inline HAL_Status HAL_MutexTryLock(HAL_Mutex *mtx)
{
    return HAL_MutexLock(mtx, 0);
}

static __inline HAL_Status HAL_MutexUnlock(HAL_Mutex *mtx)
{
    return osMutexRelease(*mtx) == osOK ? HAL_STATUS_OK : HAL_STATUS_ERROR;
}

typedef osMessageQueueId_t HAL_Queue;

static __inline HAL_Status HAL_QueueInit(HAL_Queue *queue, uint32_t msgSize, uint32_t msgNum)
{
    *queue = osMessageQueueNew(msgNum, msgSize, NULL);
    return *queue ? HAL_STATUS_OK : HAL_STATUS_ERROR;
}

static __inline HAL_Status HAL_QueueDeinit(HAL_Queue *queue)
{
    return osMessageQueueDelete(*queue) == osOK ? HAL_STATUS_OK : HAL_STATUS_ERROR;
}

static __inline HAL_Status HAL_QueueSend(HAL_Queue *queue, const void *msg, uint32_t msec)
{
    return osMessageQueuePut(*queue, msg, 0, msec) == osOK ? HAL_STATUS_OK : HAL_STATUS_ERROR;
}

static __inline HAL_Status HAL_QueueTrySend(HAL_Queue *queue, const void *msg)
{
    return HAL_QueueSend(queue, msg, 0);
}

static __inline HAL_Status HAL_QueueRecv(HAL_Queue *queue, void *msg, uint32_t msec)
{
    return osMessageQueueGet(*queue, msg, NULL, msec) == osOK ? HAL_STATUS_OK : HAL_STATUS_ERROR;
}

static __inline HAL_Status HAL_QueueTryRecv(HAL_Queue *queue, void *msg)
{
    return HAL_QueueRecv(queue, msg, 0);
}




/* Thread */
typedef osThreadId_t HAL_Thread;


enum {
    HAL_OS_PRIORITY_IDLE = osPriorityIdle,
    HAL_OS_PRIORITY_LOWEST = osPriorityLow,
    HAL_OS_PRIORITY_LOW = osPriorityLow1,
    HAL_OS_PRIORITY_BELOW_NORMAL = osPriorityBelowNormal,
    HAL_OS_PRIORITY_NORMAL = osPriorityNormal,
    HAL_OS_PRIORITY_ABOVE_NORMAL = osPriorityAboveNormal,
    HAL_OS_PRIORITY_HIGH = osPriorityHigh,
    HAL_OS_PRIORITY_REALTIME = osPriorityRealtime,
};
typedef uint8_t HAL_OS_Priority;



static __inline HAL_Status HAL_ThreadCreate(void (*entry)(void *), const char *name, int stackSize, void *arg, const int priority, HAL_Thread *thread)
{
    static osThreadAttr_t attr = {0};

    attr.name = name;
    attr.stack_size = stackSize;
    attr.priority = priority;

    osThreadId_t  threadId = osThreadNew(entry, arg, &attr);
    if (thread){
        *thread = threadId;
    }

    return threadId ? HAL_STATUS_OK : HAL_STATUS_ERROR;
}

static __inline HAL_Status HAL_ThreadDelete(HAL_Thread *thread)
{
    return osThreadTerminate(*thread) == osOK ? HAL_STATUS_OK : HAL_STATUS_ERROR;
}

static __inline HAL_Status HAL_ThreadSuspend(HAL_Thread *thread)
{
    return osThreadSuspend(*thread) == osOK ? HAL_STATUS_OK : HAL_STATUS_ERROR;
}

static __inline HAL_Status HAL_ThreadResume(HAL_Thread *thread)
{
    return osThreadResume(*thread) == osOK ? HAL_STATUS_OK : HAL_STATUS_ERROR;
}

static __inline HAL_Status HAL_ThreadSetPriority(HAL_Thread *thread, int priority)
{
    return osThreadSetPriority(*thread, priority) == osOK ? HAL_STATUS_OK : HAL_STATUS_ERROR;
}

static __inline HAL_Status HAL_ThreadGetPriority(HAL_Thread *thread, int *priority)
{
    *priority = osThreadGetPriority(*thread);
    return HAL_STATUS_OK;
}

static __inline HAL_Thread HAL_ThreadGetID()
{
    return osThreadGetId();
}

static __inline HAL_Status HAL_ThreadDelay(uint32_t msec)
{
    osDelay(msec);
    return HAL_STATUS_OK;
}

static __inline HAL_Status HAL_ThreadSuspendScheduler(void)
{
    return osKernelLock() == osOK ? HAL_STATUS_OK : HAL_STATUS_ERROR;
}

static __inline HAL_Status HAL_ThreadResumeScheduler(void)
{
    return osKernelUnlock() == osOK ? HAL_STATUS_OK : HAL_STATUS_ERROR;
}

static __inline int HAL_ThreadIsSchedulerRunning(void)
{
    return osKernelGetState() == osKernelRunning;
}

static __inline void HAL_ThreadStartScheduler(void)
{
    osKernelStart();
}

static __inline void HAL_ThreadInitilize(void)
{
    osKernelInitialize();
}

/* Timer */
typedef osTimerId_t HAL_Timer;
enum {
    HAL_TIMER_PERIODIC = osTimerPeriodic,
    HAL_TIMER_ONESHOT = osTimerOnce,
};
typedef uint8_t HAL_TimerType;


static __inline HAL_Status HAL_TimerInit(HAL_Timer *timer, HAL_TimerType type, void (*cb)(void *), void *arg)
{
    *timer = osTimerNew(cb, type, arg, NULL);
    return *timer ? HAL_STATUS_OK : HAL_STATUS_ERROR;
}

static __inline HAL_Status HAL_TimerDeinit(HAL_Timer *timer)
{
    return osTimerDelete(*timer) == osOK ? HAL_STATUS_OK : HAL_STATUS_ERROR;
}

static __inline HAL_Status HAL_TimerStart(HAL_Timer *timer,uint32_t msec)
{
    return osTimerStart(*timer, msec) == osOK ? HAL_STATUS_OK : HAL_STATUS_ERROR;
}

static __inline HAL_Status HAL_TimerChangePeriod(HAL_Timer *timer, uint32_t msec)
{
    return osTimerStart(*timer, msec) == osOK ? HAL_STATUS_OK : HAL_STATUS_ERROR;
}

static __inline HAL_Status HAL_TimerStop(HAL_Timer *timer)
{
    return osTimerStop(*timer) == osOK ? HAL_STATUS_OK : HAL_STATUS_ERROR;
}

static __inline int HAL_TimerIsActive(HAL_Timer *timer)
{
    return osTimerIsRunning(*timer);
}
#endif

/* Keep system alive, eg. feed watchdog */
static __inline void HAL_Alive(void)
{
//    HAL_WDG_Feed();
}

/* Time */
static __inline uint32_t HAL_Ticks(void)
{
#ifdef USE_RTOS_SYSTEM
    return osKernelGetTickCount();
#else
    return HAL_GetTick();
#endif
}

static __inline void HAL_msleep(uint32_t msec)
{
#ifdef USE_RTOS_SYSTEM
    osDelay(msec);
#else
    HAL_Delay(msec);
#endif
}

static __inline uint32_t HAL_SecsToTicks(uint32_t sec)
{
    return sec * 1000;
}
static __inline uint32_t HAL_MSecsToTicks(uint32_t msec)
{
    return msec;
}
static __inline uint32_t HAL_TicksToMSecs(uint32_t t)
{
    return t;
}
static __inline uint32_t HAL_TicksToSecs(uint32_t t)
{
    return t / 1000;
}

static __inline uint32_t HAL_millis(void)
{
    return HAL_TicksToMSecs(HAL_Ticks());
}
static __inline uint32_t HAL_seconds(void)
{
    return HAL_TicksToSecs(HAL_Ticks());
}

/**
 * @brief Get the elapesd millis from start
 * @param start
 * @return
 */
static __inline uint32_t HAL_elapsedMillis(uint32_t start)
{
    return HAL_millis() - start;
}

static __inline uint32_t HAL_elapsedSeconds(uint32_t start)
{
    return HAL_seconds() - start;
}

/* Memory */
#define HAL_Malloc(l)           malloc(l)
#define HAL_Free(p)             free(p)
#define HAL_Memcpy(d, s, l)     memcpy(d, s, l)
#define HAL_Memset(d, c, l)     memset(d, c, l)
#define HAL_Memcmp(a, b, l)     memcmp(a, b, l)
#define HAL_Memmove(d, s, n)    memmove(d, s, n)

#ifdef __cplusplus
}
#endif



#endif //MINIFOC_F4_HAL_OS_H
