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
#include <string.h>
#include "fs/vfs/vfs.h"
#include "fs/vfs/fatfsManager.h"
#include "fs/vfs/littlefsManager.h"
#include "fs/vfs/vfsFatDriver.h"
#include "fs/vfs/vfsLittleFsDriver.h"
#include "fs/FatFs/App/fatfs.h"
#include "MiniAssert.h"


static vfsManager_t vfsManager = {
        .curPath = {0},
        .fsCount = 0,
        .fsList = NULL
};

static void vfsAddFileSystem(vfsFileSystem_t *fileSystem) {
    vfsManager.fsCount++;
    vfsManager.fsList = realloc(vfsManager.fsList, vfsManager.fsCount * sizeof(vfsFileSystem_t));
    vfsManager.fsList[vfsManager.fsCount - 1] = *fileSystem;
}

void vfsRegisterFileSystem(char *letter, void *fsDrv, void *fHandle) {
    vfsFileSystem_t *fileSystem = malloc(sizeof(vfsFileSystem_t));
    fileSystem->fsDrv = fsDrv;
    fileSystem->fHandle = fHandle;
    strncpy(fileSystem->fsLetter, letter, VFS_MAX_FILESYSTEM_NAME_LENGTH);
    vfsAddFileSystem(fileSystem);
}

size_t vfsGetFileSystemCount(void) {
    return vfsManager.fsCount;
}

vfsFileSystem_t *vfsGetFileSystem(uint8_t index) {
    if (index >= vfsManager.fsCount) {
        return NULL;
    }
    return &vfsManager.fsList[index];
}

void vfsSetCurrentPath(const char *path) {
    strncpy(vfsManager.curPath, path, VFS_NAME_MAX);
}

char * vfsGetCurrentPath(void) {
    return vfsManager.curPath;
}

char *vfsGetRealPath(char *path) {
    if (path == NULL) {
        return NULL;
    }

    // Remove leading spaces
    while (*path == ' ') {
        path++;
    }

    char *end = path + strlen(path) - 1;
    if (*end == '/') {
        end--;
        if (end > path ){
            if (*end == '.' && *(end - 1) == '.'){
                *(end + 1) = '\0';
            }else if(*end == '.'){
                *end = '\0';
            }
        } else {
            path[0] = '/';
            path[1] = '\0';
            return path;
        }
    }

    // Remove extra slashes and handle . and ..
    char *src = path, *dst = path;
    while (*src != '\0') {
        if (*src == '/') {
            // Skip extra slashes
            while (*src == '/') src++;
            if (*src == '.') {
                src++;
                if (*src == '.') {
                    // handle ..
                    src++;
                    if (dst > path && *dst == '/') {
                        dst--;  // Go back before the last slash
                        while (dst > path && *dst != '/') dst--;
                    }
                }
            } else {
                *dst++ = '/';
            }
        } else {
            *dst++ = *src++;
        }
    }

    if (dst == path) {
        path[0] = '/';
        path[1] = '\0';

        return path;
    }
    *dst = '\0';

    end = path + strlen(path) - 1;
    while(end > path && *end == '/'){
        *end = '\0';
        end--;
    }

    return path;
}

int vfsSystemInit(void) {
    fatfsManager_init();
    lfsManager_init();

    vfsRegisterFileSystem(VFS_PATH_SDCARD, vfsFatFsDriverGet(), fatfsManager_getFatfs());
    vfsRegisterFileSystem(VFS_PATH_SYSTEM, vfsLittleFsDriverGet(), lfsManager_getLfs(LFS_SYSTEM_PARTITION));
    vfsRegisterFileSystem(VFS_PATH_DATA, vfsLittleFsDriverGet(), lfsManager_getLfs(LFS_DATA_PARTITION));
    vfsRegisterFileSystem(VFS_PATH_LOG, vfsLittleFsDriverGet(), lfsManager_getLfs(LFS_LOG_PARTITION));
    vfsRegisterFileSystem(VFS_PATH_USER, vfsLittleFsDriverGet(), lfsManager_getLfs(LFS_USER_PARTITION));

    return 0;
}

