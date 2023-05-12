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
#include <stdio.h>

#ifndef MINIFOC_F4_VFS_H
#define MINIFOC_F4_VFS_H

#define  VFS_NAME_MAX 255

#define VFS_PATH_SDCARD "sdcard"
#define VFS_PATH_SYSTEM "system"
#define VFS_PATH_USER "user"
#define VFS_PATH_DATA "data"
#define VFS_PATH_LOG "log"

enum {
    VFS_RES_OK = 0,
    VFS_RES_ERR,
    VFS_RES_NOT_FOUND,
    VFS_RES_OUT_OF_MEM,
    VFS_RES_INVALID_PARAM,
    VFS_RES_INVALID_FILE,
    VFS_RES_INVALID_PATH,
    VFS_RES_INVALID_NAME,
    VFS_RES_INVALID_OBJECT,
    VFS_RES_INVALID_DRIVE,
    VFS_RES_DENIED,
    VFS_RES_EXIST,
    VFS_RES_WRITE_PROTECTED,
    VFS_RES_NOT_ENABLED,
    VFS_RES_NO_FILESYSTEM,
    VFS_RES_MKFS_ABORTED,
    VFS_RES_TIMEOUT,
    VFS_RES_LOCKED,
    VFS_RES_NOT_ENOUGH_CORE,
    VFS_RES_TOO_MANY_OPEN_FILES,
    VFS_RES_INVALID_SEQUENCE,
    VFS_RES_EOF,
    VFS_RES_STR_ERR,
    VFS_RES_DISK_ERR,
    VFS_RES_INT_ERR,
    VFS_RES_NOT_SUPPORTED,
    VFS_RES_WRITE_FAULT,
    VFS_RES_READ_FAULT,
    VFS_RES_NOT_READY,
    VFS_RES_NO_VOLUME,
    VFS_RES_PART_ERR,
    VFS_RES_INVALID_STATE,
    VFS_RES_BUFFER_OVERFLOW,
    VFS_RES_INVALID_ADDRESS,
    VFS_RES_NOT_ENABLED_PARTITION,
    VFS_RES_NO_FILESYSTEM_SUPPORT,
};

typedef uint8_t vfsRes_t;


enum {
    VFS_MODE_READ = 0x01,
    VFS_MODE_WRITE = 0x02,
    VFS_MODE_APPEND = 0x04,
    VFS_MODE_CREATE = 0x08,
    VFS_MODE_TRUNC = 0x10,
};

typedef uint8_t vfsMode_t;

enum {
    VFS_SEEK_SET = 0,
    VFS_SEEK_CUR = 1,
    VFS_SEEK_END = 2,
};

typedef uint8_t vfsSeekMode_t;

typedef struct {
    size_t size;
    uint8_t  isDir;
    uint32_t modifiedTime;
    // Type of the file, either LFS_TYPE_REG or LFS_TYPE_DIR
    uint8_t type;
    char name[VFS_NAME_MAX+1];
}vfsFileInfo_t;


typedef struct {
    char curPath[VFS_NAME_MAX+1];
    vfsRes_t (*open)(void *file, const char *path, vfsMode_t flags);
    vfsRes_t (*close)(void *file);
    vfsRes_t (*read)(void *file, void *buf, uint32_t len, uint32_t *readLen);
    vfsRes_t (*write)(void *file, const void *buf, uint32_t len, uint32_t *writeLen);
    vfsRes_t (*truncate)(void *file, uint32_t size);
    vfsRes_t (*sync)(void *file);
    vfsRes_t (*seek)(void *file, int32_t offset, vfsSeekMode_t whence);
    vfsRes_t (*tell)(void *file, uint32_t *offset);
    vfsRes_t (*stat)(void *file,const char *path, vfsFileInfo_t *fileInfo);
    vfsRes_t (*size)(void *file, uint32_t *size);
    vfsRes_t (*unlink)(void *file, const char *path);
    vfsRes_t (*rename)(void *dir, const char *oldPath, const char *newPath);
    vfsRes_t (*mkdir)(void *dir, const char *path);
    vfsRes_t (*rmdir)(void *dir, const char *path);
    vfsRes_t (*opendir)(void *dir, const char *path);
    vfsRes_t (*readdir)(void *dir, vfsFileInfo_t *fileInfo);
    vfsRes_t (*closedir)(void *dir);
    vfsRes_t (*chdir)(void *dir, const char *path);
    vfsRes_t (*getcwd)(void *dir, char *buffer, size_t bufferSize);
}vfsDrv_t;


