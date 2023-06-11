
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
#include <stdio.h>
#include <inttypes.h>
#include "console/lettershell/shell.h"
#include "kernel/FreeRTOS/FreeRTOS.h"
#include "kernel/FreeRTOS/task.h"
#include "driver/main.h"
#include "kernel/FreeRTOS/CMSIS_RTOS_V2/cmsis_os.h"
#include "driver/usart.h"
#include "common/framework/MiniCommon.h"
#include "fs/vfs/vfs.h"
#include "driver/hal/hal_uart.h"
#include "driver/hal/hal_os.h"

#undef LOG_TAG
#define LOG_TAG "MINISHELL"


/**
 * @brief rm file
 * @param path
 * @return
 */
size_t userShellUnlink(char *path) {
    if (path == NULL) {
        return VFS_RES_ERR;
    }

    return vfsUnlink(path);
}


/**
 * @brief Create directory in current working directory
 * @param path
 * @return
 */
size_t userShellMkdir(char *path) {
    if (path == NULL) {
        return VFS_RES_ERR;
    }

    char *realPath = vfsGetRealPath(path);
    if (realPath == NULL) {
        return VFS_RES_INVALID_PATH;
    }

    return vfsMkdir(path);
}

static uint8_t isSystemRootPath(char *path, char *buffer, size_t bufferSize, int printDetail) {
    if (path == NULL) {
        return 0;
    }

    if (path[0] == '/' && path[1] == 0) {
        int fileSystemCount = vfsGetFileSystemCount();
        for (int i = 0; i < fileSystemCount; ++i) {
            vfsFileSystem_t *fileSystem = vfsGetFileSystem(i);
            if (fileSystem) {
                if (printDetail == SHELL_LS_MATCH){
                    strcat(buffer, fileSystem->fsLetter);
                    strcat(buffer, " ");
                }else {
                    strcat(buffer, CSI(31));
                    strcat(buffer, fileSystem->fsLetter);
                    strcat(buffer, CSI(39));
                    strcat(buffer, "\t");
                }
            }
        }
        return 1;
    }

    return 0;
}
/**
 * @brief Get current working directory
 * @param buffer
 * @param bufferSize
 * @return
 */
size_t userShellGetcwd(char *buffer, size_t bufferSize) {
    if (buffer == NULL || bufferSize == 0) {
        return VFS_RES_INVALID_PARAM;
    }

    char *path = vfsGetCurrentPath();
    if (path[0] == '/' && path[1] == 0) {
        strcpy(buffer, "/");
        buffer[1] = '\0';

        return VFS_RES_OK;
    }

    return vfsGetcwd(buffer, bufferSize);
}

/**
 * @brief change directory
 * @param path
 * @return
 */
size_t userShellChdir(char *path) {
    if (path == NULL) {
        return VFS_RES_INVALID_PATH;
    }

    char *realPath = vfsGetRealPath(path);
    if (realPath == NULL) {
        return VFS_RES_INVALID_PATH;
    }

    if (realPath[0] == '/' && realPath[1] == 0) {
        vfsSetCurrentPath("/");

        return VFS_RES_OK;
    }

    /*add '/' if path is filesystem fsletter */
    if (realPath[0] == '/' && realPath[1] != '\0') {
        size_t fileSystemCount = vfsGetFileSystemCount();
        for (int i = 0; i < fileSystemCount; ++i) {
            vfsFileSystem_t *fileSystem = vfsGetFileSystem(i);
            if (fileSystem && !strncmp(fileSystem->fsLetter, &realPath[1], strlen(fileSystem->fsLetter)) &&
                    realPath[strlen(fileSystem->fsLetter) + 1] == '\0') {
                realPath[strlen(fileSystem->fsLetter) + 1] = '/';
                realPath[strlen(fileSystem->fsLetter) + 2] = '\0';
                break;
            }
        }
    }

    vfsRes_t res = vfsChdir(realPath);
    return res ;
}

static uint32_t getDirSize(char *path) {
    vfsRes_t res;
    vfsFile_t dir;
    vfsFileInfo_t fno;
    uint32_t size = 0;

    if (path == NULL) {
        return 0;
    }

    res = vfsDirOpen(&dir, &path[1]);
    if (res != VFS_RES_OK) {
//        log_e("opendir %s failed,err:%d", path, res);
        return 0;
    }

    while (vfsDirRead(&dir, &fno) == VFS_RES_OK) {
        if (fno.name[0] == '\0') {
            break;
        }

        if (fno.name[0] == '.') {
            continue;
        }

        if (fno.isDir) {
            char *oldPath = path + strlen(path);
            strcat(path, "/");
            strcat(path, fno.name);
            size += getDirSize(path);
            *oldPath = '\0';
        } else {
            size += fno.size;
        }
    }

    vfsDirClose(&dir);

    return size;
}

