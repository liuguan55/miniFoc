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
#include <stdint.h>
#include "compiler.h"
#include "fs/vfs/vfsLittleFsDriver.h"
#include "fs/vfs/vfs.h"
#include "fs/littlefs/lfs.h"
#include "MiniAssert.h"
#include "kernel/FreeRTOS/CMSIS_RTOS_V2/cmsis_os2.h"
#include "fs/vfs/littlefsManager.h"

osStatus_t ltfsLock(vfsFile_t *vfsFile) {
    lfs_t *lfs = (lfs_t *) vfsFile->handle;
    lfsCfg_t *cfg = (lfsCfg_t *)lfs->cfg->context;
    osMutexId_t mutex = cfg->mutex;

    return osMutexAcquire(mutex, osWaitForever);
}

void ltfsUnlock(vfsFile_t *vfsFile){
    lfs_t *lfs = (lfs_t *) vfsFile->handle;
    lfsCfg_t *cfg = (lfsCfg_t *)lfs->cfg->context;
    osMutexId_t mutex = cfg->mutex;

    osMutexRelease(mutex);
}

vfsRes_t ltfsOpen(void *file, const char *path, vfsMode_t flags) {
    MINI_ASSERT_MSG(file != NULL, "file is NULL");
    vfsFile_t *vfsFile = (vfsFile_t *) file;
    vfsFile->file = malloc(sizeof(lfs_file_t));
    if (vfsFile->file == NULL) {
        return VFS_RES_ERR;
    }

    memset(vfsFile->file, 0, sizeof(lfs_file_t));

    MINI_ASSERT_MSG(vfsFile->handle != NULL, "handle is NULL");

    vfsFile->path = path;

    int lfsFlag = 0;

    if (flags & VFS_MODE_READ) {
        lfsFlag |= LFS_O_RDONLY;
    }
    if (flags & VFS_MODE_WRITE) {
        lfsFlag |= LFS_O_WRONLY;
    }
    if (flags & VFS_MODE_APPEND) {
        lfsFlag |= LFS_O_APPEND;
    }
    if (flags & VFS_MODE_CREATE) {
        lfsFlag |= LFS_O_CREAT;
    }
    if (flags & VFS_MODE_TRUNC) {
        lfsFlag |= LFS_O_TRUNC;
    }

    ltfsLock(vfsFile);
    int ret = lfs_file_open((lfs_t *) vfsFile->handle, vfsFile->file,  path, lfsFlag);
    ltfsUnlock(vfsFile);
    if (ret != 0) {
        return VFS_RES_ERR;
    }

    return VFS_RES_OK;
}

vfsRes_t ltfsClose(void *file) {
    MINI_ASSERT_MSG(file != NULL, "file is NULL");
    vfsFile_t *vfsFile = (vfsFile_t *) file;
    MINI_ASSERT_MSG(vfsFile->file != NULL, "file->file is NULL");

    ltfsLock(vfsFile);
    int ret = lfs_file_close((lfs_t *) vfsFile->handle, vfsFile->file);
    ltfsUnlock(vfsFile);
    if (ret != 0) {
        return VFS_RES_ERR;
    }
    free(vfsFile->file);
    vfsFile->file = NULL;
    return VFS_RES_OK;
}

vfsRes_t ltfsRead(void *file, void *buf, uint32_t len, uint32_t *readLen) {
    MINI_ASSERT_MSG(file != NULL, "file is NULL");
    MINI_ASSERT_MSG(buf != NULL, "buf is NULL");
    MINI_ASSERT_MSG(len > 0, "len is 0");
    MINI_ASSERT_MSG(readLen != NULL, "readLen is NULL");

    vfsFile_t *vfsFile = (vfsFile_t *) file;
    MINI_ASSERT_MSG(vfsFile->file != NULL, "file->file is NULL");
    MINI_ASSERT_MSG(vfsFile->handle != NULL, "handle is 0");

    ltfsLock(vfsFile);
    int ret = lfs_file_read((lfs_t *) vfsFile->handle, vfsFile->file, buf, len);
    ltfsUnlock(vfsFile);
    if (ret < 0) {
        return VFS_RES_ERR;
    }

    if (readLen != NULL) {
        *readLen = ret;
    }

    return VFS_RES_OK;
}

