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
// Created by 86189 on 2023/8/6.
//
#include "sys/MiniDebug.h"
#include "driver/hal/hal_os.h"
#include "driver/hal/hal_usb.h"
#include "log/easylogger/elog.h"
#include "driver/hal/hal_timer.h"
#include "driver/hal/hal_board.h"
#include "driver/hal/hal_dev.h"
#include "driver/hal/hal_gpio.h"
#include "driver/hal/hal_pwm.h"
#include "driver/hal/hal_os.h"
#include "pwm.h"
#include "main.h"



void pwmInit(void)
{
    HAL_pwmInit(HAL_PWM_ID_1);
    HAL_pwmInit(HAL_PWM_ID_2);
    HAL_pwmInit(HAL_PWM_ID_3);

    pwmSetDuty(REMOTE_PWM_ID_0, 0);
    pwmSetDuty(REMOTE_PWM_ID_1, 0);
    pwmSetDuty(REMOTE_PWM_ID_2, 0);
    pwmSetDuty(REMOTE_PWM_ID_3, 0);
    pwmSetDuty(REMOTE_PWM_ID_4, 0);
    pwmSetDuty(REMOTE_PWM_ID_5, 0);
    pwmSetDuty(REMOTE_PWM_ID_6, 0);
    pwmSetDuty(REMOTE_PWM_ID_7, 0);
}


void pwmSetDuty(Remote_PWM_ID pwmId,  uint16_t duty)
{
    switch (pwmId) {
        case REMOTE_PWM_ID_0:
            HAL_pwmOutput(HAL_PWM_ID_3, HAL_PWM_CH2, duty);
            break;
        case REMOTE_PWM_ID_1:
            HAL_pwmOutput(HAL_PWM_ID_3, HAL_PWM_CH3, duty);
            break;
        case REMOTE_PWM_ID_2:
            HAL_pwmOutput(HAL_PWM_ID_3, HAL_PWM_CH4, duty);
            break;
        case REMOTE_PWM_ID_3:
            HAL_pwmOutput(HAL_PWM_ID_2, HAL_PWM_CH3, duty);
            break;
        case REMOTE_PWM_ID_4:
            HAL_pwmOutput(HAL_PWM_ID_2, HAL_PWM_CH4, duty);
            break;
        case REMOTE_PWM_ID_5:
            HAL_pwmOutput(HAL_PWM_ID_1, HAL_PWM_CH4, duty);
            break;
        case REMOTE_PWM_ID_6:
            HAL_pwmOutput(HAL_PWM_ID_1, HAL_PWM_CH1, duty);
            break;
        case REMOTE_PWM_ID_7:
            HAL_pwmOutput(HAL_PWM_ID_1, HAL_PWM_CH3, duty);
            break;
        default:
            break;
    }
}