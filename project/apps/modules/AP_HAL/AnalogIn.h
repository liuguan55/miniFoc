
#ifndef __AP_HAL_ANALOG_IN_H__
#define __AP_HAL_ANALOG_IN_H__

#include "AP_HAL_Namespace.h"




class AP_HAL::AnalogSource {
public:
    enum {
        HAL_ADC_SAMPLETIME_3CYCLES = 0,
        HAL_ADC_SAMPLETIME_15CYCLES,
        HAL_ADC_SAMPLETIME_28CYCLES,
        HAL_ADC_SAMPLETIME_56CYCLES,
        HAL_ADC_SAMPLETIME_84CYCLES,
        HAL_ADC_SAMPLETIME_112CYCLES,
        HAL_ADC_SAMPLETIME_144CYCLES,
        HAL_ADC_SAMPLETIME_480CYCLES,
        HAL_ADC_SAMPLETIME_NR,
    };
    virtual float read_average() = 0;
    virtual float read_latest() = 0;
    virtual void set_pin(uint8_t p) = 0;

    // optionally allow setting of a pin that stops the device from
    // reading. This is needed for sonar devices where you have more
    // than one sonar, and you want to stop them interfering with each
    // other. It assumes that if held low the device is stopped, if
    // held high the device starts reading.    
    virtual void set_stop_pin(uint8_t p) = 0;

    // optionally allow a settle period in milliseconds. This is only
    // used if a stop pin is set. If the settle period is non-zero
    // then the analog input code will wait to get a reading for that
    // number of milliseconds. Note that this will slow down the
    // reading of analog inputs.
    virtual void set_settle_time(uint16_t settle_time_ms) = 0;

    // return a voltage from 0.0 to 5.0V, scaled
    // against a reference voltage
    virtual float voltage_average() = 0;

    // return a voltage from 0.0 to 5.0V, scaled
    // against a reference voltage
    virtual float voltage_latest() = 0;

    // return a voltage from 0.0 to 5.0V, assuming a ratiometric
    // sensor
    virtual float voltage_average_ratiometric() = 0;
};

class AP_HAL::AnalogIn {
public:
    virtual void init(void* implspecific) = 0;
    virtual AP_HAL::AnalogSource* channel(int16_t n) = 0;

    // board 5V rail voltage in volts
    virtual float board_voltage(void) = 0;

    // servo rail voltage in volts, or 0 if unknown
    virtual float servorail_voltage(void) { return 0; }

    // power supply status flags, see MAV_POWER_STATUS
    virtual uint16_t power_status_flags(void) { return 0; }
};

#define ANALOG_INPUT_BOARD_VCC 254
#define ANALOG_INPUT_NONE 255

#endif // __AP_HAL_ANALOG_IN_H__

