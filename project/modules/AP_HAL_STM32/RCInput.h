
#ifndef __AP_HAL_STM32_RCINPUT_H__
#define __AP_HAL_STM32_RCINPUT_H__

#include "AP_HAL_STM32.h"

class STM32::STM32RCInput : public AP_HAL::RCInput {
public:
    STM32RCInput();
    void init(void* machtnichts) override;
    bool  new_input()   override;
    uint8_t num_channels() override;
    uint16_t read(uint8_t ch) override;
    uint8_t read(uint16_t* periods, uint8_t len) override;

    bool set_overrides(int16_t *overrides, uint8_t len) override;
    bool set_override(uint8_t channel, int16_t override) override;
    void clear_overrides() override;

private:
    bool new_rc_input;

    /* override state */
    uint16_t _override[8];
};

#endif // __AP_HAL_STM32_RCINPUT_H__
