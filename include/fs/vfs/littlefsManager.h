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
// Created by 86189 on 2023/5/3.
//

#ifndef MINIFOC_F4_LITTLEFSMANAGER_H
#define MINIFOC_F4_LITTLEFSMANAGER_H
#include "fs/littlefs/lfs.h"
#include "kernel/FreeRTOS/CMSIS_RTOS_V2/cmsis_os2.h"
#include "driver/component/flash/sfud/sfud.h"

enum{
    LFS_SYSTEM_PARTITION = 0,
    LFS_DATA_PARTITION ,
    LFS_LOG_PARTITION ,
    LFS_USER_PARTITION,
    lfs_partition_NR,
};

typedef struct {
    uint32_t addr;
    uint32_t size;
} lfs_partition_t;

typedef struct{
    sfud_flash *flashHandle;
    osMutexId_t mutex;
    lfs_partition_t partition;
}lfsCfg_t;

void lfsManager_init();
lfs_t *lfsManager_getLfs(int lfs_partition_index);

#endif //MINIFOC_F4_LITTLEFSMANAGER_H
