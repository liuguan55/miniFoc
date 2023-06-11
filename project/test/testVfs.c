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
#include <stdlib.h>
#include <string.h>
#include "fs/vfs/vfs.h"
#include "common/framework/UnityManager.h"

void testSdcard(){
    vfsRes_t res;
    vfsFile_t file;

    res = vfsOpen(&file, VFS_PATH_SDCARD"/test.txt", VFS_MODE_CREATE | VFS_MODE_WRITE);
    TEST_ASSERT_EQUAL(VFS_RES_OK, res);
    uint32_t writeLen = 0;
    res = vfsWrite(&file, "hello world", 11, &writeLen);
    TEST_ASSERT_EQUAL(VFS_RES_OK, res);
    TEST_ASSERT_EQUAL(11, writeLen);

    res = vfsClose(&file);
    TEST_ASSERT_EQUAL(VFS_RES_OK, res);

    res = vfsOpen(&file, VFS_PATH_SDCARD"/test.txt",  VFS_MODE_READ);
    TEST_ASSERT_EQUAL(VFS_RES_OK, res);
    uint32_t readLen = 0;
    char buf[12] = {0};
    res = vfsRead(&file, buf, 11, &readLen);
    TEST_ASSERT_EQUAL(VFS_RES_OK, res);
    TEST_ASSERT_EQUAL(11, readLen);
    TEST_ASSERT_EQUAL_STRING("hello world", buf);

    res = vfsTell(&file, &readLen);
    TEST_ASSERT_EQUAL(VFS_RES_OK, res);
    TEST_ASSERT_EQUAL(11, readLen);

    res = vfsSeek(&file, 0, VFS_SEEK_SET);
    TEST_ASSERT_EQUAL(VFS_RES_OK, res);

    res = vfsTell(&file, &readLen);
    TEST_ASSERT_EQUAL(VFS_RES_OK, res);
    TEST_ASSERT_EQUAL(0, readLen);

    res = vfsSeek(&file, 0, VFS_SEEK_END);
    TEST_ASSERT_EQUAL(VFS_RES_OK, res);

    res = vfsTell(&file, &readLen);
    TEST_ASSERT_EQUAL(VFS_RES_OK, res);
    TEST_ASSERT_EQUAL(11, readLen);

    res = vfsSize(&file, &readLen);
    TEST_ASSERT_EQUAL(VFS_RES_OK, res);
    TEST_ASSERT_EQUAL(11, readLen);

    vfsFileInfo_t info;
    res = vfsStat(VFS_PATH_SDCARD"/test.txt", &info);
    TEST_ASSERT_EQUAL(VFS_RES_OK, res);
    TEST_ASSERT_EQUAL(11, info.size);

    res = vfsSync(&file);
    TEST_ASSERT_EQUAL(VFS_RES_OK, res);

    res = vfsClose(&file);
    TEST_ASSERT_EQUAL(VFS_RES_OK, res);

    res = vfsDirOpen(&file, VFS_PATH_SDCARD"/qwe");
    TEST_ASSERT_EQUAL(VFS_RES_OK, res);
    res = vfsDirRead(&file, &info);
    TEST_ASSERT_EQUAL(VFS_RES_OK, res);
    res = vfsDirClose(&file);
    TEST_ASSERT_EQUAL(VFS_RES_OK, res);
}

void testSystem(){
    vfsRes_t res = VFS_RES_OK;
    vfsFile_t file;

    res = vfsOpen(&file, VFS_PATH_SYSTEM"/test.txt", VFS_MODE_CREATE | VFS_MODE_WRITE);
    TEST_ASSERT_EQUAL(VFS_RES_OK, res);
    uint32_t writeLen = 0;
    res = vfsWrite(&file, "hello world", 11, &writeLen);
    TEST_ASSERT_EQUAL(VFS_RES_OK, res);
    TEST_ASSERT_EQUAL(11, writeLen);

    res = vfsClose(&file);
    TEST_ASSERT_EQUAL(VFS_RES_OK, res);

    res = vfsOpen(&file, VFS_PATH_SYSTEM"/test.txt",  VFS_MODE_READ);
    TEST_ASSERT_EQUAL(VFS_RES_OK, res);
    uint32_t readLen = 0;
    char buf[12] = {0};
    res = vfsRead(&file, buf, 11, &readLen);
    TEST_ASSERT_EQUAL(VFS_RES_OK, res);
    TEST_ASSERT_EQUAL(11, readLen);
    TEST_ASSERT_EQUAL_STRING("hello world", buf);

    res = vfsTell(&file, &readLen);
    TEST_ASSERT_EQUAL(VFS_RES_OK, res);
    TEST_ASSERT_EQUAL(11, readLen);

    res = vfsSeek(&file, 0, VFS_SEEK_SET);
    TEST_ASSERT_EQUAL(VFS_RES_OK, res);

    res = vfsTell(&file, &readLen);
    TEST_ASSERT_EQUAL(VFS_RES_OK, res);
    TEST_ASSERT_EQUAL(0, readLen);

    res = vfsSeek(&file, 0, VFS_SEEK_END);
    TEST_ASSERT_EQUAL(VFS_RES_OK, res);

    res = vfsTell(&file, &readLen);
    TEST_ASSERT_EQUAL(VFS_RES_OK, res);
    TEST_ASSERT_EQUAL(11, readLen);

    res = vfsSize(&file, &readLen);
    TEST_ASSERT_EQUAL(VFS_RES_OK, res);
    TEST_ASSERT_EQUAL(11, readLen);

    vfsFileInfo_t info;
    res = vfsStat(VFS_PATH_SYSTEM"/test.txt", &info);
    TEST_ASSERT_EQUAL(VFS_RES_OK, res);
    TEST_ASSERT_EQUAL(11, info.size);

    res = vfsSync(&file);
    TEST_ASSERT_EQUAL(VFS_RES_OK, res);

    res = vfsClose(&file);
    TEST_ASSERT_EQUAL(VFS_RES_OK, res);

//    res = vfsMkdir(VFS_PATH_SYSTEM"qwe");
//    TEST_ASSERT_EQUAL(VFS_RES_OK, res);
    res = vfsDirOpen(&file, VFS_PATH_SYSTEM"/qwe");
    TEST_ASSERT_EQUAL(VFS_RES_OK, res);
    res = vfsDirRead(&file, &info);
    TEST_ASSERT_EQUAL(VFS_RES_OK, res);
    res = vfsDirClose(&file);
    TEST_ASSERT_EQUAL(VFS_RES_OK, res);
//
//    res = vfsRemoveDir(VFS_PATH_SYSTEM"qwe");
//    TEST_ASSERT_EQUAL(VFS_RES_OK, res);
}

