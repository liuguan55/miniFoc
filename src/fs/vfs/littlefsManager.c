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
#include <stdlib.h>
#include <stdint.h>
#include "fs/vfs/littlefsManager.h"
#include "fs/littlefs/lfs.h"
#include "driver/component/flash/sfud/sfud.h"
#include "kernel/FreeRTOS/CMSIS_RTOS_V2/cmsis_os2.h"

#define FLASH_START_ADDR (128 * 1024)

#define LFS_SYSTEM_PARTITION_SIZE (512 * 1024)
#define LFS_DATA_PARTITION_SIZE (512 * 1024)
#define LFS_LOG_PARTITION_SIZE (512 * 1024)
#define LFS_USER_PARTITION_SIZE (512 * 1024 - FLASH_START_ADDR)

#define LFS_SYSTEM_PARTITION_ADDR (FLASH_START_ADDR)
#define LFS_DATA_PARTITION_ADDR (LFS_SYSTEM_PARTITION_ADDR + LFS_SYSTEM_PARTITION_SIZE)
#define LFS_LOG_PARTITION_ADDR (LFS_DATA_PARTITION_ADDR + LFS_DATA_PARTITION_SIZE)
#define LFS_USER_PARTITION_ADDR (LFS_LOG_PARTITION_ADDR + LFS_LOG_PARTITION_SIZE)



static const lfs_partition_t lfs_partition[lfs_partition_NR] = {
        {LFS_SYSTEM_PARTITION_ADDR, LFS_SYSTEM_PARTITION_SIZE},
        {LFS_DATA_PARTITION_ADDR, LFS_DATA_PARTITION_SIZE},
        {LFS_LOG_PARTITION_ADDR, LFS_LOG_PARTITION_SIZE},
        {LFS_USER_PARTITION_ADDR, LFS_USER_PARTITION_SIZE},
};


typedef struct {
    lfsCfg_t cfg[lfs_partition_NR];
    lfs_t lfs[lfs_partition_NR];
    struct lfs_config lfs_cfg[lfs_partition_NR];
}lfsManager_t;

static lfsManager_t lfsManager = {0};

int user_provided_block_device_read(const struct lfs_config *c, lfs_block_t block,
                                    lfs_off_t off, void *buffer, lfs_size_t size) {
    lfsCfg_t *lfsCfg = (lfsCfg_t *)c->context;
    sfud_flash *flashHandle = lfsCfg->flashHandle;
    uint32_t addr = lfsCfg->partition.addr + block * c->block_size + off;
//    printf("read addr:0x%lx size %lu\r\n", lfsCfg->partition.addr,size);
    //使用SFUD开源库提供的API实现Flash读取
    sfud_read(flashHandle, addr, size, (uint8_t *)buffer);

    return 0;
}

int user_provided_block_device_prog(const struct lfs_config *c, lfs_block_t block,
                                    lfs_off_t off, const void *buffer, lfs_size_t size) {
    lfsCfg_t *lfsCfg = (lfsCfg_t *)c->context;
    sfud_flash *flashHandle = lfsCfg->flashHandle;
    uint32_t addr = lfsCfg->partition.addr + block * c->block_size + off;
    //使用SFUD开源库提供的API实现Flash写入
    sfud_write(flashHandle, addr, size, (uint8_t *)buffer);

    return 0;
}

int user_provided_block_device_erase(const struct lfs_config *c, lfs_block_t block) {
    lfsCfg_t *lfsCfg = (lfsCfg_t *)c->context;
    sfud_flash *flashHandle = lfsCfg->flashHandle;

    uint32_t addr = lfsCfg->partition.addr + block * c->block_size;
//    printf("read addr:0x%lx size\r\n", lfsCfg->partition.addr);
    //使用SFUD开源库提供的API实现Flash擦除
    sfud_erase(flashHandle, addr, c->block_size);

    return 0;
}

int user_provided_block_device_sync(const struct lfs_config *c) {
    return 0;
}


void lfs_config_init(lfsManager_t *manager, int lfs_partition_index){
    manager->lfs_cfg[lfs_partition_index].context = &manager->cfg[lfs_partition_index];
    manager->lfs_cfg[lfs_partition_index].read = user_provided_block_device_read;
    manager->lfs_cfg[lfs_partition_index].prog = user_provided_block_device_prog;
    manager->lfs_cfg[lfs_partition_index].erase = user_provided_block_device_erase;
    manager->lfs_cfg[lfs_partition_index].sync = user_provided_block_device_sync;
    manager->lfs_cfg[lfs_partition_index].read_size = 256;
    manager->lfs_cfg[lfs_partition_index].prog_size = 256;
    manager->lfs_cfg[lfs_partition_index].block_size = 4096;
    manager->lfs_cfg[lfs_partition_index].block_count = lfs_partition[lfs_partition_index].size / manager->lfs_cfg[lfs_partition_index].block_size;
    manager->lfs_cfg[lfs_partition_index].cache_size = 1024;
    manager->lfs_cfg[lfs_partition_index].lookahead_size = 128;
    manager->lfs_cfg[lfs_partition_index].block_cycles = 500;
}

void lfsManager_init() {
    if (sfud_init() != SFUD_SUCCESS){
        printf("SFUD init failed!\n");
        return ;
    }
    memset(&lfsManager, 0, sizeof(lfsManager_t));

    sfud_flash *flashHandle = sfud_get_device(SFUD_W25Q16X_DEVICE_INDEX);
    if (flashHandle == NULL){
        printf("SFUD get device failed!\n");
        return ;
    }

    for (int i = 0; i < lfs_partition_NR; ++i) {
        lfsCfg_t *cfg = &lfsManager.cfg[i];
        cfg->flashHandle = flashHandle;
        cfg->partition = lfs_partition[i];
        cfg->mutex = osMutexNew(NULL);
        lfs_config_init(&lfsManager, i);
        int err =lfs_mount(&lfsManager.lfs[i], &lfsManager.lfs_cfg[i]);
        if (err) {
            lfs_format(&lfsManager.lfs[i], &lfsManager.lfs_cfg[i]);
            if (lfs_mount(&lfsManager.lfs[i], &lfsManager.lfs_cfg[i])){
                printf("littlefs mount failed\n");
            }
        }
    }

//    for (int i = 0; i < lfs_partition_NR; ++i) {
//        lfs_t lfs = lfsManager.lfs[i];
//        int err = 0;
//        lfs_file_t file;
//
//        // read current count
//        uint32_t boot_count = 0;
//        err = lfs_file_open(&lfs, &file, "boot_count", LFS_O_RDWR|LFS_O_CREAT);
//        if (err != 0){
//            printf("open file failed ,err:%d\n",err);
//            return ;
//        }
//        lfs_file_read(&lfs, &file, &boot_count, sizeof(boot_count));
//
//        // update boot count
//        boot_count += 1;
//        lfs_file_rewind(&lfs, &file);
//        lfs_file_write(&lfs, &file, &boot_count, sizeof(boot_count));
//        // remember the storage is not updated until the file is closed successfully
//        lfs_file_close(&lfs, &file);
//        // release any resources we were using
//        lfs_unmount(&lfs);
//
//        // print the boot count
//        printf("boot_count: %lu\n", boot_count);
//    }

}

lfs_t *lfsManager_getLfs(int lfs_partition_index){
    return &lfsManager.lfs[lfs_partition_index];
}