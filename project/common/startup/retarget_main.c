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
// Created by 86189 on 2023/5/24.
//

#include <stdio.h>
#include "kernel/FreeRTOS/FreeRTOS.h"
#include "driver/hal/hal_os.h"
#include "common/ProjectConf.h"
#include "common/framework/platform_init.h"

extern int __real_main(void);

static HAL_Thread g_main_thread;

static void main_task(void *arg)
{
    platform_init();
    __real_main();
    HAL_ThreadDelete(&g_main_thread);
}

void main_task_start(void)
{
    HAL_ThreadInitilize();

    if (HAL_ThreadCreate(main_task, "main", PRJCONF_MAIN_THREAD_STACK_SIZE, NULL,PRJCONF_MAIN_THREAD_PRIO, &g_main_thread) != HAL_STATUS_OK) {
        printf("[ERR] create main task failed\n");
    }

    HAL_ThreadStartScheduler();

    while (1) {
        printf("error\n");
    }
}