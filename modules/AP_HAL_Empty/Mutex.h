
#ifndef __AP_HAL_EMPTY_MUTEX_H__
#define __AP_HAL_EMPTY_MUTEX_H__

#include "AP_HAL_Empty.h"

class Empty::EmptyMutex : public AP_HAL::Mutex {
public:
    EmptyMutex() : _taken(false) {}
    bool lock(uint32_t timeoutMs) ;
    bool unlock() ;
private:
    bool _taken;
};

#endif // __AP_HAL_EMPTY_MUTEX_H__
