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
#include "sys/MiniDebug.h"
#include "driver/hal/hal_os.h"
#include "driver/hal/hal_usb.h"
#include "log/easylogger/elog.h"
#include "driver/hal/hal_timer.h"
#include "driver/hal/hal_board.h"
#include "driver/hal/hal_dev.h"
#include "driver/hal/hal_gpio.h"
#include "Led.h"
#include "Buzzer.h"
#include "Key.h"
#include "wireless.h"
#include "remote.h"
#include "event.h"
#include "pwm.h"
#include "motor.h"
#include "controller.h"

#undef LOG_TAG
#define LOG_TAG "main"

/**
  * @brief  The application entry point.
  *
  * @retval int
  */
int main(void) {
    eventInit();
    keyInit();
    ledTaskStart();
    wirelessInit();
    pwmInit();
    controllerInit();

    uint8_t recvData[32] = {0};
    while(1){
        if(wirelessRecv(recvData, sizeof (recvData)) > 0){
            remote_parsePacket(recvData,sizeof (recvData));
            ledToggle();
        }
        HAL_msleep(10);
    }

    return 0;
}
