
#ifndef __AP_HAL_STM32_SPIDRIVER_H__
#define __AP_HAL_STM32_SPIDRIVER_H__

#include "AP_HAL_STM32.h"
#include "Mutex.h"
#include "inc/spi.h"

#define STM32_SPI_NUM_BUSES 2
#define STM32_SPI_DEVICE_NUM_DEVICES 2

class STM32::STM32SPIDeviceDriver : public AP_HAL::SPIDeviceDriver {
public:
    friend class STM32::STM32SPIDeviceManager;
    STM32SPIDeviceDriver(uint8_t bus, enum AP_HAL::SPIDevice type, uint8_t mode,
            uint8_t bitsPerWord, uint8_t cs_pin, uint32_t lowspeed, uint32_t highspeed);
    void init() override;
    AP_HAL::Mutex* get_mutex() override;
    void transaction(const uint8_t *tx, uint8_t *rx, uint16_t len) override;

    void cs_assert() override;
    void cs_release() override;
    uint8_t transfer (uint8_t data) override;
    void transfer (const uint8_t *data, uint16_t len)   override;

    void set_bus_speed(enum bus_speed speed) override;
private:
    uint8_t _cs_pin;
    AP_HAL::DigitalSource *_cs;
    uint8_t _mode;
    uint8_t _bitsPerWord;
    uint32_t _lowspeed;
    uint32_t _highspeed;
    uint32_t _speed;
    enum AP_HAL::SPIDevice _type;
    uint8_t _bus;
    SPI_HandleTypeDef *_hspi;
};

class STM32::STM32SPIDeviceManager : public AP_HAL::SPIDeviceManager {
public:
    STM32SPIDeviceManager();
    void init(void *) override;
    AP_HAL::SPIDeviceDriver* device(enum AP_HAL::SPIDevice) override;
    static AP_HAL::Mutex *get_mutex(uint8_t bus) ;

    static void cs_assert(enum AP_HAL::SPIDevice type);
    static void cs_release(enum AP_HAL::SPIDevice type);
    static void transaction(STM32SPIDeviceDriver &driver, const uint8_t *tx, uint8_t *rx, uint16_t len);
private:
    static STM32SPIDeviceDriver _device[STM32_SPI_DEVICE_NUM_DEVICES];
    static STM32Mutex _mutex[STM32_SPI_NUM_BUSES];
};

#endif // __AP_HAL_STM32_SPIDRIVER_H__
