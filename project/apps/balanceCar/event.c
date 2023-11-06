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
// Created by 86189 on 2023/8/5.
//
#include "sys/MiniDebug.h"
#include "driver/hal/hal_os.h"
#include "driver/hal/hal_usb.h"
#include "log/easylogger/elog.h"
#include "driver/hal/hal_timer.h"
#include "driver/hal/hal_board.h"
#include "driver/hal/hal_dev.h"
#include "driver/hal/hal_gpio.h"
#include "kernel/FreeRTOS/FreeRTOS.h"
#include "Led.h"
#include "Buzzer.h"
#include "Key.h"
#include "wireless.h"
#include "remote.h"
#include "event.h"


#undef LOG_TAG
#define LOG_TAG "main"




static HAL_Queue g_eventQueue;


void eventSend(uint8_t type, uint32_t data, uint32_t timeout){
    RemoteEvent_t event;
    event.type = type;
    event.data = data;
    HAL_Status res = HAL_QueueSend(&g_eventQueue, &event, timeout);
    if (res != HAL_OK){
        printf("event queue send failed");
    }
}

/**
 * @brief Event task
 * @param arg
 */
void eventTask(void *arg){
    RemoteEvent_t event;
    while (1){
        HAL_Status res = HAL_QueueRecv(&g_eventQueue, &event, 1000);
        if (res == HAL_OK){
            if (event.type != EVENT_TYPE_JOYSTICK){
                printf("event type %d data %d", event.type, event.data);
            }
            switch (event.type){
                case EVENT_TYPE_KEY:
                    switch (event.data){
                        case KEY_UP:
                            printf("key up");
                            break;
                        case KEY_DOWN:
                            printf("key down");
                            break;
                        case KEY_LEFT:
                            printf("key left");
                            break;
                        case KEY_RIGHT:
                            printf("key right");
                            break;
                        case KEY_OK:
                            printf("key ok");
                            break;
                        case KEY_CANCEL:
                            printf("key cancel");
                            break;
                        default:
                            break;
                    }
                    break;

                break;

                default:
                    break;
            }
        }
    }
}

void eventInit(void){
    HAL_Status res = HAL_QueueInit(&g_eventQueue, sizeof(RemoteEvent_t),10);
    if (res != HAL_OK){
        printf("event queue init failed");
        return ;
    }

    HAL_ThreadCreate(eventTask, "eventTask", 4*128, NULL, HAL_OS_PRIORITY_HIGH, NULL);
}