vfsRes_t ltfsWrite(void *file, const void *buf, uint32_t len, uint32_t *writeLen) {
    MINI_ASSERT_MSG(file != NULL, "file is NULL");
    MINI_ASSERT_MSG(buf != NULL, "buf is NULL");
    MINI_ASSERT_MSG(len > 0, "len is 0");
    MINI_ASSERT_MSG(writeLen != NULL, "writeLen is NULL");

    vfsFile_t *vfsFile = (vfsFile_t *) file;
    MINI_ASSERT_MSG(vfsFile->file != NULL, "file->file is NULL");
    MINI_ASSERT_MSG(vfsFile->handle != NULL, "handle is 0");

    ltfsLock(vfsFile);
    int ret = lfs_file_write((lfs_t *) vfsFile->handle, vfsFile->file, buf, len);
    ltfsUnlock(vfsFile);
    if (ret < 0) {
        return VFS_RES_ERR;
    }

    if (writeLen != NULL) {
        *writeLen = ret;
    }

    return VFS_RES_OK;
}

vfsRes_t ltfsSeek(void *file, int32_t offset, vfsSeekMode_t whence) {
    MINI_ASSERT_MSG(file != NULL, "file is NULL");
    vfsFile_t *vfsFile = (vfsFile_t *) file;
    MINI_ASSERT_MSG(vfsFile->file != NULL, "file->file is NULL");
    MINI_ASSERT_MSG(vfsFile->handle != NULL, "handle is 0");

    ltfsLock(vfsFile);
    int ret = lfs_file_seek((lfs_t *) vfsFile->handle, vfsFile->file, offset, whence);
    ltfsUnlock(vfsFile);
    if (ret < 0) {
        return VFS_RES_ERR;
    }
    return VFS_RES_OK;
}

vfsRes_t ltfsTell(void *file, uint32_t *offset) {
    MINI_ASSERT_MSG(file != NULL, "file is NULL");
    vfsFile_t *vfsFile = (vfsFile_t *) file;
    MINI_ASSERT_MSG(vfsFile->file != NULL, "file->file is NULL");
    MINI_ASSERT_MSG(vfsFile->handle != NULL, "handle is 0");

    ltfsLock(vfsFile);
    int ret = lfs_file_tell((lfs_t *) vfsFile->handle, vfsFile->file);
    ltfsUnlock(vfsFile);
    if (ret < 0) {
        return VFS_RES_ERR;
    }

    if (offset != NULL) {
        *offset = ret;
    }

    return VFS_RES_OK;
}

vfsRes_t ltfsTruncate(void *file, uint32_t size) {
    MINI_ASSERT_MSG(file != NULL, "file is NULL");
    vfsFile_t *vfsFile = (vfsFile_t *) file;
    MINI_ASSERT_MSG(vfsFile->file != NULL, "file->file is NULL");
    MINI_ASSERT_MSG(vfsFile->handle != NULL, "handle is 0");

    ltfsLock(vfsFile);
    int ret = lfs_file_truncate((lfs_t *) vfsFile->handle, vfsFile->file, size);
    ltfsUnlock(vfsFile);
    if (ret < 0) {
        return VFS_RES_ERR;
    }
    return VFS_RES_OK;
}

vfsRes_t ltfsSync(void *file) {
    MINI_ASSERT_MSG(file != NULL, "file is NULL");
    vfsFile_t *vfsFile = (vfsFile_t *) file;
    MINI_ASSERT_MSG(vfsFile->file != NULL, "file->file is NULL");
    MINI_ASSERT_MSG(vfsFile->handle != NULL, "handle is 0");

    ltfsLock(vfsFile);
    int ret = lfs_file_sync((lfs_t *) vfsFile->handle, vfsFile->file);
    ltfsUnlock(vfsFile);
    if (ret < 0) {
        return VFS_RES_ERR;
    }
    return VFS_RES_OK;
}

vfsRes_t ltfsSize(void *file, uint32_t *size) {
    MINI_ASSERT_MSG(file != NULL, "file is NULL");
    vfsFile_t *vfsFile = (vfsFile_t *) file;
    MINI_ASSERT_MSG(vfsFile->file != NULL, "file->file is NULL");
    MINI_ASSERT_MSG(vfsFile->handle != NULL, "handle is 0");

    ltfsLock(vfsFile);
    int fileSize = lfs_file_size((lfs_t *) vfsFile->handle, vfsFile->file);
    ltfsUnlock(vfsFile);
    if (fileSize < 0) {
        return VFS_RES_ERR;
    }

    if (size != NULL) {
        *size = fileSize;
    }

    return VFS_RES_OK;
}

