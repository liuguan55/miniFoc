
#ifndef __AP_HAL_STM32_NAMESPACE_H__
#define __AP_HAL_STM32_NAMESPACE_H__

/* While not strictly required, names inside the STM32 namespace are prefixed
 * with STM32 for clarity. (Some of our users aren't familiar with all of the
 * C++ namespace rules.)
 */

namespace STM32 {
    class STM32UARTDriver;
    class STM32I2CDriver;
    class STM32SPIDeviceManager;
    class STM32SPIDeviceDriver;
    class STM32AnalogSource;
    class STM32AnalogIn;
    class STM32Storage;
    class STM32GPIO;
    class STM32DigitalSource;
    class STM32RCInput;
    class STM32RCOutput;
    class STM32Semaphore;
    class STM32Mutex;
    class STM32Scheduler;
    class STM32Util;
    class STM32PrivateMember;
}

#endif // __AP_HAL_STM32_NAMESPACE_H__

