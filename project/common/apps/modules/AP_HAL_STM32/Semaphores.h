
#ifndef __AP_HAL_STM32_SEMAPHORE_H__
#define __AP_HAL_STM32_SEMAPHORE_H__
#include "kernel/FreeRTOS/CMSIS_RTOS_V2/cmsis_os2.h"
#include "AP_HAL_STM32.h"


class STM32::STM32Semaphore : public AP_HAL::Semaphore {
public:
    STM32Semaphore() {
        _sem = osSemaphoreNew(1, 0 , NULL);
    }
    ~STM32Semaphore(){
        osSemaphoreDelete(_sem);
    }

    bool give();
    bool take(uint32_t timeout_ms);
    bool take_nonblocking();
private:
    osSemaphoreId_t _sem;
};

#endif // __AP_HAL_STM32_SEMAPHORE_H__