void testData(){
    vfsRes_t res = VFS_RES_OK;
    vfsFile_t file;

    res = vfsOpen(&file, VFS_PATH_DATA"/test.txt", VFS_MODE_CREATE | VFS_MODE_WRITE);
    TEST_ASSERT_EQUAL(VFS_RES_OK, res);
    uint32_t writeLen = 0;
    res = vfsWrite(&file, "hello world", 11, &writeLen);
    TEST_ASSERT_EQUAL(VFS_RES_OK, res);
    TEST_ASSERT_EQUAL(11, writeLen);

    res = vfsClose(&file);
    TEST_ASSERT_EQUAL(VFS_RES_OK, res);

    res = vfsOpen(&file, VFS_PATH_DATA"/test.txt",  VFS_MODE_READ);
    TEST_ASSERT_EQUAL(VFS_RES_OK, res);
    uint32_t readLen = 0;
    char buf[12] = {0};
    res = vfsRead(&file, buf, 11, &readLen);
    TEST_ASSERT_EQUAL(VFS_RES_OK, res);
    TEST_ASSERT_EQUAL(11, readLen);
    TEST_ASSERT_EQUAL_STRING("hello world", buf);

    res = vfsTell(&file, &readLen);
    TEST_ASSERT_EQUAL(VFS_RES_OK, res);
    TEST_ASSERT_EQUAL(11, readLen);

    res = vfsSeek(&file, 0, VFS_SEEK_SET);
    TEST_ASSERT_EQUAL(VFS_RES_OK, res);

    res = vfsTell(&file, &readLen);
    TEST_ASSERT_EQUAL(VFS_RES_OK, res);
    TEST_ASSERT_EQUAL(0, readLen);

    res = vfsSeek(&file, 0, VFS_SEEK_END);
    TEST_ASSERT_EQUAL(VFS_RES_OK, res);

    res = vfsTell(&file, &readLen);
    TEST_ASSERT_EQUAL(VFS_RES_OK, res);
    TEST_ASSERT_EQUAL(11, readLen);

    res = vfsSize(&file, &readLen);
    TEST_ASSERT_EQUAL(VFS_RES_OK, res);
    TEST_ASSERT_EQUAL(11, readLen);

    vfsFileInfo_t info;
    res = vfsStat(VFS_PATH_DATA"/test.txt", &info);
    TEST_ASSERT_EQUAL(VFS_RES_OK, res);
    TEST_ASSERT_EQUAL(11, info.size);

    res = vfsSync(&file);
    TEST_ASSERT_EQUAL(VFS_RES_OK, res);

    res = vfsClose(&file);
    TEST_ASSERT_EQUAL(VFS_RES_OK, res);

    res = vfsMkdir(VFS_PATH_DATA"/qwe");
    TEST_ASSERT_EQUAL(VFS_RES_OK, res);
    res = vfsDirOpen(&file, VFS_PATH_DATA"/qwe");
    TEST_ASSERT_EQUAL(VFS_RES_OK, res);
    res = vfsDirRead(&file, &info);
    TEST_ASSERT_EQUAL(VFS_RES_OK, res);
    res = vfsDirClose(&file);
    TEST_ASSERT_EQUAL(VFS_RES_OK, res);

    res = vfsRemoveDir(VFS_PATH_DATA"/qwe");
    TEST_ASSERT_EQUAL(VFS_RES_OK, res);
}

static void test_run(){
    UNITY_BEGIN();
    RUN_TEST(testSdcard);
    RUN_TEST(testSystem);
    RUN_TEST(testData);
    UNITY_END();
}

UnityManager_add_test(test_run)
