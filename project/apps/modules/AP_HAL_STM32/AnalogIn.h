
#ifndef __AP_HAL_STM32_ANALOGIN_H__
#define __AP_HAL_STM32_ANALOGIN_H__

#include <AP_HAL_STM32/AP_HAL_STM32.h>
#include "Filter/AverageFilter.h"

class STM32::STM32AnalogSource : public AP_HAL::AnalogSource {
public:
    explicit STM32AnalogSource(float v);
    float read_average() override;
    float read_latest() override;
    void set_pin(uint8_t p) override;
    void set_stop_pin(uint8_t p) override;
    void set_settle_time(uint16_t settle_time_ms) override;
    float voltage_average() override;
    float voltage_latest() override;
    float voltage_average_ratiometric() override{ return voltage_average(); }
private:
    float _v;
    int32_t _lastValue;
    float _averageVoltage;
    float _lastVoltage;
    int32_t _conversionTime;

    AverageFilterUInt16_Size5 _filter;
};

class STM32::STM32AnalogIn : public AP_HAL::AnalogIn {
public:
    STM32AnalogIn();
    void init(void* implspecific) override;
    AP_HAL::AnalogSource* channel(int16_t n) override;
    float board_voltage(void) override;
};
#endif // __AP_HAL_STM32_ANALOGIN_H__