vfsRes_t ltfsUnlink(void *file, const char *path) {
    MINI_ASSERT_MSG(file != NULL, "file is NULL");
    MINI_ASSERT_MSG(path != NULL, "path is NULL");
    vfsFile_t *vfsFile = (vfsFile_t *) file;
    MINI_ASSERT_MSG(vfsFile->file != NULL, "file->file is NULL");
    MINI_ASSERT_MSG(vfsFile->handle != NULL, "handle is 0");

    ltfsLock(vfsFile);
    int ret = lfs_remove((lfs_t *) vfsFile->handle , path);
    ltfsUnlock(vfsFile);
    if (ret < 0) {
        return VFS_RES_ERR;
    }
    return VFS_RES_OK;
}

vfsRes_t ltfsStat(void *file, const char *path, vfsFileInfo_t *info) {
    MINI_ASSERT_MSG(file != NULL, "file is NULL");
    MINI_ASSERT_MSG(path != NULL, "path is NULL");
    MINI_ASSERT_MSG(info != NULL, "info is NULL");
    vfsFile_t *vfsFile = (vfsFile_t *) file;
    MINI_ASSERT_MSG(vfsFile->handle != NULL, "handle is 0");

    struct lfs_info lfsInfo;
    ltfsLock(vfsFile);
    int ret = lfs_stat((lfs_t *) vfsFile->handle, path, &lfsInfo);
    ltfsUnlock(vfsFile);
    if (ret < 0) {
        return VFS_RES_ERR;
    }

    info->size = lfsInfo.size;
    info->type = lfsInfo.type;
    strncpy(info->name, lfsInfo.name, sizeof(info->name) - 1);

    return VFS_RES_OK;
}

vfsRes_t ltfsRename(void *file, const char *oldPath, const char *newPath) {
    MINI_ASSERT_MSG(file != NULL, "file is NULL");
    MINI_ASSERT_MSG(oldPath != NULL, "oldPath is NULL");
    MINI_ASSERT_MSG(newPath != NULL, "newPath is NULL");
    vfsFile_t *vfsFile = (vfsFile_t *) file;
    MINI_ASSERT_MSG(vfsFile->file != NULL, "file->file is NULL");
    MINI_ASSERT_MSG(vfsFile->handle != NULL, "handle is 0");

    ltfsLock(vfsFile);
    int ret = lfs_rename((lfs_t *) vfsFile->handle, oldPath, newPath);
    ltfsUnlock(vfsFile);
    if (ret < 0) {
        return VFS_RES_ERR;
    }
    return VFS_RES_OK;
}

vfsRes_t ltfsMkdir(void *file, const char *path) {
    MINI_ASSERT_MSG(file != NULL, "file is NULL");
    MINI_ASSERT_MSG(path != NULL, "path is NULL");
    vfsFile_t *vfsFile = (vfsFile_t *) file;
    MINI_ASSERT_MSG(vfsFile->handle != NULL, "handle is 0");

    ltfsLock(vfsFile);
    int ret = lfs_mkdir((lfs_t *) vfsFile->handle, path);
    ltfsUnlock(vfsFile);
    if (ret < 0) {
        return VFS_RES_ERR;
    }
    return VFS_RES_OK;
}

vfsRes_t ltfsDirOpen(void *dir, const char *path) {
    MINI_ASSERT_MSG(dir != NULL, "dir is NULL");
    MINI_ASSERT_MSG(path != NULL, "path is NULL");
    vfsFile_t *vfsFile = (vfsFile_t *) dir;

    MINI_ASSERT_MSG(vfsFile->handle != NULL, "file->handle is NULL");

    vfsFile->file = malloc(sizeof(lfs_dir_t));
    if (vfsFile->file == NULL) {
        return VFS_RES_ERR;
    }
    memset(vfsFile->file, 0, sizeof(lfs_dir_t));

    ltfsLock(vfsFile);
    int ret = lfs_dir_open((lfs_t *)vfsFile->handle, (lfs_dir_t *) vfsFile->file, path);
    ltfsUnlock(vfsFile);
    if (ret < 0) {
        return VFS_RES_ERR;
    }

    return VFS_RES_OK;
}