__unused vfsRes_t vfsOpen(vfsFile_t *file, const char *path, vfsMode_t flags) {
    uint8_t i;

    MINI_ASSERT_MSG(file != NULL, "file is NULL");
    MINI_ASSERT_MSG(path != NULL, "path is NULL");

    if (path[0] == '/') {
        path = path + 1;
    }

    for (i = 0; i < vfsManager.fsCount; i++) {
        if (strncmp(path, vfsManager.fsList[i].fsLetter, strlen(vfsManager.fsList[i].fsLetter)) == 0) {
            file->fsDrv = vfsManager.fsList[i].fsDrv;
            file->handle = vfsManager.fsList[i].fHandle;
            const char *realPath = path + strlen(vfsManager.fsList[i].fsLetter);
            return ((vfsDrv_t *) vfsManager.fsList[i].fsDrv)->open(file, realPath, flags);
        }
    }
    return VFS_RES_NOT_FOUND;
}

__unused vfsRes_t vfsClose(vfsFile_t *file) {
    uint8_t i;
    for (i = 0; i < vfsManager.fsCount; i++) {
        if (file->fsDrv == vfsManager.fsList[i].fsDrv) {
            return ((vfsDrv_t *) vfsManager.fsList[i].fsDrv)->close(file);
        }
    }
    return VFS_RES_NOT_FOUND;
}

__unused vfsRes_t vfsRead(vfsFile_t *file, void *buf, uint32_t len, uint32_t *read) {
    uint8_t i;
    for (i = 0; i < vfsManager.fsCount; i++) {
        if (file->fsDrv == vfsManager.fsList[i].fsDrv) {
            return ((vfsDrv_t *) vfsManager.fsList[i].fsDrv)->read(file, buf, len, read);
        }
    }
    return VFS_RES_NOT_FOUND;
}

__unused vfsRes_t vfsWrite(vfsFile_t *file, const void *buf, uint32_t len, uint32_t *written) {
    uint8_t i;
    for (i = 0; i < vfsManager.fsCount; i++) {
        if (file->fsDrv == vfsManager.fsList[i].fsDrv) {
            return ((vfsDrv_t *) vfsManager.fsList[i].fsDrv)->write(file, buf, len, written);
        }
    }
    return VFS_RES_NOT_FOUND;
}

__unused vfsRes_t vfsSeek(vfsFile_t *file, uint32_t pos, vfsSeekMode_t whence) {
    uint8_t i;
    for (i = 0; i < vfsManager.fsCount; i++) {
        if (file->fsDrv == vfsManager.fsList[i].fsDrv) {
            return ((vfsDrv_t *) vfsManager.fsList[i].fsDrv)->seek(file, pos, whence);
        }
    }
    return VFS_RES_NOT_FOUND;
}


__unused vfsRes_t vfsTell(vfsFile_t *file, uint32_t *pos) {
    uint8_t i;
    for (i = 0; i < vfsManager.fsCount; i++) {
        if (file->fsDrv == vfsManager.fsList[i].fsDrv) {
            return ((vfsDrv_t *) vfsManager.fsList[i].fsDrv)->tell(file, pos);
        }
    }
    return VFS_RES_NOT_FOUND;
}

__unused vfsRes_t vfsSize(vfsFile_t *file, uint32_t *size) {
    uint8_t i;
    for (i = 0; i < vfsManager.fsCount; i++) {
        if (file->fsDrv == vfsManager.fsList[i].fsDrv) {
            return ((vfsDrv_t *) vfsManager.fsList[i].fsDrv)->size(file, size);
        }
    }
    return VFS_RES_NOT_FOUND;
}


__unused vfsRes_t vfsTruncate(vfsFile_t *file, uint32_t length) {
    uint8_t i;
    for (i = 0; i < vfsManager.fsCount; i++) {
        if (file->fsDrv == vfsManager.fsList[i].fsDrv) {
            return ((vfsDrv_t *) vfsManager.fsList[i].fsDrv)->truncate(file, length);
        }
    }
    return VFS_RES_NOT_FOUND;
}


__unused vfsRes_t vfsSync(vfsFile_t *file) {
    uint8_t i;
    for (i = 0; i < vfsManager.fsCount; i++) {
        if (file->fsDrv == vfsManager.fsList[i].fsDrv) {
            return ((vfsDrv_t *) vfsManager.fsList[i].fsDrv)->sync(file);
        }
    }
    return VFS_RES_NOT_FOUND;
}


