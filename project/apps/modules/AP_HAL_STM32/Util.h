
#ifndef __AP_HAL_STM32_UTIL_H__
#define __AP_HAL_STM32_UTIL_H__

#include <AP_HAL/AP_HAL.h>
#include "AP_HAL_STM32_Namespace.h"

class STM32::STM32Util : public AP_HAL::Util {
public:
    bool run_debug_shell(AP_HAL::BetterStream *stream) { UNUSED(stream); return false; }
    /*
     get system identifier (STM32 serial number)
    */
    bool get_system_id(char buf[40]);

    uint16_t available_memory(void);
};

#endif // __AP_HAL_STM32_UTIL_H__
