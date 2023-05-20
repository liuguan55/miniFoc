#include "AP_HAL/AP_HAL_Board.h"
#include "GPIO.h"
#include "SPIDriver.h"

using namespace STM32;

#define         MHZ         (1000U*1000U)

enum {
    SPI_MODE_0,
    SPI_MODE_1,
    SPI_MODE_2,
    SPI_MODE_3,
};

static SPI_HandleTypeDef *spiMapping[STM32_SPI_NUM_BUSES] = {
   &hspi1,
   &hspi2,
};

STM32SPIDeviceDriver STM32SPIDeviceManager::_device[STM32_SPI_DEVICE_NUM_DEVICES] = {
        /* MPU9250 is restricted to 1MHz for non-data and interrupt registers */
        STM32SPIDeviceDriver(0, AP_HAL::SPIDevice_ST7735S, SPI_MODE_0, 8, AP_HAL::GPIO_SPI2_CS1_PIN,  1*MHZ, 16*MHZ),
        STM32SPIDeviceDriver(1, AP_HAL::SPIDevice_Dataflash, SPI_MODE_0, 8, AP_HAL::GPIO_SPI2_CS1_PIN,  1*MHZ, 16*MHZ),
};
// have a separate semaphore per bus
STM32Mutex STM32SPIDeviceManager::_mutex[STM32_SPI_NUM_BUSES];

STM32SPIDeviceDriver::STM32SPIDeviceDriver(uint8_t bus, enum AP_HAL::SPIDevice type, uint8_t mode, uint8_t bitsPerWord, uint8_t cs_pin, uint32_t lowspeed, uint32_t highspeed):
    _cs_pin(cs_pin),
    _mode(mode),
    _bitsPerWord(bitsPerWord),
    _lowspeed(lowspeed),
    _highspeed(highspeed),
    _speed(highspeed),
    _type(type),
    _bus(bus)
{
    _hspi = spiMapping[_bus];
}

void STM32SPIDeviceDriver::init()
{
    // Init the SPI
    hspi1.Instance = SPI1;
    hspi1.Init.Mode = SPI_MODE_MASTER;
    hspi1.Init.Direction = SPI_DIRECTION_2LINES;
    hspi1.Init.DataSize = SPI_DATASIZE_8BIT;
    hspi1.Init.CLKPolarity = SPI_POLARITY_LOW;
    hspi1.Init.CLKPhase = SPI_PHASE_1EDGE;
    hspi1.Init.NSS = SPI_NSS_HARD_OUTPUT;
    hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_2;
    hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
    hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
    hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
    hspi1.Init.CRCPolynomial = 10;

    uint32_t DataSize = SPI_DATASIZE_8BIT;
    if (_bitsPerWord == 8){
        DataSize = SPI_DATASIZE_8BIT;
    } else if (_bitsPerWord == 16){
        DataSize = SPI_DATASIZE_16BIT;
    } else {
        hal.scheduler->panic("Invalid bitsPerWord");
    }

    uint32_t CLKPolarity = SPI_POLARITY_LOW;
    uint32_t CLKPhase = SPI_PHASE_1EDGE;
    if (_mode == SPI_MODE_0){
        CLKPolarity = SPI_POLARITY_LOW;
        CLKPhase = SPI_PHASE_1EDGE;
    } else if (_mode == SPI_MODE_1){
        CLKPolarity = SPI_POLARITY_LOW;
        CLKPhase = SPI_PHASE_2EDGE;
    } else if (_mode == SPI_MODE_2){
        CLKPolarity = SPI_POLARITY_HIGH;
        CLKPhase = SPI_PHASE_1EDGE;
    } else if (_mode == SPI_MODE_3){
        CLKPolarity = SPI_POLARITY_HIGH;
        CLKPhase = SPI_PHASE_2EDGE;
    } else {
        hal.scheduler->panic("Invalid mode");
    }

    uint32_t baudrate = 0;
    if (_speed == _lowspeed){
        baudrate = SPI_BAUDRATEPRESCALER_64;
    } else if (_speed == _highspeed){
        baudrate = SPI_BAUDRATEPRESCALER_2;
    } else {
        hal.scheduler->panic("Invalid speed");
    }

    _hspi->Init.DataSize = DataSize;
    _hspi->Init.CLKPolarity = CLKPolarity;
    _hspi->Init.CLKPhase = CLKPhase;
    _hspi->Init.BaudRatePrescaler = baudrate;
    HAL_SPI_Init(_hspi);

    // Init the CS
    _cs = hal.gpio->channel(_cs_pin);
    if (_cs == nullptr) {
        hal.scheduler->panic("Unable to instantiate cs pin");
    }

    _cs->mode(HAL_GPIO_OUTPUT);
    _cs->write(HAL_GPIO_HIGH);       // do not hold the SPI bus initially
}

