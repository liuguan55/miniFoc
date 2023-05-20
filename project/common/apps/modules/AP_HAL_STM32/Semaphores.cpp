
#include "Semaphores.h"
#include "kernel/FreeRTOS/CMSIS_RTOS_V2/cmsis_os2.h"

using namespace STM32;

bool STM32Semaphore::give() {
    osStatus_t status = osSemaphoreRelease(_sem);

    return status == osOK;
}

bool STM32Semaphore::take(uint32_t timeout_ms) {
    osStatus_t status = osSemaphoreAcquire(_sem, timeout_ms);
    return status == osOK;
}

bool STM32Semaphore::take_nonblocking() {
    return false ;
}
