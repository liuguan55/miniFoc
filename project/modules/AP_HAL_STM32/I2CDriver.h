
#ifndef __AP_HAL_STM32_I2CDRIVER_H__
#define __AP_HAL_STM32_I2CDRIVER_H__

#include "AP_HAL_STM32.h"
#include "driver/i2c.h"

class STM32::STM32I2CDriver : public AP_HAL::I2CDriver {
public:
    STM32I2CDriver(AP_HAL::Mutex* mutex, uint8_t bus) : _mutex(mutex),
        _bus(bus),_speed(100000),_timeout(100),_errorCount(0) {
        if (_bus == AP_HAL::I2CDriver::HAL_I2C_BUS_1){
            _hi2c = &hi2c1;
        } else if (_bus == AP_HAL::I2CDriver::HAL_I2C_BUS_2){
        } else if (_bus == AP_HAL::I2CDriver::HAL_I2C_BUS_3){
        }
    }
    void begin();
    void end();
    void setTimeout(uint16_t ms);
    void setHighSpeed(bool active);

    /* write: for i2c devices which do not obey register conventions */
    uint8_t write(uint8_t addr, uint8_t len, uint8_t* data);
    /* writeRegister: write a single 8-bit value to a register */
    uint8_t writeRegister(uint8_t addr, uint8_t reg, uint8_t val);
    /* writeRegisters: write bytes to contigious registers */
    uint8_t writeRegisters(uint8_t addr, uint8_t reg,
                                   uint8_t len, uint8_t* data);

    /* read: for i2c devices which do not obey register conventions */
    uint8_t read(uint8_t addr, uint8_t len, uint8_t* data);
    /* readRegister: read from a device register - writes the register,
     * then reads back an 8-bit value. */
    uint8_t readRegister(uint8_t addr, uint8_t reg, uint8_t* data);
    /* readRegister: read contigious device registers - writes the first 
     * register, then reads back multiple bytes */
    uint8_t readRegisters(uint8_t addr, uint8_t reg,
                                  uint8_t len, uint8_t* data);

    uint8_t lockup_count();

    AP_HAL::Mutex* get_mutex() { return _mutex; }

private:
    AP_HAL::Mutex* _mutex;
    I2C_HandleTypeDef *_hi2c;
    uint8_t _bus;
    uint32_t _speed;
    uint16_t  _timeout;
    uint8_t _errorCount;
};

#endif // __AP_HAL_STM32_I2CDRIVER_H__
