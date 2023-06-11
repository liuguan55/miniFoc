
#include "Mutex.h"

using namespace Empty;

bool EmptyMutex::lock(uint32_t timeoutMs)  {
    UNUSED(timeoutMs);

    if (_taken) {
        _taken = false;
        return true;
    } else {
        return false;
    }
}

bool EmptyMutex::unlock()  {
    return false;
}