typedef struct {
    void *handle;
    void *file;
    vfsDrv_t *fsDrv;
    const char *path;
}vfsFile_t,vfsDir_t;

#define VFS_MAX_FILESYSTEM_COUNT 10
#define VFS_MAX_FILESYSTEM_NAME_LENGTH 32

typedef struct {
    char fsLetter[VFS_MAX_FILESYSTEM_NAME_LENGTH];
    void *fsDrv;
    void *fHandle;
} vfsFileSystem_t;

typedef struct {
    char curPath[VFS_NAME_MAX+1];
    uint8_t fsCount;
    vfsFileSystem_t *fsList;
} vfsManager_t;


/**
 * The function initializes and registers various file systems for the virtual file system.
 * 
 * @return an integer value of 0.
 */
int vfsSystemInit();

/**
 * @brief Get file system count
 * @return
 */
size_t vfsGetFileSystemCount(void);

/**
 * @brief Get file system by index
 * @param index
 * @return file system handle
 */
vfsFileSystem_t *vfsGetFileSystem(uint8_t index);

/**
 * @brief Set current path
 * @param path
 */
void vfsSetCurrentPath(const char *path);

/**
 * @brief Get current path
 * @return
 */
char * vfsGetCurrentPath(void);

/**
 * @brief Get real path
 * @param path
 * @return
 */
char *vfsGetRealPath(char *path);
/**
 * The function searches for a file system driver based on the file path and opens the file using the
 * appropriate driver.
 * 
 * @param file A pointer to a vfsFile_t structure that will be filled with information about the opened
 * file.
 * @param path A string representing the path of the file to be opened.
 * @param flags vfsMode_t flags is a parameter that specifies the mode in which the file should be
 * opened. It is an enumeration type that can take values such as VFS_MODE_READ, VFS_MODE_WRITE,
 * VFS_MODE_APPEND, etc. These flags determine the access permissions and behavior of the file when it
 * 
 * @return a value of type `vfsRes_t`, which is an enumerated type representing the result of the
 * operation. The possible values of `vfsRes_t` are implementation-specific, but one possible value
 * being returned in this case is `VFS_RES_NOT_FOUND`, indicating that the file specified by `path`
 * could not be found in any of the mounted file systems.
 */
vfsRes_t vfsOpen(vfsFile_t *file, const char *path, vfsMode_t flags);


/**
 * The function closes a file by iterating through a list of file systems and calling the close
 * function of the corresponding file system driver.
 * 
 * @param file A pointer to a structure representing a file in the virtual file system.
 * 
 * @return a value of type `vfsRes_t`.
 */

vfsRes_t vfsClose(vfsFile_t *file);

/**
 * The function reads data from a file using the appropriate file system driver.
 * 
 * @param file A pointer to a vfsFile_t structure that represents the file to be read.
 * @param buf A pointer to the buffer where the data read from the file will be stored.
 * @param len len is the length of the data to be read from the file.
 * @param read The "read" parameter is a pointer to a uint32_t variable that will be used to store the
 * number of bytes actually read from the file. The function will update this variable with the number
 * of bytes read before returning.
 * 
 * @return a vfsRes_t type, which is an enumerated type representing the result of the read operation.
 * It could be VFS_RES_OK if the read was successful, or VFS_RES_NOT_FOUND if the file system driver
 * was not found.
 */
vfsRes_t vfsRead(vfsFile_t *file, void *buf, uint32_t len, uint32_t *read);

/**
 * The function writes data to a file in a virtual file system.
 * 
 * @param file A pointer to a vfsFile_t structure representing the file to be written to.
 * @param buf buf is a pointer to the buffer containing the data to be written to the file.
 * @param len len is the length of the data to be written to the file.
 * @param written The "written" parameter is a pointer to a uint32_t variable that will be used to
 * store the number of bytes actually written to the file during the write operation. The function will
 * update this variable before returning.
 * 
 * @return a vfsRes_t type, which is an enumerated type representing the result of the operation. It
 * could be VFS_RES_OK if the write operation was successful, or VFS_RES_NOT_FOUND if the file system
 * driver was not found.
 */
vfsRes_t vfsWrite(vfsFile_t *file, const void *buf, uint32_t len, uint32_t *written);

