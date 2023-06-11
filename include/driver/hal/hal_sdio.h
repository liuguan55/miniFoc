// /* Copyright (C) 2023 dengzhijun. All rights reserved.
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

//
// Created by 86189 on 2023/6/4.
//

#ifndef MINIFOC_F4_HAL_SDIO_H
#define MINIFOC_F4_HAL_SDIO_H
#include "driver/hal/hal_def.h"

#ifdef __cplusplus
extern "C" {
#endif

enum {
    HAL_SDC_1 = 0,
    HAL_SDC_NR
};
typedef uint8_t HAL_SDC_ID;

/**
 * @brief Init sdcard controller
 * @param sdcId ID of specified sdcard controller
 * @return HAL_STATUS_OK if success, else fail
 */
HAL_Status hal_sdcInit(HAL_SDC_ID sdcId);

/**
 * @brief Deinit sdcard controller
 * @param sdcId id of specified sdcard controller
 * @return HAL_STATUS_OK if success, else fail
 */
HAL_Status hal_sdcDeinit(HAL_SDC_ID sdcId);

#ifdef __cplusplus
}
#endif

#endif //MINIFOC_F4_HAL_SDIO_H
