//
// Created by 86189 on 2023/4/28.
//

#ifndef MINIFOC_F4_AP_HAL_BOARD_H
#define MINIFOC_F4_AP_HAL_BOARD_H
#include "AP_HAL.h"
#include "AP_HAL_Empty/AP_HAL_Empty.h"
#include "AP_HAL_STM32/AP_HAL_STM32.h"

#define hal HAL_STM32::getInstance()

#endif //MINIFOC_F4_AP_HAL_BOARD_H
