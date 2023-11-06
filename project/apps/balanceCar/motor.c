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
// Created by 86189 on 2023/8/27.
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
#include "motor.h"
#include "main.h"
#include "pwm.h"


void motorEnable(void)
{
    board_pinmux_info_t pinmux_info;
    HAL_BoardIoctl(HAL_BIR_GET_CFG, HAL_MKDEV(HAL_DEV_MAJOR_GPIO, REMOTE_MOTOR_EN), (uint32_t)&pinmux_info);

    HAL_GpioWritePin(pinmux_info.pinmux[0].port, pinmux_info.pinmux[0].pin, GPIO_PIN_HIGH);
}

void motorDisable(void)
{
    board_pinmux_info_t pinmux_info;
    HAL_BoardIoctl(HAL_BIR_GET_CFG, HAL_MKDEV(HAL_DEV_MAJOR_GPIO, REMOTE_MOTOR_EN), (uint32_t)&pinmux_info);

    HAL_GpioWritePin(pinmux_info.pinmux[0].port, pinmux_info.pinmux[0].pin, GPIO_PIN_LOW);
}

int motor1Status(void)
{
    board_pinmux_info_t pinmux_info;
    HAL_BoardIoctl(HAL_BIR_GET_CFG, HAL_MKDEV(HAL_DEV_MAJOR_GPIO_BUTTON, REMOTE_MOTOR1_STATUS), (uint32_t)&pinmux_info);

    return HAL_GpioReadPin(pinmux_info.pinmux[0].port, pinmux_info.pinmux[0].pin);
}

int motor2Status(void)
{
    board_pinmux_info_t pinmux_info;
    HAL_BoardIoctl(HAL_BIR_GET_CFG, HAL_MKDEV(HAL_DEV_MAJOR_GPIO_BUTTON, REMOTE_MOTOR2_STATUS), (uint32_t)&pinmux_info);

    return HAL_GpioReadPin(pinmux_info.pinmux[0].port, pinmux_info.pinmux[0].pin);
}


void motorSetSpeed(int32_t motor1_output, int32_t motor2_output, int32_t motor3_output , int32_t motor4_output)
{
    if (motor1_output > 0){
        pwmSetDuty(REMOTE_PWM_ID_0, motor1_output);
        pwmSetDuty(REMOTE_PWM_ID_1, 0);
    }else if (motor1_output < 0){
        pwmSetDuty(REMOTE_PWM_ID_0, 0);
        pwmSetDuty(REMOTE_PWM_ID_1, -motor1_output);
    }else{
        pwmSetDuty(REMOTE_PWM_ID_0, 0);
        pwmSetDuty(REMOTE_PWM_ID_1, 0);
    }

    if (motor2_output > 0){
        pwmSetDuty(REMOTE_PWM_ID_2, motor2_output);
        pwmSetDuty(REMOTE_PWM_ID_3, 0);
    }else if (motor2_output < 0){
        pwmSetDuty(REMOTE_PWM_ID_2, 0);
        pwmSetDuty(REMOTE_PWM_ID_3, -motor2_output);
    }else{
        pwmSetDuty(REMOTE_PWM_ID_2, 0);
        pwmSetDuty(REMOTE_PWM_ID_3, 0);
    }

    if (motor3_output > 0){
        pwmSetDuty(REMOTE_PWM_ID_4, motor3_output);
        pwmSetDuty(REMOTE_PWM_ID_5, 0);
    }else if (motor3_output < 0){
        pwmSetDuty(REMOTE_PWM_ID_4, 0);
        pwmSetDuty(REMOTE_PWM_ID_5, -motor3_output);
    }else{
        pwmSetDuty(REMOTE_PWM_ID_4, 0);
        pwmSetDuty(REMOTE_PWM_ID_5, 0);
    }

    if (motor4_output > 0){
        pwmSetDuty(REMOTE_PWM_ID_6, motor4_output);
        pwmSetDuty(REMOTE_PWM_ID_7, 0);
    }else if (motor4_output < 0){
        pwmSetDuty(REMOTE_PWM_ID_6, 0);
        pwmSetDuty(REMOTE_PWM_ID_7, -motor4_output);
    }else{
        pwmSetDuty(REMOTE_PWM_ID_6, 0);
        pwmSetDuty(REMOTE_PWM_ID_7, 0);
    }
}