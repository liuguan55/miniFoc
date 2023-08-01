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
 * @brief print all envs
 *
 * @param argc
 * @param argv
 */
void printEnvs(int argc, char *argv[]) {
    UNUSED(argc);
    UNUSED(argv);
#ifdef USE_SPI_FLASH
    ef_print_env();
#endif
}

SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0) | SHELL_CMD_TYPE(SHELL_TYPE_CMD_MAIN) | SHELL_CMD_DISABLE_RETURN,
                 envs, printEnvs, print envs);

/**
 * @brief Set the Env
 *
 * @param argc
 * @param argv
 */
void setEnv(int argc, char *argv[]) {
    if (argc < 3) {
        log_e("error:param less than 3\n");
        return;
    }

    char *key = argv[1];
    char *value = argv[2];

#ifdef USE_SPI_FLASH
    ef_set_env(key, value);
#endif
}

SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0) | SHELL_CMD_TYPE(SHELL_TYPE_CMD_MAIN) | SHELL_CMD_DISABLE_RETURN,
                 setenv, setEnv, set envs);

/**
 * @brief Get the Env
 *
 * @param argc
 * @param argv
 */
void getEnv(int argc, char *argv[]) {
#ifdef USE_SPI_FLASH
    if (argc < 2) {
//        ef_print_env();
        return;
    }

    char *key = argv[1];


    char *value = ef_get_env(key);

    log_i("%s=%s\n", key, value);
#endif
}

SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0) | SHELL_CMD_TYPE(SHELL_TYPE_CMD_MAIN) | SHELL_CMD_DISABLE_RETURN,
                 getenv, getEnv, get envs);

void resetEnv(int argc, char *argv[]) {
#ifdef USE_SPI_FLASH
    if (argc < 2) {
//        ef_print_env();
        return;
    }

    UNUSED(argv);

    ef_env_set_default();

    log_i("reset envs\n");
#endif
}

SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0) | SHELL_CMD_TYPE(SHELL_TYPE_CMD_MAIN) | SHELL_CMD_DISABLE_RETURN,
                 resetenv, resetEnv, reset envs);