__unused vfsRes_t vfsStat(const char *path, vfsFileInfo_t *st) {
    uint8_t i;

    for (i = 0; i < vfsManager.fsCount; i++) {
        if (strncmp(path, vfsManager.fsList[i].fsLetter, strlen(vfsManager.fsList[i].fsLetter)) == 0) {
            vfsFile_t file = {
                    .handle = vfsManager.fsList[i].fHandle,
            };
            const char *realPath = path + strlen(vfsManager.fsList[i].fsLetter);
            return ((vfsDrv_t *) vfsManager.fsList[i].fsDrv)->stat(&file, realPath, st);
        }
    }

    return VFS_RES_NOT_FOUND;
}


__unused vfsRes_t vfsUnlink(const char *path) {
    uint8_t i;

    MINI_ASSERT_MSG(path != NULL, "path is NULL");

    if (path[0] == '/') {
        path = path + 1;
    }

    for (i = 0; i < vfsManager.fsCount; i++) {
        if (strncmp(path, vfsManager.fsList[i].fsLetter, strlen(vfsManager.fsList[i].fsLetter)) == 0) {
            vfsFile_t file = {
                    .handle = vfsManager.fsList[i].fHandle,
            };
            const char *realPath = path + strlen(vfsManager.fsList[i].fsLetter);
            return ((vfsDrv_t *) vfsManager.fsList[i].fsDrv)->unlink(&file, realPath);
        }
    }

    return VFS_RES_NOT_FOUND;
}


__unused vfsRes_t vfsRename(const char *oldName, const char *newName) {
    uint8_t i;

    MINI_ASSERT_MSG(oldName != NULL, "oldName is NULL");
    MINI_ASSERT_MSG(newName != NULL, "newName is NULL");

    if (oldName[0] == '/') {
        oldName = oldName + 1;
    }

    if (newName[0] == '/') {
        newName = newName + 1;
    }

    for (i = 0; i < vfsManager.fsCount; i++) {
        if (strncmp(oldName, vfsManager.fsList[i].fsLetter, strlen(vfsManager.fsList[i].fsLetter)) == 0) {
            vfsFile_t file = {
                    .handle = vfsManager.fsList[i].fHandle,
            };
            const char *realOldName = oldName + strlen(vfsManager.fsList[i].fsLetter);
            const char *realNewName = newName + strlen(vfsManager.fsList[i].fsLetter);

            return ((vfsDrv_t *) vfsManager.fsList[i].fsDrv)->rename(&file, realOldName, realNewName);
        }
    }

    return VFS_RES_NOT_FOUND;
}


__unused vfsRes_t vfsMkdir(const char *path) {
    uint8_t i;

    MINI_ASSERT_MSG(path != NULL, "path is NULL");

    if (path[0] == '/') {
        path = path + 1;
    }

    for (i = 0; i < vfsManager.fsCount; i++) {
        if (strncmp(path, vfsManager.fsList[i].fsLetter, strlen(vfsManager.fsList[i].fsLetter)) == 0) {
            vfsFile_t file = {
                    .handle = vfsManager.fsList[i].fHandle,
            };
            const char *realPath = path + strlen(vfsManager.fsList[i].fsLetter);
            return ((vfsDrv_t *) vfsManager.fsList[i].fsDrv)->mkdir(&file, realPath);
        }
    }

    return VFS_RES_NOT_FOUND;
}

__unused vfsRes_t vfsRemoveDir(const char *path) {
    uint8_t i;

    MINI_ASSERT_MSG(path != NULL, "path is NULL");

    if (path[0] == '/') {
        path = path + 1;
    }

    for (i = 0; i < vfsManager.fsCount; i++) {
        if (strncmp(path, vfsManager.fsList[i].fsLetter, strlen(vfsManager.fsList[i].fsLetter)) == 0) {
            vfsFile_t file = {
                    .handle = vfsManager.fsList[i].fHandle,
            };
            const char *realPath = path + strlen(vfsManager.fsList[i].fsLetter);
            return ((vfsDrv_t *) vfsManager.fsList[i].fsDrv)->rmdir(&file, realPath);
        }
    }

    return VFS_RES_NOT_FOUND;
}


