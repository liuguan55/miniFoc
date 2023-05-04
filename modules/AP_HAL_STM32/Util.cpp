#include <stdio.h>
#include "stm32f4xx_hal.h"
#include <AP_HAL/AP_HAL.h>
#include "Util.h"


using namespace STM32;

bool STM32Util::get_system_id(char *buf) {
    char chipid[40];
    snprintf(chipid, sizeof(chipid),"%x-%x-%x\n", HAL_GetUIDw0(), HAL_GetUIDw1(), HAL_GetUIDw2());
    int ret = memcmp(buf, (void *)chipid, sizeof(chipid));

    return ret != 0;
}

uint16_t STM32Util::available_memory(void){
    return 0;
}