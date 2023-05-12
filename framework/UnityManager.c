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
// Created by 86189 on 2023/5/2.
//
#include <stdlib.h>
#include "unity/unity.h"
#include "UnityManager.h"


typedef struct {
    unity_func_t *funcs;
    uint16_t count;
} UnityManager_t;

static UnityManager_t unityManager = {0};

void setUp(void) {
    // set stuff up here
    printf("\r\nsetUp\n");
}

void tearDown(void) {
    // clean stuff up here
    printf("\r\ntearDown\n");
}

void UnityManager_init(void) {
    unityManager.count = 0;
}

void UnityManager_add(unity_func_t func) {
    unityManager.count++;
    unityManager.funcs = (unity_func_t *) realloc(unityManager.funcs,
                                                  unityManager.count * sizeof(unity_func_t));
    unityManager.funcs[unityManager.count - 1] = func;
}

void UnityManager_run(void) {
    for (int i = 0; i < unityManager.count; i++) {
        unityManager.funcs[i]();
    }
}
