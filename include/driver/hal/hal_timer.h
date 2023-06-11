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
// Created by 86189 on 2023/5/30.
//

#ifndef MINIFOC_F4_HAL_TIMER_H
#define MINIFOC_F4_HAL_TIMER_H
#include "driver/hal/hal_def.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Timer initialization
 * @return HAL_Status_ok if success, else error code
 */
HAL_Status HAL_timerInit(void);

/**
 * @brief Timer de-initialization
 */
void HAl_timerDeinit(void);

/**
 * @brief Get the millis from system
 * @return
 */
uint32_t HAL_millis(void);

/**
 * @brief Get the elapesd millis from start
 * @param start
 * @return
 */
uint32_t HAL_elapesdMillis(uint32_t start);

/**
 * @brief Delay ms
 * @param ms
 */
void HAL_msleep(uint32_t ms);

/**
 * @brief Delay s
 * @param s
 */
void HAL_sleep(uint32_t s);

void ConfigureTimeForRunTimeStats(void);

#ifdef __cplusplus
}
#endif
#endif //MINIFOC_F4_HAL_TIMER_H
