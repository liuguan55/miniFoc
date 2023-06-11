
// * Copyright (C) 2023 dengzhijun. All rights reserved.
//  *
//  *  Redistribution and use in source and binary forms, with or without
//  *  modification, are permitted provided that the following conditions
//  *  are met:
//  *    1. Redistributions of source code must retain the above copyright
//  *       notice, this list of conditions and the following disclaimer.
//  *    2. Redistributions in binary form must reproduce the above copyright
//  *       notice, this list of conditions and the following disclaimer in the
//  *       documentation and/or other materials provided with the
//  *       distribution.
//  *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
//  *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
//  *  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
//  *  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
//  *  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
//  *  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
//  *  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
//  *  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
//  *  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
//  *  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
//  *  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//  *


#include "AnalogIn.h"
#include "driver/adc.h"

using namespace STM32;

#define ADC_SAMPLETIME_3CYCLES    0x00000000U
#define ADC_SAMPLETIME_15CYCLES   ((uint32_t)ADC_SMPR1_SMP10_0)
#define ADC_SAMPLETIME_28CYCLES   ((uint32_t)ADC_SMPR1_SMP10_1)
#define ADC_SAMPLETIME_56CYCLES   ((uint32_t)(ADC_SMPR1_SMP10_1 | ADC_SMPR1_SMP10_0))
#define ADC_SAMPLETIME_84CYCLES   ((uint32_t)ADC_SMPR1_SMP10_2)
#define ADC_SAMPLETIME_112CYCLES  ((uint32_t)(ADC_SMPR1_SMP10_2 | ADC_SMPR1_SMP10_0))
#define ADC_SAMPLETIME_144CYCLES  ((uint32_t)(ADC_SMPR1_SMP10_2 | ADC_SMPR1_SMP10_1))
#define ADC_SAMPLETIME_480CYCLES  ((uint32_t)ADC_SMPR1_SMP10)

static const uint32_t sAdcMapTable[AP_HAL::AnalogSource::HAL_ADC_SAMPLETIME_NR] = {
        ADC_SAMPLETIME_3CYCLES,
        ADC_SAMPLETIME_15CYCLES,
        ADC_SAMPLETIME_28CYCLES,
        ADC_SAMPLETIME_56CYCLES,
        ADC_SAMPLETIME_84CYCLES,
        ADC_SAMPLETIME_112CYCLES,
        ADC_SAMPLETIME_144CYCLES,
        ADC_SAMPLETIME_480CYCLES,
};

STM32AnalogSource::STM32AnalogSource(float v) :
        _v(v),
        _lastValue(0),
        _averageVoltage(0.0f),
        _lastVoltage(0.0f),
        _conversionTime(HAL_ADC_SAMPLETIME_56CYCLES) {

}

float STM32AnalogSource::read_average() {
    _lastValue  =  analogRead(_v, sAdcMapTable[_conversionTime], 100);
    float averageValue = _filter.apply(_lastValue);

    _averageVoltage = 5.0f * averageValue / 1024.0f;;
    _lastVoltage = 5.0f * _lastValue / 1024.0f;

    return averageValue ;
}

float STM32AnalogSource::voltage_average() {
    return _averageVoltage;
}

float STM32AnalogSource::voltage_latest() {
    return _lastVoltage;
}

float STM32AnalogSource::read_latest() {
    return _lastValue;
}

void STM32AnalogSource::set_pin(uint8_t p) {
    UNUSED(p);
}

void STM32AnalogSource::set_stop_pin(uint8_t p) {
    UNUSED(p);
}

void STM32AnalogSource::set_settle_time(uint16_t settle_time_ms) {
    _conversionTime = settle_time_ms;
}

STM32AnalogIn::STM32AnalogIn() {}

void STM32AnalogIn::init(void *implspecific) {
    UNUSED(implspecific);

    MX_ADC1_Init();
}

AP_HAL::AnalogSource *STM32AnalogIn::channel(int16_t n) {
    return new STM32AnalogSource(n);
}

float STM32AnalogIn::board_voltage(void) {
    unsigned short raw_adc = analogRead(ADC_CHANNEL_4, ADC_SAMPLETIME_84CYCLES, 100);
    return (float) raw_adc * 69.3 / 4096;
}