/**
 * This function searches for a file system driver and calls its seek function with the given
 * parameters.
 * 
 * @param file A pointer to a vfsFile_t structure representing the file to seek within.
 * @param pos pos is the position in the file where the seek operation should be performed. It is an
 * unsigned 32-bit integer value.
 * @param whence The parameter "whence" is a value of type "vfsSeekMode_t" which specifies the
 * reference point used for seeking within the file. It can take one of the following values:
 * 
 * @return a value of type vfsRes_t, which is an enumerated type representing various possible results
 * of a file system operation. The specific value being returned depends on the outcome of the
 * operation performed by the seek function of the file system driver associated with the file. If the
 * file system driver is found and the seek operation is successful, the function will return a value
 * indicating success. If the file
 */
vfsRes_t vfsSeek(vfsFile_t *file, uint32_t pos, vfsSeekMode_t whence) ;

/**
 * The function returns the current position of a file in a virtual file system.
 * 
 * @param file A pointer to a vfsFile_t structure representing the file being queried for its current
 * position.
 * @param pos A pointer to a uint32_t variable that will be used to store the current position of the
 * file pointer. The function will update this variable with the current position.
 * 
 * @return a value of type `vfsRes_t`, which is an enumerated type representing the result of the VFS
 * operation. The possible values of `vfsRes_t` are defined by the VFS_RES_* macros.
 */
vfsRes_t vfsTell(vfsFile_t *file, uint32_t *pos) ;

/**
 * The function retrieves the size of a file from the appropriate file system driver.
 * 
 * @param file A pointer to a vfsFile_t structure representing the file whose size is being queried.
 * @param size A pointer to a uint32_t variable where the size of the file will be stored. The function
 * will set this variable to the size of the file in bytes.
 * 
 * @return a value of type vfsRes_t, which is an enumerated type representing various possible results
 * of a file system operation. The specific value being returned depends on the outcome of the
 * operation performed by the function.
 */
vfsRes_t vfsSize(vfsFile_t *file, uint32_t *size);

/**
 * The function truncates a file to a specified length using the appropriate file system driver.
 * 
 * @param file A pointer to a vfsFile_t structure representing the file to be truncated.
 * @param length The length parameter in the vfsTruncate function is an unsigned 32-bit integer that
 * represents the new size of the file after truncation. This function is used to truncate or resize a
 * file to a specified length.
 * 
 * @return a value of type `vfsRes_t`, which is an enumerated type representing various possible
 * results of the function. The specific value being returned depends on the outcome of the function's
 * execution. If the function successfully truncates the file, it will return `VFS_RES_OK`. If the file
 * system driver is not found, it will return `VFS_RES_NOT_FOUND`.
 */
vfsRes_t vfsTruncate(vfsFile_t *file, uint32_t length);

/**
 * The function `vfsSync` synchronizes a file with its corresponding file system driver.
 * 
 * @param file A pointer to a structure representing a file in the virtual file system.
 * 
 * @return a value of type `vfsRes_t`, which is an enumerated type representing various possible
 * results of a file system operation. The specific value being returned depends on the result of
 * calling the `sync` function of the file system driver associated with the given file. If the
 * driver's `sync` function returns successfully, the function returns `VFS_RES_OK`. If the driver is
 * not
 */
vfsRes_t vfsSync(vfsFile_t *file) ;

/**
 * The function searches for a file in a list of file systems and returns its information if found.
 * 
 * @param path A string representing the path of the file or directory to be checked for its status.
 * @param st A pointer to a vfsFileInfo_t structure that will be filled with information about the file
 * or directory specified by the path parameter.
 * 
 * @return a value of type vfsRes_t, which is an enumerated type representing the result of the VFS
 * operation. The specific value being returned depends on the outcome of the function's execution. If
 * the function successfully retrieves information about the file specified by the path argument, it
 * will return VFS_RES_OK. If the file is not found, it will return VFS_RES_NOT_FOUND.
 */
vfsRes_t vfsStat(const char *path, vfsFileInfo_t *st) ;

/**
 * The function `vfsUnlink` searches for a file system in a list and calls the `unlink` function of the
 * corresponding driver to delete a file.
 * 
 * @param path A string representing the path of the file to be unlinked (deleted) from the virtual
 * file system.
 * 
 * @return a value of type `vfsRes_t`, which is a custom-defined enumeration type representing the
 * result of the unlink operation. The possible values of `vfsRes_t` are implementation-specific, but
 * typically include success, failure, and various error conditions.
 */
