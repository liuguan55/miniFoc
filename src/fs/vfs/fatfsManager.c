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
// Created by 86189 on 2023/5/7.
//
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "fs/vfs/fatfsManager.h"
#include "fs/FatFs/App/fatfs.h"

static FATFS SDFatFS;    /* File system object for SD logical drive */
static uint8_t sdInitalize = 0;

int fatfsManager_init(void)
{
    MX_FATFS_Init();

    const char *SDPath = fatfs_get_sd_path();
    int retSD = f_mount(&SDFatFS, SDPath, 1);
    if (retSD != FR_OK){
        if (retSD == FR_NO_FILESYSTEM){
            char work[_MAX_SS];
            retSD = f_mkfs(SDPath, 0 ,_MAX_SS , work, sizeof(work));
            if (retSD == FR_OK)	{
                printf("fs format success\n");
                retSD = f_mount(NULL, SDPath, 1);
                retSD = f_mount(&SDFatFS, SDPath, 1);
            }else {
                printf("format failed ,err:%d\n",retSD);
                return retSD;
            }
        }else {
            printf("f_mount failed ,err:%d\n",retSD);
            return retSD;
        }
    }else {
        sdInitalize = 1;
        printf("mount fatfs success\n");
    }

    return 0;
}

FATFS* fatfsManager_getFatfs(void)
{
    return &SDFatFS;
}

//
//int MiniFatfs_writeLog(const char *filename, const char *data, int len)
//{
//    if (!sdInitalize) {
//        return 0;
//    }
//
//    FIL file ;
//    int writeLen = 0;
//
//    int ret = f_open(&file, filename, FA_OPEN_APPEND | FA_WRITE);
//    if (ret != FR_OK)
//    {
//        printf("open %s failed,ret %d\n",filename, ret);
//        return 0;
//    }
//
//    ret = f_write(&file, data, len, (UINT *)&writeLen);
//
//    f_close(&file);
//
//    return writeLen;
//}
