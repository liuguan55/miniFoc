
#ifndef __AP_HAL_STM32_RCOUTPUT_H__
#define __AP_HAL_STM32_RCOUTPUT_H__

#include "AP_HAL_STM32.h"


#define STM32_NUM_OUTPUT_CHANNELS 16

class STM32::STM32RCOutput : public AP_HAL::RCOutput {
    void     init(void* machtnichts);
    void     set_freq(uint32_t chmask, uint16_t freq_hz);
    uint16_t get_freq(uint8_t ch);
    void     enable_ch(uint8_t ch);
    void     disable_ch(uint8_t ch);
    void     write(uint8_t ch, uint16_t period_us);
    void     write(uint8_t ch, uint16_t* period_us, uint8_t len);
    uint16_t read(uint8_t ch);
    void     read(uint16_t* period_us, uint8_t len);

private:
    uint16_t _freq_hz;
    uint16_t _period[STM32_NUM_OUTPUT_CHANNELS];
    volatile uint8_t _max_channel;
    volatile bool _need_update;
    uint32_t _last_output;
    unsigned _servo_count;
    unsigned _alt_servo_count;
    uint32_t _rate_mask;
    uint16_t _enabled_channels;
};

#endif // __AP_HAL_STM32_RCOUTPUT_H__
