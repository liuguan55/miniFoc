#include <stdio.h>
#include <AP_HAL/AP_HAL.h>

#include "HAL_STM32_Class.h"
#include "AP_HAL_STM32_Private.h"

using namespace STM32;

static STM32UARTDriver uartADriver(STM32UARTDriver::HAL_STM32_UART1);
static STM32UARTDriver uartBDriver(STM32UARTDriver::HAL_STM32_UART2);
static STM32UARTDriver uartCDriver(STM32UARTDriver::HAL_STM32_UART3);
static STM32UARTDriver uartDDriver(STM32UARTDriver::HAL_STM32_UART4);
static STM32UARTDriver uartEDriver(STM32UARTDriver::HAL_STM32_UART5);
static STM32Mutex  i2cMutex;
static STM32I2CDriver  i2cDriver(&i2cMutex, AP_HAL::I2CDriver::HAL_I2C_BUS_1);
static STM32SPIDeviceManager spiDeviceManager;
static STM32AnalogIn analogIn;
static STM32Storage storageDriver;
static STM32GPIO gpioDriver;
static STM32RCInput rcinDriver;
static STM32RCOutput rcoutDriver;
static STM32Scheduler schedulerInstance;
static STM32Util utilInstance;

HAL_STM32::HAL_STM32() :
    AP_HAL::HAL(
        &uartADriver,
        &uartBDriver,
        &uartCDriver,
        &uartDDriver,
        &uartEDriver,
        &i2cDriver,
        &spiDeviceManager,
        &analogIn,
        &storageDriver,
        &uartADriver,
        &gpioDriver,
        &rcinDriver,
        &rcoutDriver,
        &schedulerInstance,
        &utilInstance),
    _member(new STM32PrivateMember(123))
{}

void HAL_STM32::init(int argc,char* const argv[]) const {
    /* initialize all drivers and private members here.
     * up to the programmer to do this in the correct order.
     * Scheduler should likely come first. */
    UNUSED(argc);
    UNUSED(argv);

    gpio->init();
    analogin->init(nullptr);
    spi->init(nullptr);
    scheduler->init(nullptr);
    scheduler->system_initialized();
//    uartA->begin(115200);
    _member->init();
}

const AP_HAL::HAL& HAL_STM32::getInstance(void) {
    static HAL_STM32 handle;

    return handle;
}

