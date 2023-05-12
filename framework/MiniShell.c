
/**
 * Copyright 2022 dengzhijun
 * 
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 * 
 *     http://www.apache.org/licenses/LICENSE-2.0
 * 
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include "lettershell/shell.h"
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"
#include <stdio.h>
#include "usart.h"
#include "MiniCommon.h"
#include "vfs/vfs.h"

#undef LOG_TAG
#define LOG_TAG "MINISHELL"

static char shellBuffer[MINI_SHELL_BUFFER_SIZE] = {0};
static Shell miniShell;
ShellFs shellFs;
char shellPathBuffer[512] = "/";
Log shellLog = {
        .active = 1,
        .level = LOG_DEBUG
};

static osMutexId_t shellMutex;
const osMutexAttr_t shellMutex_attributes = {
        .name = "shellMutex",
        .attr_bits = osMutexRecursive,
};

osThreadId_t shellTaskHandle;;
const osThreadAttr_t shellTask_attributes = {
        .name = "miniShell",
        .stack_size = MINI_SHELL_TASK_STACK_SIZE,
        .priority = (osPriority_t) osPriorityLow,
};

signed short ShellRead(char *data, unsigned short dataSize) {
    return MiniUart_readData(MINI_SHELL_USART_IDX, data, dataSize);
}

signed short ShellWrite(char *data, unsigned short dataSize) {
    usb_write_data(data, dataSize);

    return MiniUart_writeData(MINI_SHELL_USART_IDX, data, dataSize);
}

int ShellLock(Shell *shell) {
    osMutexAcquire(shellMutex, portMAX_DELAY);
    return 0;
}

int ShellUnlock(Shell *shell) {
    osMutexRelease(shellMutex);
    return 0;
}

/**
 * @brief 日志写函数实现
 *
 * @param buffer 数据
 * @param len 数据长度
 *
 */
void terminalLogWrite(char *buffer, short len) {
    if (shellLog.shell) {
        shellWriteEndLine(shellLog.shell, buffer, len);
    }
}

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

static uint8_t isSystemRootPath(char *path, char *buffer, size_t bufferSize) {
    if (path == NULL) {
        return 0;
    }

    if (path[0] == '/' && path[1] == 0) {
        int fileSystemCount = vfsGetFileSystemCount();
        for (int i = 0; i < fileSystemCount; ++i) {
            vfsFileSystem_t *fileSystem = vfsGetFileSystem(i);
            if (fileSystem) {
                strcat(buffer, CSI(31));
                strcat(buffer, fileSystem->fsLetter);
                strcat(buffer, CSI(39));
                strcat(buffer, "\t");
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
    if (realPath[0] == '/' && realPath[1] != 0) {
        size_t fileSystemCount = vfsGetFileSystemCount();
        for (int i = 0; i < fileSystemCount; ++i) {
            vfsFileSystem_t *fileSystem = vfsGetFileSystem(i);
            if (fileSystem && !strncmp(fileSystem->fsLetter, &path[1], strlen(fileSystem->fsLetter)) &&
                path[strlen(fileSystem->fsLetter) + 1] == 0) {
                realPath[strlen(fileSystem->fsLetter) + 1] = '/';
            }
        }
    }

    vfsRes_t res = vfsChdir(path);
    return res ;
}

/**
 * @brief 列出文件
 *
 * @param path 路径
 * @param buffer 结果缓冲
 * @param maxLen 最大缓冲长度
 * @return size_t 0
 */
size_t userShellListDir(char *path, char *buffer, size_t maxLen) {
    vfsRes_t res;
    vfsFile_t dir;
    vfsFileInfo_t fno;

    if (path == NULL || buffer == NULL || maxLen == 0) {
        return -1;
    }

    if (isSystemRootPath(path, buffer, maxLen)) {
        return 0;
    }

    res = vfsDirOpen(&dir, &path[1]);
    if (res != VFS_RES_OK) {
        log_e("opendir failed,err:%d", res);
        return -1;
    }

    memset(buffer, 0, maxLen);

    do {
        res = vfsDirRead(&dir, &fno);
        if (res != VFS_RES_OK || fno.name[0] == 0) {
            break;
        }

        if (fno.isDir) {
            strcat(buffer, CSI(31));
            strcat(buffer, fno.name);
        } else {
            strcat(buffer, CSI(32));
            strcat(buffer, fno.name);
        }

        strcat(buffer, CSI(39));
        strcat(buffer, "\t");
    } while (1);

    vfsDirClose(&dir);

    return 0;
}

/**
 * @brief  init shell handle
 * 
 */
void MiniShell_Init(void) {
    miniShell.read = ShellRead;
    miniShell.write = ShellWrite;
#if SHELL_USING_LOCK
    shellMutex = osMutexNew(&shellMutex_attributes);
    miniShell.lock = ShellLock;
    miniShell.unlock = ShellUnlock;
#endif

    shellSetPath(&miniShell, shellPathBuffer);
    shellInit(&miniShell, shellBuffer, sizeof(shellBuffer));

    shellFs.getcwd = userShellGetcwd;
    shellFs.chdir = userShellChdir;
    shellFs.listdir = userShellListDir;
    shellFs.mkdir = userShellMkdir;
    shellFs.unlink = userShellUnlink;

    shellFsInit(&shellFs, shellPathBuffer, 512);
    shellCompanionAdd(&miniShell, SHELL_COMPANION_ID_FS, &shellFs);

    shellLog.write = terminalLogWrite;
    logRegister(&shellLog, &miniShell);

//  logDebug("hello world");
//  logHexDump(LOG_ALL_OBJ, LOG_DEBUG, (void *)&miniShell, sizeof(miniShell));
}

/**
 * @brief execute shell task
 * 
 */

void MiniShell_Run(void) {
    shellTaskHandle = osThreadNew(shellTask, &miniShell, &shellTask_attributes);
}