AP_HAL::Mutex* STM32SPIDeviceDriver::get_mutex()
{
    return STM32SPIDeviceManager::get_mutex(_bus);
}

void STM32SPIDeviceDriver::transaction(const uint8_t *tx, uint8_t *rx, uint16_t len)
{
    STM32SPIDeviceManager::transaction(*this, tx, rx, len);
}


void STM32SPIDeviceDriver::cs_assert()
{
    STM32SPIDeviceManager::cs_assert(_type);
}

void STM32SPIDeviceDriver::cs_release()
{
    STM32SPIDeviceManager::cs_release(_type);
}

uint8_t STM32SPIDeviceDriver::transfer (uint8_t data)
{
    uint8_t v = 0;
    transaction(&data, &v, 1);

    return v;
}

void STM32SPIDeviceDriver::transfer (const uint8_t *data, uint16_t len)
{
    transaction(data, NULL, len);
}

void STM32SPIDeviceDriver::set_bus_speed(AP_HAL::SPIDeviceDriver::bus_speed speed) {
    if (speed == SPI_SPEED_LOW) {
        _speed = _lowspeed;
    } else {
        _speed = _highspeed;
    }
}

STM32SPIDeviceManager::STM32SPIDeviceManager()
{}

void STM32SPIDeviceManager::init(void *)
{
    for (uint8_t i=0; i < STM32_SPI_DEVICE_NUM_DEVICES; i++) {
        _device[i].init();
    }
}

AP_HAL::SPIDeviceDriver* STM32SPIDeviceManager::device(enum AP_HAL::SPIDevice dev)
{
    for (uint8_t i=0; i < STM32_SPI_DEVICE_NUM_DEVICES; i++) {
        if (_device[i]._type == dev) {
            return &_device[i];
        }
    }

    return nullptr;
}

AP_HAL::Mutex *STM32SPIDeviceManager::get_mutex(uint8_t bus) {
    return &_mutex[bus];
}

void STM32SPIDeviceManager::cs_assert(AP_HAL::SPIDevice type) {
    uint8_t bus = 0, i;

    for (i=0; i<STM32_SPI_DEVICE_NUM_DEVICES; i++) {
        if (_device[i]._type == type) {
            bus = _device[i]._bus;
            break;
        }
    }

    if (i == STM32_SPI_DEVICE_NUM_DEVICES) {
        hal.scheduler->panic("Bad device type");
    }

    for (i=0; i<STM32_SPI_DEVICE_NUM_DEVICES; i++) {
        if (_device[i]._bus != bus) {
            // not the same bus
            continue;
        }
        if (_device[i]._type != type) {
            if (_device[i]._cs->read() != 1) {
                hal.console->printf("two CS enabled at once i=%u %u and %u\n",
                                    (unsigned)i, (unsigned)type, (unsigned)_device[i]._type);
            }
        }
    }

    for (i=0; i<STM32_SPI_DEVICE_NUM_DEVICES; i++) {
        if (_device[i]._type == type) {
            _device[i]._cs->write(0);
        }
    }
}

void STM32SPIDeviceManager::cs_release(AP_HAL::SPIDevice type) {
    uint8_t bus = 0, i;

    for (i=0; i<STM32_SPI_DEVICE_NUM_DEVICES; i++) {
        if (_device[i]._type == type) {
            bus = _device[i]._bus;
            break;
        }
    }

    if (i == STM32_SPI_DEVICE_NUM_DEVICES) {
        hal.scheduler->panic("Bad device type");
    }

    for (i=0; i<STM32_SPI_DEVICE_NUM_DEVICES; i++) {
        if (_device[i]._bus != bus) {
            // not the same bus
            continue;
        }
        _device[i]._cs->write(1);
    }
}

void STM32SPIDeviceManager::transaction(STM32SPIDeviceDriver &driver, const uint8_t *tx, uint8_t *rx, uint16_t len) {
    // we set the mode before we assert the CS line so that the bus is
    cs_assert(driver._type);
    HAL_SPI_TransmitReceive(driver._hspi, (uint8_t *)tx, rx, len, 1000);
    cs_release(driver._type);
}

