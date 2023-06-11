
#ifndef __AP_HAL_STM32_STORAGE_H__
#define __AP_HAL_STM32_STORAGE_H__

#include "AP_HAL_STM32.h"

#define STM32_STORAGE_SIZE 4096
#define STM32_STORAGE_MAX_WRITE 512
#define STM32_STORAGE_LINE_SHIFT 9
#define STM32_STORAGE_LINE_SIZE (1<<STM32_STORAGE_LINE_SHIFT)
#define STM32_STORAGE_NUM_LINES (STM32_STORAGE_SIZE/STM32_STORAGE_LINE_SIZE)

class STM32::STM32Storage : public AP_HAL::Storage {
public:
    STM32Storage();
    void init(void *);
    void read_block(void *dst, uint16_t src, size_t n);
    void write_block(uint16_t dst, const void* src, size_t n);
    void _timer_tick(void);

private:
    volatile bool _initialised;
    void _storage_create(void);
    void _storage_open(void);
    void _mark_dirty(uint16_t loc, uint16_t length);
    uint8_t _buffer[STM32_STORAGE_SIZE];
    volatile uint32_t _dirty_mask;
};

#endif // __AP_HAL_STM32_STORAGE_H__
