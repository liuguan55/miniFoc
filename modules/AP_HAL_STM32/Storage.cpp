#include <stdio.h>
#include <string.h>
#include "Storage.h"

using namespace STM32;

STM32Storage::STM32Storage()
{}

void STM32Storage::init(void*)
{
}

void STM32Storage::read_block(void* dst, uint16_t src, size_t n) {
    if (src >= sizeof(_buffer)-(n-1)) {
        return;
    }
    _storage_open();
    memcpy(dst, &_buffer[src], n);
}

void STM32Storage::write_block(uint16_t loc, const void* src, size_t n)
{
    if (loc >= sizeof(_buffer)-(n-1)) {
        return;
    }
    if (memcmp(src, &_buffer[loc], n) != 0) {
        _storage_open();
        memcpy(&_buffer[loc], src, n);
        _mark_dirty(loc, n);
    }
}

void STM32Storage::_timer_tick(void) {
    if (!_initialised || _dirty_mask == 0) {
        return;
    }

}

void STM32Storage::_storage_create(void) {

}

void STM32Storage::_storage_open(void) {
    if (_initialised) {
        return;
    }

    _initialised = true;
}

void STM32Storage::_mark_dirty(uint16_t loc, uint16_t length) {
    uint16_t end = loc + length;
    while (loc < end) {
        uint8_t line = (loc >> STM32_STORAGE_LINE_SHIFT);
        _dirty_mask |= 1 << line;
        loc += STM32_STORAGE_LINE_SHIFT;
    }
}

