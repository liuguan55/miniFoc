
#ifndef __AP_HAL_NAMESPACE_H__
#define __AP_HAL_NAMESPACE_H__

#include "string.h"
#include "utility/FastDelegate.h"

namespace AP_HAL {

    /* Toplevel pure virtual class Hal.*/
    class HAL;

    /* Toplevel class names for drivers: */
    class UARTDriver;
    class I2CDriver;

    class SPIDeviceDriver;
    class SPIDeviceManager;

    class AnalogSource;
    class AnalogIn;
    class Storage;
    class DigitalSource;
    class GPIO;
    class RCInput;
    class RCOutput;
    class Scheduler;
    class Semaphore;
    class Mutex;

    class Util;

    /* Utility Classes */
    class Print;
    class Stream;
    class BetterStream;

    /* Typdefs for function pointers (Procedure, Member Procedure)

       For member functions we use the FastDelegate delegates class
       which allows us to encapculate a member function as a type
     */
    typedef void(*Proc)(void);
    typedef fastdelegate::FastDelegate0<> MemberProc;

    /**
     * Global names for all of the existing SPI devices on all platforms.
     */

    enum SPIDevice {
        SPIDevice_Dataflash,
        SPIDevice_ST7735S,
    };

    enum GPIODevice{
        GPIO_LED1_PIN = 0,
        GPIO_MOTOR_ENABLE_PIN ,
        GPIO_SPI1_CS1_PIN,
        GPIO_SPI2_CS1_PIN,
        GPIO_SPI2_CS2_PIN,
        GPIO_BTN_1_PIN,
        GPIO_BTN_2_PIN,
        GPIO_BTN_3_PIN,
        GPIO_BTN_4_PIN,
        GPIO_LCD_RST_PIN,
        GPIO_LCD_RES_PIN,
        GPIO_NR,
    };


}

// macro to hide the details of AP_HAL::MemberProc
#define AP_HAL_MEMBERPROC(func) fastdelegate::MakeDelegate(this, func)

#endif // __AP_HAL_NAMESPACE_H__
