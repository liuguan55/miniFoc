
#ifndef __AP_HAL_STM32_CLASS_H__
#define __AP_HAL_STM32_CLASS_H__

#include <AP_HAL/AP_HAL.h>

#include "AP_HAL_STM32_Namespace.h"
#include "PrivateMember.h"

class HAL_STM32 : public AP_HAL::HAL {
public:
    HAL_STM32();
    void init(int argc, char * const * argv) const;

    static const AP_HAL::HAL& getInstance(void) ;
private:
    STM32::STM32PrivateMember *_member;
};

#endif // __AP_HAL_STM32_CLASS_H__

