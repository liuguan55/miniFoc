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
// Created by 86189 on 2023/5/6.
//
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "fs/FatFs/App/fatfs.h"
#include "fs/vfs/vfsFatDriver.h"
#include "fs/vfs/vfs.h"


vfsRes_t fatfsOpen(void *file, const char *path, vfsMode_t flags){
    FRESULT res = FR_OK;

    vfsFile_t *vfsFile = (vfsFile_t *)file;
    vfsFile->file = malloc(sizeof(FIL));

    uint8_t mode = 0;
    if(flags & VFS_MODE_READ){
        mode |= FA_READ;
    }
    if(flags & VFS_MODE_WRITE){
        mode |= FA_WRITE;
    }
    if(flags & VFS_MODE_APPEND){
        mode |= FA_OPEN_APPEND;
    }
    if(flags & VFS_MODE_CREATE){
        mode |= FA_CREATE_ALWAYS;
    }
    if(flags & VFS_MODE_TRUNC){
        mode |= FA_CREATE_ALWAYS;
    }

    FIL *fp = vfsFile->file;
    res = f_open(fp, path, mode);
    if(res != FR_OK){
        free(vfsFile->file);
        return VFS_RES_ERR;
    }

    return VFS_RES_OK;
}

vfsRes_t fatfsClose(void *file){
    FRESULT res = FR_OK;

    vfsFile_t *vfsFile = (vfsFile_t *)file;
    FIL *fp = (FIL *)vfsFile->file;
    res = f_close(fp);
    if(res != FR_OK){
        return VFS_RES_ERR;
    }

    free(fp);
    fp = NULL;

    return VFS_RES_OK;
}

vfsRes_t fatfsRead(void *file, void *buf, uint32_t len, uint32_t *readLen){
    FRESULT res = FR_OK;

    vfsFile_t *vfsFile = (vfsFile_t *)file;
    FIL *fp = (FIL *)vfsFile->file;
    res = f_read(fp, buf, len, (UINT *) readLen);
    if(res != FR_OK){
        return VFS_RES_ERR;
    }


    return VFS_RES_OK;
}

vfsRes_t fatfsWrite(void *file, const void *buf, uint32_t len, uint32_t *writeLen){
    FRESULT res = FR_OK;

    vfsFile_t *vfsFile = (vfsFile_t *)file;
    FIL *fp = (FIL *)vfsFile->file;
    res = f_write(fp, buf, len, (UINT *) writeLen);
    if(res != FR_OK){
        return VFS_RES_ERR;
    }

    return VFS_RES_OK;
}

vfsRes_t fatfsSeek(void *file, int32_t offset, vfsSeekMode_t whence){
    FRESULT res = FR_OK;

    vfsFile_t *vfsFile = (vfsFile_t *)file;
    FIL *fp = (FIL *)vfsFile->file;
    uint32_t pos = 0;
    switch (whence){
        case VFS_SEEK_SET:
            break;
        case VFS_SEEK_CUR:
            pos = f_tell(fp);
            break;
        case VFS_SEEK_END:
            pos = f_size(fp);
            break;
        default:
            return VFS_RES_ERR;
    }

    res = f_lseek(fp, pos + offset);
    if(res != FR_OK){
        return VFS_RES_ERR;
    }

    return VFS_RES_OK;
}

vfsRes_t fatfsTell(void *file, uint32_t *pos){
    vfsFile_t *vfsFile = (vfsFile_t *)file;
    FIL *fp = (FIL *)vfsFile->file;
    *pos = f_tell(fp);

    return VFS_RES_OK;
}

vfsRes_t fatfsTruncate(void *file, uint32_t length){
    FRESULT res = FR_OK;

    vfsFile_t *vfsFile = (vfsFile_t *)file;
    FIL *fp = (FIL *)vfsFile->file;
    res = f_truncate(fp);
    if(res != FR_OK){
        return VFS_RES_ERR;
    }

    return VFS_RES_OK;
}

vfsRes_t fatfsSync(void *file){
    FRESULT res = FR_OK;

    vfsFile_t *vfsFile = (vfsFile_t *)file;
    FIL *fp = (FIL *)vfsFile->file;
    res = f_sync(fp);
    if(res != FR_OK){
        return VFS_RES_ERR;
    }

    return VFS_RES_OK;
}

vfsRes_t fatfsSize(void *file, uint32_t *size){
    vfsFile_t *vfsFile = (vfsFile_t *)file;
    FIL *fp = (FIL *)vfsFile->file;
    *size = f_size(fp);

    return VFS_RES_OK;
}

