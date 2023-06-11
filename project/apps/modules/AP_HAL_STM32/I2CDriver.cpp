
#include <AP_HAL/AP_HAL.h>
#include "I2CDriver.h"
#include "driver/i2c.h"
#include "log/easylogger/elog.h"

using namespace STM32;

void STM32I2CDriver::begin() {
    if (_bus == AP_HAL::I2CDriver::HAL_I2C_BUS_1) {
        _hi2c->Init.ClockSpeed = _speed;
        HAL_I2C_Init(_hi2c);
    } else if (_bus == AP_HAL::I2CDriver::HAL_I2C_BUS_2) {
    } else if (_bus == AP_HAL::I2CDriver::HAL_I2C_BUS_3) {
    } else {
        log_e("not support i2c bus\n");
    }
}

void STM32I2CDriver::end() {
    if (_bus == AP_HAL::I2CDriver::HAL_I2C_BUS_1) {
        HAL_I2C_DeInit(_hi2c);
    } else if (_bus == AP_HAL::I2CDriver::HAL_I2C_BUS_2) {
    } else if (_bus == AP_HAL::I2CDriver::HAL_I2C_BUS_3) {
    } else {
        log_e("not support i2c bus\n");
    }
}

void STM32I2CDriver::setTimeout(uint16_t ms) {}

void STM32I2CDriver::setHighSpeed(bool active) {
    _speed = 400000;
    begin();
}

uint8_t STM32I2CDriver::write(uint8_t addr, uint8_t len, uint8_t *data) {
    HAL_StatusTypeDef status = HAL_I2C_Master_Transmit(_hi2c, addr, data, len, _timeout);
    if (status != HAL_OK) {
        _errorCount++;
    }

    return status;
}

uint8_t STM32I2CDriver::writeRegister(uint8_t addr, uint8_t reg, uint8_t val) {
    uint8_t buf[2];

    buf[0] = reg ;
    buf[1] = val ;
    return write(addr, 2, buf);
}

uint8_t STM32I2CDriver::writeRegisters(uint8_t addr, uint8_t reg,
                                       uint8_t len, uint8_t *data) {
    uint8_t buf[len+1];
    buf[0] = reg;

    if (len != 0) {
        memcpy(&buf[1], data, len);
    }
    return write(addr, len+1, buf);
}

uint8_t STM32I2CDriver::read(uint8_t addr, uint8_t len, uint8_t *data) {
    HAL_StatusTypeDef status = HAL_I2C_Master_Receive(_hi2c, addr, data, len, _timeout);
    if (status != HAL_OK) {
        _errorCount++;
    }
    return status;
}

uint8_t STM32I2CDriver::readRegister(uint8_t addr, uint8_t reg, uint8_t *data) {
    uint8_t status;

    write(addr, 1, &reg);
    status = read(addr, 1, data);

    return status;
}

uint8_t STM32I2CDriver::readRegisters(uint8_t addr, uint8_t reg,
                                      uint8_t len, uint8_t *data) {
    uint8_t status;

    write(addr, 1, &reg);
    status = read(addr, len, data);

    return status;
}

uint8_t STM32I2CDriver::lockup_count() {
    return _errorCount;
}
