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

#include "driver/hal/hal_wdg.h"
#include "stm32f4xx_hal.h"
#include "sys/MiniDebug.h"

typedef struct {
    WWDG_HandleTypeDef hwdg;
    void (*wdgCallback)(void);
    uint32_t timeout ;
    uint8_t wdgEnable;
}WdgDev_t;

static WdgDev_t gWdgDev;

void WWDG_IRQHandler(void)
{
    /* USER CODE BEGIN WWDG_IRQn 0 */

    /* USER CODE END WWDG_IRQn 0 */
    HAL_WWDG_IRQHandler(&gWdgDev.hwdg);
    /* USER CODE BEGIN WWDG_IRQn 1 */

    /* USER CODE END WWDG_IRQn 1 */
}

void HAL_WWDG_EarlyWakeupCallback(WWDG_HandleTypeDef *hwwdg)
{
    if(gWdgDev.wdgCallback != NULL){
        gWdgDev.wdgCallback();
    }

    if (gWdgDev.wdgEnable == 1){
        HAL_WWDG_Refresh(hwwdg);
    }
}

void hal_wdgInit(wdgConfig_t *wdgConfig){
    gWdgDev.timeout = wdgConfig->timeout;
    gWdgDev.wdgCallback = wdgConfig->callback;
    gWdgDev.wdgEnable = 1;
    gWdgDev.hwdg.Instance = WWDG;
    gWdgDev.hwdg.Init.Prescaler = WWDG_PRESCALER_8;
    gWdgDev.hwdg.Init.Window = 64;
    gWdgDev.hwdg.Init.Counter = 127;
    gWdgDev.hwdg.Init.EWIMode = WWDG_EWI_ENABLE;
    if (HAL_WWDG_Init(&gWdgDev.hwdg) != HAL_OK)
    {
        assert(NULL);
    }

    __HAL_RCC_WWDG_CLK_ENABLE();

    /* WWDG interrupt Init */
    HAL_NVIC_SetPriority(WWDG_IRQn, 5, 0);
    HAL_NVIC_EnableIRQ(WWDG_IRQn);
}


void hal_wdgFeed(void){
    HAL_WWDG_Refresh(&gWdgDev.hwdg);
}


void hal_wdgStart(void){
}


void hal_wdgStop(void){
}

void hal_wdgReset(void){
    gWdgDev.wdgEnable = 0;
}
