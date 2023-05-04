
#include "RCOutput.h"

using namespace STM32;

void STM32RCOutput::init(void* machtnichts) {}

void STM32RCOutput::set_freq(uint32_t chmask, uint16_t freq_hz) {
    // we can't set this per channel yet
    if (freq_hz > 50) {
        // we're being asked to set the alt rate

        _freq_hz = freq_hz;
    }

    /* work out the new rate mask. The PX4IO board has 3 groups of servos.

       Group 0: channels 0 1
       Group 1: channels 4 5 6 7
       Group 2: channels 2 3

       Channels within a group must be set to the same rate.

       For the moment we never set the channels above 8 to more than
       50Hz
     */
    if (freq_hz > 50) {
        // we are setting high rates on the given channels
        _rate_mask |= chmask & 0xFF;
        if (_rate_mask & 0x3) {
            _rate_mask |= 0x3;
        }
        if (_rate_mask & 0xc) {
            _rate_mask |= 0xc;
        }
        if (_rate_mask & 0xF0) {
            _rate_mask |= 0xF0;
        }
    } else {
        // we are setting low rates on the given channels
        if (chmask & 0x3) {
            _rate_mask &= ~0x3;
        }
        if (chmask & 0xc) {
            _rate_mask &= ~0xc;
        }
        if (chmask & 0xf0) {
            _rate_mask &= ~0xf0;
        }
    }
}

uint16_t STM32RCOutput::get_freq(uint8_t ch) {
    if (_rate_mask & (1U<<ch)) {
        return _freq_hz;
    }
    return 50;
}

void STM32RCOutput::enable_ch(uint8_t ch)
{
    _enabled_channels |= (1U<<ch);
}

void STM32RCOutput::disable_ch(uint8_t ch)
{
    _enabled_channels &= ~(1U<<ch);
}

void STM32RCOutput::write(uint8_t ch, uint16_t period_us)
{
    if (ch >= _servo_count + _alt_servo_count) {
        return;
    }
    if (!(_enabled_channels & (1U<<ch))) {
        // not enabled
        return;
    }
    if (ch >= _max_channel) {
        _max_channel = ch + 1;
    }
    if (period_us != _period[ch]) {
        _period[ch] = period_us;
        _need_update = true;
    }
}

void STM32RCOutput::write(uint8_t ch, uint16_t* period_us, uint8_t len)
{
    for (uint8_t i=0; i<len; i++) {
        write(i, period_us[i]);
    }
}

uint16_t STM32RCOutput::read(uint8_t ch) {
    if (ch >= STM32_NUM_OUTPUT_CHANNELS) {
        return 0;
    }
    return _period[ch];
}

void STM32RCOutput::read(uint16_t* period_us, uint8_t len)
{
    for (uint8_t i=0; i<len; i++) {
        period_us[i] = read(i);
    }
}

