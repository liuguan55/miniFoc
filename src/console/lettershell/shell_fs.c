/**
 * @file shell_fs.c
 * @author Letter (nevermindzzt@gmail.com)
 * @brief shell file system support
 * @version 0.1
 * @date 2020-07-22
 * 
 * @copyright (c) 2020 Letter
 * 
 */
#include <stdlib.h>
#include <stdio.h>
#include <getopt.h>
#include <string.h>
#include "console/lettershell/shell_fs.h"
#include "console/lettershell/shell.h"
#include "fs/FatFs/ff.h"
#include "console/lettershell/shell_ext.h"
#include "fs/vfs/vfs.h"

/**
 * @brief 改变当前路径(shell调用)
 * 
 * @param dir 路径
 */
void shellCD(char *dir) {
    Shell *shell = shellGetCurrent();
    ShellFs *shellFs = shellCompanionGet(shell, SHELL_COMPANION_ID_FS);
    SHELL_ASSERT(shellFs, return);

    char path[256] = {0};
    if (dir[0] == '/') {
        snprintf(path, sizeof(path), "%s", dir);
    } else {
        snprintf(path, sizeof(path), "%s/%s", shellGetPath(shell), dir);
    }

    if (shellFs->chdir(path) != 0) {
        shellWriteString(shell, "error: ");
        shellWriteString(shell, path);
        shellWriteString(shell, " is not a directory\r\n");
    }

    shellFs->getcwd(shellFs->info.path, shellFs->info.pathLen);
}

SHELL_EXPORT_CMD(
        SHELL_CMD_PERMISSION(0) | SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC) | SHELL_CMD_RESERVE_CHAR |
        SHELL_CMD_DISABLE_RETURN,
        cd, shellCD, change
                dir);

/**
 * @brief 列出文件(shell调用)
 * 
 */
void shellLS(char *args) {
    char *buffer;

    EShellLsMode printType = SHELL_LS_PRINT;
    if (!strcmp(args, "-al")){
        printType = SHELL_LS_PRINT_AL;
    }

    if (!strcmp(args, "-alh")){
        printType = SHELL_LS_PRINT_HUMAN;
    }

    Shell *shell = shellGetCurrent();
    ShellFs *shellFs = shellCompanionGet(shell, SHELL_COMPANION_ID_FS);
    SHELL_ASSERT(shellFs, return);

    buffer = SHELL_MALLOC(SHELL_FS_LIST_FILE_BUFFER_MAX);
    SHELL_ASSERT(buffer, return);
    memset(buffer, 0, SHELL_FS_LIST_FILE_BUFFER_MAX);
    shellFs->listdir(shellGetPath(shell), buffer, SHELL_FS_LIST_FILE_BUFFER_MAX, printType);

    shellWriteString(shell, buffer);

    SHELL_FREE(buffer);
}

SHELL_EXPORT_CMD(
        SHELL_CMD_PERMISSION(0) | SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC) | SHELL_CMD_RESERVE_CHAR |
        SHELL_CMD_DISABLE_RETURN,
        ls, shellLS, list
                all
                files);

/**
 * brief : create new dir
 * @param dirname
 */
void shellMKDIR(char *dir) {
    char *buffer;
    Shell *shell = shellGetCurrent();
    ShellFs *shellFs = shellCompanionGet(shell, SHELL_COMPANION_ID_FS);
    SHELL_ASSERT(shellFs, return);

    buffer = SHELL_MALLOC(SHELL_FS_LIST_FILE_BUFFER_MAX);
    SHELL_ASSERT(buffer, return);

    shellFs->getcwd(shellFs->info.path, shellFs->info.pathLen);
    snprintf(buffer, SHELL_FS_LIST_FILE_BUFFER_MAX, "%s/%s", shellFs->info.path, dir);
    if (shellFs->mkdir(buffer)) {
        shellWriteString(shell, "error: ");
        shellWriteString(shell, dir);
        shellWriteString(shell, " create directory failed\r\n");
    }

    SHELL_FREE(buffer);
}

SHELL_EXPORT_CMD(
        SHELL_CMD_PERMISSION(0) | SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC) | SHELL_CMD_RESERVE_CHAR |
        SHELL_CMD_DISABLE_RETURN,
        mkdir, shellMKDIR, create
                dir);

/**
 * brief : remove  dir or file
 * @param dirname
 */
void shellRM(char *filename) {
    char *buffer;
    Shell *shell = shellGetCurrent();
    ShellFs *shellFs = shellCompanionGet(shell, SHELL_COMPANION_ID_FS);
    SHELL_ASSERT(shellFs, return);

    buffer = SHELL_MALLOC(SHELL_FS_LIST_FILE_BUFFER_MAX);
    SHELL_ASSERT(buffer, return);

    shellFs->getcwd(shellFs->info.path, shellFs->info.pathLen);
    snprintf(buffer, SHELL_FS_LIST_FILE_BUFFER_MAX, "%s/%s", shellFs->info.path, filename);
    if (shellFs->unlink(buffer)) {
        shellWriteString(shell, "error: ");
        shellWriteString(shell, filename);
        shellWriteString(shell, " remove file failed\r\n");
    } else {
        shellWriteString(shell, "delete: ");
        shellWriteString(shell, filename);
        shellWriteString(shell, " success\r\n");
    }

    SHELL_FREE(buffer);
}