__unused vfsRes_t vfsDirOpen(vfsDir_t *dir, const char *path) {
    uint8_t i;

    MINI_ASSERT_MSG(path != NULL, "path is NULL");

    if (path[0] == '/') {
        path = path + 1;
    }

    for (i = 0; i < vfsManager.fsCount; i++) {
        if (strncmp(path, vfsManager.fsList[i].fsLetter, strlen(vfsManager.fsList[i].fsLetter)) == 0) {
            dir->fsDrv = vfsManager.fsList[i].fsDrv;
            dir->handle = vfsManager.fsList[i].fHandle;

            char realPath[256] = {0};
            strncpy(realPath, path + strlen(vfsManager.fsList[i].fsLetter), 256);
            if (realPath[strlen(realPath) - 1] == '/'){
                realPath[strlen(realPath) - 1] = '\0';
            }

            return ((vfsDrv_t *) vfsManager.fsList[i].fsDrv)->opendir(dir, realPath);
        }
    }

    return VFS_RES_NOT_FOUND;
}


__unused vfsRes_t vfsDirRead(vfsDir_t *dir, vfsFileInfo_t *fileInfo) {
    uint8_t i;

    for (i = 0; i < vfsManager.fsCount; i++) {
        if (dir->fsDrv == vfsManager.fsList[i].fsDrv) {
            return ((vfsDrv_t *) vfsManager.fsList[i].fsDrv)->readdir(dir, fileInfo);
        }
    }

    return VFS_RES_NOT_FOUND;
}


__unused vfsRes_t vfsDirClose(vfsDir_t *dir) {
    uint8_t i;

    for (i = 0; i < vfsManager.fsCount; i++) {
        if (dir->fsDrv == vfsManager.fsList[i].fsDrv) {
            return ((vfsDrv_t *) vfsManager.fsList[i].fsDrv)->closedir(dir);
        }
    }

    return VFS_RES_NOT_FOUND;
}

__unused vfsRes_t vfsChdir(const char *path) {
    uint8_t i;

    if (path == NULL || strlen(path) < 1) {
        return VFS_RES_INVALID_PARAM;
    }

    MINI_ASSERT_MSG(path != NULL, "path is NULL");

    if (path[0] == '/') {
        path = path + 1;
    }

    for (i = 0; i < vfsManager.fsCount; i++) {
        if (strncmp(path, vfsManager.fsList[i].fsLetter, strlen(vfsManager.fsList[i].fsLetter)) == 0) {
            vfsFile_t file = {
                    .handle = vfsManager.fsList[i].fHandle,
            };

            const char *realPath = path + strlen(vfsManager.fsList[i].fsLetter) ;
            if (((vfsDrv_t *) vfsManager.fsList[i].fsDrv)->chdir == NULL) {
                return VFS_RES_NOT_SUPPORTED;
            }

            vfsRes_t res = ((vfsDrv_t *) vfsManager.fsList[i].fsDrv)->chdir(&file, realPath);
            if (res == VFS_RES_OK){
                strncpy(vfsManager.curPath , path, sizeof(vfsManager.curPath));
                strcat(vfsManager.curPath , "/");
            }

            return res ;
        }
    }

    return VFS_RES_NOT_FOUND;
}

__unused vfsRes_t vfsGetcwd(char *buffer, size_t maxSize) {
    uint8_t i;

    if (vfsManager.curPath[0] == 0){
        strncpy(buffer, "/", maxSize);
        return VFS_RES_OK;
    }

    for (i = 0; i < vfsManager.fsCount; i++) {
        if (strncmp(vfsManager.curPath, vfsManager.fsList[i].fsLetter, strlen(vfsManager.fsList[i].fsLetter)) == 0) {
            if (((vfsDrv_t *) vfsManager.fsList[i].fsDrv)->getcwd != NULL) {
                vfsFile_t file = {
                        .handle = vfsManager.fsList[i].fHandle,
                };

                memset(buffer, 0 ,maxSize);

                buffer[0] = '/';
                strncpy(buffer + 1, vfsManager.fsList[i].fsLetter, maxSize - 1);
                vfsRes_t res = ((vfsDrv_t *) vfsManager.fsList[i].fsDrv)->getcwd(&file,
                                                                         buffer + strlen(buffer),
                                                                         maxSize - strlen(buffer));
                if (res == VFS_RES_OK && buffer[strlen(buffer) - 1] != '/') {
                    strcat(buffer, "/");
                }

                return res ;
            }
        }
    }

    return VFS_RES_NOT_FOUND;
}