static bool is_integer(float num) {
    return floor(num) == num;
}
/**
 * @brief 列出文件
 *
 * @param path 路径
 * @param buffer 结果缓冲
 * @param maxLen 最大缓冲长度
 * @return size_t 0
 */
size_t userShellListDir(char *path, char *buffer, size_t maxLen, int printDetail) {
    vfsRes_t res;
    vfsFile_t dir;
    vfsFileInfo_t fno;

    if (path == NULL || buffer == NULL || maxLen == 0) {
        return -1;
    }

    if (isSystemRootPath(path, buffer, maxLen, printDetail)) {
        return 0;
    }

    res = vfsDirOpen(&dir, &path[1]);
    if (res != VFS_RES_OK) {
        log_e("opendir failed,err:%d", res);
        return -1;
    }

    memset(buffer, 0, maxLen);

    uint32_t fileSize  = 0;
    do {
        res = vfsDirRead(&dir, &fno);
        if (res != VFS_RES_OK || fno.name[0] == 0) {
            break;
        }

        if (fno.name[0] == '.') {
            continue;
        }

        if (fno.isDir) {
            char *oldPath = path + strlen(path);
            strcat(path, fno.name);
            fileSize = getDirSize(path);
            if (printDetail != SHELL_LS_MATCH) {
                strcat(buffer, CSI(31));
            }
            *oldPath = '\0';
        } else {
            if (printDetail != SHELL_LS_MATCH) {
                strcat(buffer, CSI(32));
            }
            fileSize = fno.size;
        }

        if (printDetail == SHELL_LS_MATCH){
            strcat(buffer, fno.name);
            strcat(buffer, " ");
        }else if (printDetail == SHELL_LS_PRINT){
            strcat(buffer, fno.name);
            strcat(buffer, CSI(39));
            strcat(buffer, "\t");
        }else if (printDetail == SHELL_LS_PRINT_AL){
            snprintf(buffer + strlen(buffer), maxLen - strlen(buffer), "%-20s\t\t"CSI(39)"%"PRIu32"\r\n",fno.name, fileSize);
        }else if (printDetail == SHELL_LS_PRINT_HUMAN){
            char *unit[] = {"", "KB", "MB", "GB", "TB"};
            int humanMode = 0;
            float size = fileSize;
            while(size > 1024) {
                size /= 1024;
                humanMode++;
            }

            if (is_integer(size)){
                snprintf(buffer + strlen(buffer), maxLen - strlen(buffer), "%-20s\t\t"CSI(39)"%lu%s\r\n",fno.name, (uint32_t)size,unit[humanMode]);
            }else {
                snprintf(buffer + strlen(buffer), maxLen - strlen(buffer), "%-20s\t\t"CSI(39)"%.1f%s\r\n",fno.name, size,unit[humanMode]);
            }
        }
    } while (1);

    vfsDirClose(&dir);

    return 0;
}

/**
 * @brief  init shell handle
 * 
 */
void MiniShell_Init(ConsoleDev_t *console) {
    Shell *shell = &console->shell;

    console->shellPathBuffer[0] = '/';
    shellSetPath(shell, console->shellPathBuffer);
    shellInit(shell, console->buffer, sizeof (console->buffer));

    ShellFs *shellFs = &console->shellFs;
    shellFs->getcwd = userShellGetcwd;
    shellFs->chdir = userShellChdir;
    shellFs->listdir = userShellListDir;
    shellFs->mkdir = userShellMkdir;
    shellFs->unlink = userShellUnlink;

    shellFsInit(shellFs, console->shellPathBuffer, sizeof (console->shellPathBuffer));
    shellCompanionAdd(shell, SHELL_COMPANION_ID_FS, shellFs);

    Log *shellLog = &console->shellLog;
    shellLog->active = 1;
    shellLog->level = LOG_DEBUG;

    logRegister(shellLog, shell);

//  logDebug("hello world");
//  logHexDump(LOG_ALL_OBJ, LOG_DEBUG, (void *)&miniShell, sizeof(miniShell));
}