vfsRes_t ltfsDirClose(void *dir) {
    MINI_ASSERT_MSG(dir != NULL, "dir is NULL");
    vfsFile_t *vfsFile = (vfsFile_t *) dir;
    MINI_ASSERT_MSG(vfsFile->file != NULL, "file->file is NULL");
    MINI_ASSERT_MSG(vfsFile->handle != NULL, "file->handle is NULL");

    ltfsLock(vfsFile);
    int ret = lfs_dir_close((lfs_t *)vfsFile->handle, (lfs_dir_t *) vfsFile->file);
    ltfsUnlock(vfsFile);
    if (ret < 0) {
        return VFS_RES_ERR;
    }

    if (vfsFile->file != NULL) {
        free(vfsFile->file);
        vfsFile->file = NULL;
    }

    return VFS_RES_OK;
}

vfsRes_t ltfsDirRead(void *dir, vfsFileInfo_t *info) {
    MINI_ASSERT_MSG(dir != NULL, "dir is NULL");
    MINI_ASSERT_MSG(info != NULL, "info is NULL");
    vfsFile_t *vfsFile = (vfsFile_t *) dir;
    MINI_ASSERT_MSG(vfsFile->file != NULL, "file->file is NULL");
    MINI_ASSERT_MSG(vfsFile->handle != NULL, "file->handle is NULL");

    struct lfs_info lfsInfo;
    ltfsLock(vfsFile);
    int ret = lfs_dir_read((lfs_t *)vfsFile->handle, (lfs_dir_t *) vfsFile->file, &lfsInfo);
    ltfsUnlock(vfsFile);
    if (ret < 0) {
        return VFS_RES_ERR;
    }

    info->size = lfsInfo.size;
    info->type = lfsInfo.type;
    info->isDir = lfsInfo.type == LFS_TYPE_DIR;
    strncpy(info->name, lfsInfo.name, sizeof(info->name) - 1);

    return VFS_RES_OK;
}

vfsRes_t ltfsDirRemove(void *file, const char *path) {
    MINI_ASSERT_MSG(file != NULL, "file is NULL");
    MINI_ASSERT_MSG(path != NULL, "path is NULL");
    vfsFile_t *vfsFile = (vfsFile_t *) file;
    MINI_ASSERT_MSG(vfsFile->handle != NULL, "file->handle is NULL");

    ltfsLock(vfsFile);
    int ret = lfs_remove((lfs_t *) vfsFile->handle, path);
    ltfsUnlock(vfsFile);
    if (ret < 0) {
        return VFS_RES_ERR;
    }

    return VFS_RES_OK;
}

vfsRes_t ltfsChdir(void *dir, const char *path){
    UNUSED(dir);
    MINI_ASSERT_MSG(path != NULL, "path is NULL");

    strncpy(vfsLittleFsDriverGet()->curPath, path, sizeof(vfsLittleFsDriverGet()->curPath) - 1);

    return VFS_RES_OK;
}

vfsRes_t ltfsGetcwd(void *dir, char *buffer, size_t bufferSize){
    UNUSED(dir);
    UNUSED(buffer);
    UNUSED(bufferSize);

    strncpy(buffer, vfsLittleFsDriverGet()->curPath, bufferSize - 1);

    return VFS_RES_OK;
}

static vfsDrv_t vfsLittleFsDriver = {
    .open = ltfsOpen,
    .read = ltfsRead,
    .write = ltfsWrite,
    .seek = ltfsSeek,
    .close = ltfsClose,
    .truncate = ltfsTruncate,
    .sync = ltfsSync,
    .unlink = ltfsUnlink,
    .stat = ltfsStat,
    .rename = ltfsRename,
    .tell = ltfsTell,
    .size = ltfsSize,
    .mkdir = ltfsMkdir,
    .opendir = ltfsDirOpen,
    .closedir = ltfsDirClose,
    .readdir = ltfsDirRead,
    .rmdir = ltfsDirRemove,
    .chdir = ltfsChdir,
    .getcwd = ltfsGetcwd,
};

vfsDrv_t * vfsLittleFsDriverGet() {
    return &vfsLittleFsDriver;
}