vfsRes_t vfsUnlink(const char *path);

/**
 * The function renames a file in a virtual file system.
 * 
 * @param oldName A pointer to a string containing the name of the file or directory to be renamed.
 * @param newName The new name that the file or directory will be renamed to.
 * 
 * @return a value of type `vfsRes_t`.
 */
vfsRes_t vfsRename(const char *oldName, const char *newName) ;

/**
 * The function checks for the file system type of a given path and calls the corresponding mkdir
 * function from the appropriate driver.
 * 
 * @param path A string representing the path of the directory to be created in the virtual file
 * system.
 * 
 * @return a value of type `vfsRes_t`, which is a custom-defined enumeration type representing the
 * result of the operation. The possible values of `vfsRes_t` are implementation-specific, but
 * typically include success, failure, and various error conditions.
 */
vfsRes_t vfsMkdir(const char *path);

/**
 * The function removes a directory from a virtual file system.
 * 
 * @param path A string representing the path of the directory to be removed from the virtual file
 * system.
 * 
 * @return a value of type `vfsRes_t`, which is a custom-defined enumeration type representing the
 * result of the operation. The possible values of `vfsRes_t` are implementation-specific, but
 * typically include success, failure, and various error conditions.
 */
vfsRes_t vfsRemoveDir(const char *path) ;

/**
 * The function opens a directory in a virtual file system by searching for the appropriate file system
 * driver and calling its opendir function.
 * 
 * @param dir a pointer to a structure representing a directory in the virtual file system
 * @param path A string representing the path of the directory to be opened.
 * 
 * @return The function `vfsDirOpen` returns a `vfsRes_t` type, which is an enumerated type
 * representing the result of the operation. It can be one of several values, including `VFS_RES_OK` if
 * the operation was successful, or `VFS_RES_NOT_FOUND` if the specified path was not found.
 */
vfsRes_t vfsDirOpen(vfsDir_t *dir, const char *path) ;

/**
 * The function reads a directory and returns information about the files in it.
 * 
 * @param dir A pointer to a vfsDir_t structure representing the directory to read from.
 * @param fileInfo A pointer to a structure of type vfsFileInfo_t, which will be filled with
 * information about the next file in the directory when this function is called.
 * 
 * @return a value of type `vfsRes_t`, which is an enumerated type representing the result of the
 * directory read operation. The possible return values are `VFS_RES_OK` (if the operation was
 * successful), `VFS_RES_NOT_FOUND` (if the directory or file was not found), `VFS_RES_DENIED` (if the
 * operation was denied due to permissions), `V
 */
vfsRes_t vfsDirRead(vfsDir_t *dir, vfsFileInfo_t *fileInfo) ;

/**
 * The function closes a directory in a file system.
 * 
 * @param dir A pointer to a structure representing an open directory in the virtual file system.
 * 
 * @return a value of type `vfsRes_t`, which is an enumerated type representing various possible
 * results of a VFS operation. The specific value being returned depends on the outcome of the
 * function's execution. If the directory was successfully closed, the function will return
 * `VFS_RES_OK`. If the directory was not found or there was an error closing it, the function will
 * return `V
 */
vfsRes_t vfsDirClose(vfsDir_t *dir);



/**
 * The function changes the current working directory to the specified path in a virtual file system.
 * 
 * @param path A string representing the path of the directory to change to.
 * 
 * @return a vfsRes_t type, which is an enumerated type representing the result of the operation. It
 * could be VFS_RES_NOT_SUPPORTED if the chdir operation is not supported by the file system driver,
 * VFS_RES_NOT_FOUND if the path is not found, or another value indicating success or failure.
 */
vfsRes_t vfsChdir(const char *path);


/**
 * The function searches for a file system driver that supports the getcwd function and calls it to get
 * the current working directory.
 * 
 * @param buffer A pointer to a character array where the current working directory path will be
 * stored.
 * @param maxSize The maximum size of the buffer that will be used to store the current working
 * directory path.
 * 
 * @return a vfsRes_t, which is a typedef for an integer representing the result of the operation. It
 * could be VFS_RES_OK if the operation was successful, or an error code such as VFS_RES_NOT_FOUND if
 * the current working directory could not be found.
 */
vfsRes_t vfsGetcwd(char *buffer, size_t maxSize);
#endif //MINIFOC_F4_VFS_H
