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
// Created by 86189 on 2023/5/21.
//
#include <stdint.h>
#include "sys/sys_heap.h"
#include "console/lettershell/shell.h"
#include "common/framework/util/MiniCommon.h"

#undef LOG_TAG
#define LOG_TAG "SHELL"
/**
 * @brief system reboot
 *
 * @param argc
 * @param argv
 */
void reboot(int argc, char *argv[]) {
    UNUSED(argc);
    UNUSED(argv);

    NVIC_SystemReset();
}

SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0) | SHELL_CMD_TYPE(SHELL_TYPE_CMD_MAIN) | SHELL_CMD_DISABLE_RETURN,
reboot, reboot, reboot device);

void systemInfo(int argc, char *argv[]) {
    UNUSED(argc);
    UNUSED(argv);

    extern uint32_t system_get_heap_size(void);
    log_i("hardware version: %s, software version: %s\n", HARDWARE_VERSION, SOFTWARE_VERSION);
    log_i("heap size:%d(sram) %d(rtos)\n", system_get_heap_size(), xPortGetFreeHeapSize());
    log_i("chipid:%x-%x-%x\n", HAL_GetUIDw0(), HAL_GetUIDw1(), HAL_GetUIDw2());
    log_i("cpu freq:%d\n", HAL_RCC_GetHCLKFreq());
}

SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0) | SHELL_CMD_TYPE(SHELL_TYPE_CMD_MAIN) | SHELL_CMD_DISABLE_RETURN,
systeminfo, systemInfo, system info);



static void test_env(void) {
#ifdef USE_SPI_FLASH
    uint32_t i_boot_times = 0;
    char *c_old_boot_times, c_new_boot_times[11] = {0};

    /* get the boot count number from Env */
    c_old_boot_times = ef_get_env("boot_times");
    assert_param(c_old_boot_times);
    i_boot_times = atol(c_old_boot_times);
    /* boot count +1 */
    i_boot_times++;
    printf("The system now boot %lu times\n\r", i_boot_times);
    /* interger to string */
    sprintf(c_new_boot_times, "%ld", i_boot_times);
    /* set and store the boot count number to Env */
    ef_set_env("boot_times", c_new_boot_times);
    ef_save_env();
#endif
}

SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0) | SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC) | SHELL_CMD_DISABLE_RETURN,
                 testEnv, test_env, test env);

void RunTimeStatsTask(void) {
    const int bufferSize = 1024;
    char *infoBuffer = (char *)malloc(bufferSize);

    memset(infoBuffer, 0, bufferSize);
    /*打印任务信息*/
    vTaskList(infoBuffer);
    printf("%s", infoBuffer);

    free(infoBuffer);
}

SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0) | SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC) | SHELL_CMD_DISABLE_RETURN,
                 top, RunTimeStatsTask, run
                         times
                         stats
                         task);