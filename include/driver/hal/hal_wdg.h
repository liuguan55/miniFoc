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
// Created by 86189 on 2023/6/5.
//

#ifndef MINIFOC_F4_HAL_WDG_H
#define MINIFOC_F4_HAL_WDG_H
#include "stdint.h"

#ifdef __cplusplus
extern "C" {
#endif


typedef struct{
    uint32_t timeout; //Interval to trigger watchdog reset, in milliseconds
    void (*callback)(void); //Function to call when watchdog reset is triggered
}wdgConfig_t;

/**
 * @brief WWDG Initialization Function
 * @param wdgConfig Pointer to a wdgConfig_t structure that contains
 */
void hal_wdgInit(wdgConfig_t *config);

/**
 * @brief WWDG feed
 */
void hal_wdgFeed(void);

/**
 * @brief WWDG start
 */
void hal_wdgStart(void);

/**
 * @brief WWDG stop
 */
void hal_wdgStop(void);


/**
 * @brief WWDG reset
 */
void hal_wdgReset(void);

#ifdef __cplusplus
}
#endif

#endif //MINIFOC_F4_HAL_WDG_H