SHELL_EXPORT_CMD(
        SHELL_CMD_PERMISSION(0) | SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC) | SHELL_CMD_RESERVE_CHAR |
        SHELL_CMD_DISABLE_RETURN,
        rm, shellRM, rm
                dir
                or
                file);

/**
 * brief : Create new file
 * @param dirname
 */
void shellTOUCH(char *filename) {
    char *buffer;
    Shell *shell = shellGetCurrent();
    ShellFs *shellFs = shellCompanionGet(shell, SHELL_COMPANION_ID_FS);
    SHELL_ASSERT(shellFs, return);

    buffer = SHELL_MALLOC(SHELL_FS_LIST_FILE_BUFFER_MAX);
    SHELL_ASSERT(buffer, return);

    shellFs->getcwd(shellFs->info.path, shellFs->info.pathLen);
    snprintf(buffer, SHELL_FS_LIST_FILE_BUFFER_MAX, "%s/%s", shellFs->info.path, filename);

    char *path = vfsGetRealPath(buffer);
    vfsFile_t vfsFile;
    vfsRes_t res = vfsOpen(&vfsFile, path, VFS_MODE_WRITE | VFS_MODE_CREATE);
    if (res != 0) {
        shellWriteString(shell, "error: ");
        shellWriteString(shell, filename);
        shellWriteString(shell, " touch file failed\r\n");
    }
    vfsClose(&vfsFile);

    SHELL_FREE(buffer);
}

SHELL_EXPORT_CMD(
        SHELL_CMD_PERMISSION(0) | SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC) | SHELL_CMD_RESERVE_CHAR |
        SHELL_CMD_DISABLE_RETURN,
        touch, shellTOUCH, touch
                file);

/**
 * brief : read  file
 * @param dirname
 */
void shellCAT(char *filename) {
    vfsFile_t vfsFile;
    vfsRes_t res;
    char *buffer;
    char *content;

    Shell *shell = shellGetCurrent();
    ShellFs *shellFs = shellCompanionGet(shell, SHELL_COMPANION_ID_FS);
    SHELL_ASSERT(shellFs, return);

    buffer = SHELL_MALLOC(SHELL_FS_LIST_FILE_BUFFER_MAX);
    SHELL_ASSERT(buffer, return);

    shellFs->getcwd(shellFs->info.path, shellFs->info.pathLen);
    snprintf(buffer, SHELL_FS_LIST_FILE_BUFFER_MAX, "%s/%s", shellFs->info.path, filename);

    res = vfsOpen(&vfsFile, buffer, VFS_MODE_READ);
    if (res != VFS_RES_OK) {
        shellWriteString(shell, "error: ");
        shellWriteString(shell, filename);
        shellWriteString(shell, " open file failed\r\n");
    } else {
        uint32_t readLen = 0;
        content = SHELL_MALLOC(SHELL_FS_LIST_FILE_BUFFER_MAX + 1);
        SHELL_ASSERT(buffer, return);

        while (1) {
            res = vfsRead(&vfsFile, content , SHELL_FS_LIST_FILE_BUFFER_MAX, &readLen);
            if (res != VFS_RES_OK || readLen == 0) {
                break;
            }

            content[readLen] = '\0';
            printf("%s", content);
        }
        printf("\r\n");
        SHELL_FREE(content);
    }

    vfsClose(&vfsFile);

    SHELL_FREE(buffer);
}

SHELL_EXPORT_CMD(
        SHELL_CMD_PERMISSION(0) | SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC) | SHELL_CMD_RESERVE_CHAR |
        SHELL_CMD_DISABLE_RETURN,
        cat, shellCAT, read
                file);

/**
 * brief : rename path
 * @param dirname
 */
void shellMV(char *oldPath, char *newPath) {
    vfsRes_t res;

    Shell *shell = shellGetCurrent();
    ShellFs *shellFs = shellCompanionGet(shell, SHELL_COMPANION_ID_FS);
    SHELL_ASSERT(shellFs, return);

    char oldName[256] = {0};
    if (oldPath[0] == '/') {
        snprintf(oldName, sizeof(oldName), "%s", oldPath);
    } else {
        snprintf(oldName, sizeof(oldName), "%s/%s", shellGetPath(shell), oldPath);
    }

    char newName[256] = {0};
    if (oldPath[0] == '/') {
        snprintf(newName, sizeof(newName), "%s", newPath);
    } else {
        snprintf(newName, sizeof(newName), "%s/%s", shellGetPath(shell), newPath);
    }

    char *oldFileName = vfsGetRealPath(oldName);
    char *newFileName = vfsGetRealPath(newName);

    res = vfsRename(oldFileName, newFileName);
    if (res != VFS_RES_OK) {
        shellWriteString(shell, "error: ");
        shellWriteString(shell, oldFileName);
        shellWriteString(shell, " rename file failed\r\n");
    }
}

SHELL_EXPORT_CMD(
        SHELL_CMD_PERMISSION(0) | SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC) | SHELL_CMD_RESERVE_CHAR |
        SHELL_CMD_DISABLE_RETURN,
        mv, shellMV, rename
                file);

/**
 * @brief 初始化shell文件系统支持
 * 
 * @param shellFs shell文件系统对象
 * @param pathBuffer shell路径缓冲
 * @param pathLen 路径缓冲区大小
 */
void shellFsInit(ShellFs *shellFs, char *pathBuffer, size_t pathLen) {
    shellFs->info.path = pathBuffer;
    shellFs->info.pathLen = pathLen;
    shellFs->getcwd(shellFs->info.path, pathLen);
}
