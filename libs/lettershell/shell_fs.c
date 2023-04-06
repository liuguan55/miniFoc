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
#include "shell_fs.h"
#include "shell.h"
#include "stdio.h"
#include <stdlib.h>
#include "ff.h"
#include "shell_ext.h"
/**
 * @brief 改变当前路径(shell调用)
 * 
 * @param dir 路径
 */
void shellCD(char *dir) {
  Shell *shell = shellGetCurrent();
  ShellFs *shellFs = shellCompanionGet(shell, SHELL_COMPANION_ID_FS);
  SHELL_ASSERT(shellFs, return);
  if (shellFs->chdir(dir) != 0) {
	shellWriteString(shell, "error: ");
	shellWriteString(shell, dir);
	shellWriteString(shell, " is not a directory\r\n");
  }
  shellFs->getcwd(shellFs->info.path, shellFs->info.pathLen);
}
SHELL_EXPORT_CMD(
	SHELL_CMD_PERMISSION(0) | SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC) | SHELL_CMD_RESERVE_CHAR| SHELL_CMD_DISABLE_RETURN,
	cd, shellCD, change
		dir);

/**
 * @brief 列出文件(shell调用)
 * 
 */
void shellLS(void) {
  size_t count;
  char *buffer;

  Shell *shell = shellGetCurrent();
  ShellFs *shellFs = shellCompanionGet(shell, SHELL_COMPANION_ID_FS);
  SHELL_ASSERT(shellFs, return);

  buffer = SHELL_MALLOC(SHELL_FS_LIST_FILE_BUFFER_MAX);
  SHELL_ASSERT(buffer, return);
  count = shellFs->listdir(shellGetPath(shell), buffer, SHELL_FS_LIST_FILE_BUFFER_MAX);

  shellWriteString(shell, buffer);

  SHELL_FREE(buffer);
}
SHELL_EXPORT_CMD(
	SHELL_CMD_PERMISSION(0) | SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC)| SHELL_CMD_RESERVE_CHAR | SHELL_CMD_DISABLE_RETURN,
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
	SHELL_CMD_PERMISSION(0) | SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC)  | SHELL_CMD_RESERVE_CHAR | SHELL_CMD_DISABLE_RETURN,
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
	SHELL_CMD_PERMISSION(0) | SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC)| SHELL_CMD_RESERVE_CHAR | SHELL_CMD_DISABLE_RETURN,
	rm, shellRM, rm
		dir
		or
		file);

/**
 * brief : Create new file
 * @param dirname
 */
void shellTOUCH(char *filename) {
  FIL file;
  FRESULT res;
  char *buffer;
  Shell *shell = shellGetCurrent();
  ShellFs *shellFs = shellCompanionGet(shell, SHELL_COMPANION_ID_FS);
  SHELL_ASSERT(shellFs, return);

  buffer = SHELL_MALLOC(SHELL_FS_LIST_FILE_BUFFER_MAX);
  SHELL_ASSERT(buffer, return);

  shellFs->getcwd(shellFs->info.path, shellFs->info.pathLen);
  snprintf(buffer, SHELL_FS_LIST_FILE_BUFFER_MAX, "%s/%s", shellFs->info.path, filename);

  res = f_open(&file, buffer, FA_CREATE_ALWAYS | FA_WRITE | FA_READ);
  if (res != FR_OK) {
	shellWriteString(shell, "error: ");
	shellWriteString(shell, filename);
	shellWriteString(shell, " touch file failed\r\n");
  }
  f_close(&file);

  SHELL_FREE(buffer);
}
SHELL_EXPORT_CMD(
	SHELL_CMD_PERMISSION(0) | SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC)| SHELL_CMD_RESERVE_CHAR | SHELL_CMD_DISABLE_RETURN,
	touch, shellTOUCH, touch
		file);

/**
 * brief : read  file
 * @param dirname
 */
void shellCAT(char *filename) {
  FIL file;
  FRESULT res;
  char *buffer;
  char *content;

  Shell *shell = shellGetCurrent();
  ShellFs *shellFs = shellCompanionGet(shell, SHELL_COMPANION_ID_FS);
  SHELL_ASSERT(shellFs, return);

  buffer = SHELL_MALLOC(SHELL_FS_LIST_FILE_BUFFER_MAX);
  SHELL_ASSERT(buffer, return);

  shellFs->getcwd(shellFs->info.path, shellFs->info.pathLen);
  snprintf(buffer, SHELL_FS_LIST_FILE_BUFFER_MAX, "%s/%s", shellFs->info.path, filename);

  res = f_open(&file, buffer, FA_READ);
  if (res != FR_OK) {
	shellWriteString(shell, "error: ");
	shellWriteString(shell, filename);
	shellWriteString(shell, " open file failed\r\n");
  } else {
	int readLen = 0;
	content = SHELL_MALLOC(SHELL_FS_LIST_FILE_BUFFER_MAX + 1);
	SHELL_ASSERT(buffer, return);

	while (!f_eof(&file)) {
	  res = f_read(&file, content, SHELL_FS_LIST_FILE_BUFFER_MAX, &readLen);
	  if (res != FR_OK) {
		break;
	  }

	  content[readLen] = 0;
	  printf("%s", content);
	}

	SHELL_FREE(content);
  }

  f_close(&file);

  SHELL_FREE(buffer);
}
SHELL_EXPORT_CMD(
	SHELL_CMD_PERMISSION(0) | SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC)| SHELL_CMD_RESERVE_CHAR | SHELL_CMD_DISABLE_RETURN,
	cat, shellCAT, read
		file);

/**
 * brief : rename path
 * @param dirname
 */
void shellMV(char *oldPath, char *newPath) {
  FRESULT res;
  char *buffer;
  Shell *shell = shellGetCurrent();
  res = f_rename(oldPath, newPath);
  if (res != FR_OK) {
	shellWriteString(shell, "error: ");
	shellWriteString(shell, oldPath);
	shellWriteString(shell, " rename file failed\r\n");
  }
}
SHELL_EXPORT_CMD(
	SHELL_CMD_PERMISSION(0) | SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC) | SHELL_CMD_RESERVE_CHAR | SHELL_CMD_DISABLE_RETURN,
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