vfsRes_t fatfsRename(void *dir ,const char *oldName, const char *newName){
    FRESULT res = FR_OK;

    UNUSED(dir);

    res = f_rename(oldName, newName);
    if(res != FR_OK){
        return VFS_RES_ERR;
    }

    return VFS_RES_OK;
}

vfsRes_t fatfsStat(void *file,const char *path, vfsFileInfo_t *fileInfo){
    FRESULT res = FR_OK;
    FILINFO fno;

    UNUSED(file);

    res = f_stat(path, &fno);
    if(res != FR_OK){
        return VFS_RES_ERR;
    }

    fileInfo->size = fno.fsize;
    fileInfo->isDir = fno.fattrib & AM_DIR;
    fileInfo->modifiedTime = fno.fdate << 16 | fno.ftime;
    strncpy(fileInfo->name, fno.fname, VFS_NAME_MAX);

    return VFS_RES_OK;
}

vfsRes_t fatfsUnlink(void *dir, const char *path){
    FRESULT res = FR_OK;

    UNUSED(dir);

    res = f_unlink(path);
    if(res != FR_OK){
        return VFS_RES_ERR;
    }

    return VFS_RES_OK;
}

vfsRes_t fatfsMkdir(void *dir, const char *path){
    FRESULT res = FR_OK;

    UNUSED(dir);
    res = f_mkdir(path);
    if(res != FR_OK){
        return VFS_RES_ERR;
    }

    return VFS_RES_OK;
}

vfsRes_t fatfsRemoveDir(void *dir, const char *path){
    FRESULT res = FR_OK;

    UNUSED(dir);
    res = f_unlink(path);
    if(res != FR_OK){
        return VFS_RES_ERR;
    }

    return VFS_RES_OK;
}

vfsRes_t fatfsOpenDir(void *dir, const char *path){
    FRESULT res = FR_OK;
    vfsFile_t *vfsDir = (vfsFile_t *)dir;
    vfsDir->file = malloc(sizeof(DIR));

    res = f_opendir(vfsDir->file, path);
    if(res != FR_OK){
        free(vfsDir->file);
        return VFS_RES_ERR;
    }

    return VFS_RES_OK;
}

vfsRes_t fatfsCloseDir(void *dir){
    FRESULT res = FR_OK;

    vfsFile_t *vfsDir = (vfsFile_t *)dir;
    res = f_closedir(vfsDir->file);
    if(res != FR_OK){
        return VFS_RES_ERR;
    }

    free(vfsDir->file);
    vfsDir->file = NULL;

    return VFS_RES_OK;
}

vfsRes_t fatfsReadDir(void *dir, vfsFileInfo_t *fileInfo){
    FRESULT res = FR_OK;
    FILINFO fno;

    vfsFile_t *vfsDir = (vfsFile_t *)dir;
    res = f_readdir(vfsDir->file, &fno);
    if(res != FR_OK){
        return VFS_RES_ERR;
    }

    fileInfo->size = fno.fsize;
    fileInfo->isDir = fno.fattrib & AM_DIR;
    fileInfo->modifiedTime = fno.fdate << 16 | fno.ftime;
    strncpy(fileInfo->name, fno.fname, VFS_NAME_MAX);

    return VFS_RES_OK;
}

vfsRes_t  fatfsChdir(void *dir, const char *path){
    vfsRes_t res = VFS_RES_OK;

    UNUSED(dir);
    FRESULT ret = f_chdir(path);
    if (ret != FR_OK) {
        res = VFS_RES_ERR;
    }

    return res ;
}

vfsRes_t fatfsGetcwd(void *dir, char *buffer, size_t bufferSize){
    vfsRes_t res = VFS_RES_OK;

    UNUSED(dir);
    FRESULT ret = f_getcwd(buffer, bufferSize);
    if (ret != FR_OK) {
        res = VFS_RES_ERR;
    }

    return res ;
}

static vfsDrv_t vfsFatfsDriver = {
        .open = fatfsOpen,
        .close = fatfsClose,
        .read = fatfsRead,
        .write = fatfsWrite,
        .tell = fatfsTell,
        .size = fatfsSize,
        .truncate = fatfsTruncate,
        .seek = fatfsSeek,
        .rename = fatfsRename,
        .stat = fatfsStat,
        .sync = fatfsSync,
        .unlink = fatfsUnlink,
        .mkdir = fatfsMkdir,
        .rmdir = fatfsRemoveDir,
        .opendir = fatfsOpenDir,
        .closedir = fatfsCloseDir,
        .readdir = fatfsReadDir,
        .chdir = fatfsChdir,
        .getcwd = fatfsGetcwd,
};

vfsDrv_t * vfsFatFsDriverGet() {
    return &vfsFatfsDriver;
}