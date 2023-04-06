/*
 * @Author: liuguan
 * @Date: 2022-06-14 15:55:19
 * @LastEditors: liuguan
 * @LastEditTime: 2022-06-14 15:55:21
 * @FilePath: /esp8266-rtos/src/espShell.h
 * @Description: 
 * 
 * Copyright (c) 2022 by liuguan, All Rights Reserved. 
 */
/*
 * @Author: liuguan
 * @Date: 2022-06-14 15:55:19
 * @LastEditors: liuguan
 * @LastEditTime: 2022-06-14 15:55:20
 * @FilePath: /esp8266-rtos/src/espShell.h
 * @Description: 
 * 
 * Copyright (c) 2022 by liuguan, All Rights Reserved. 
 */
// Copyright 2022 dengzhijun
// 
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// 
//     http://www.apache.org/licenses/LICENSE-2.0
// 
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
#pragma once
#include "lettershell/shell.h"
#include "lettershell/shell_fs.h"
#include "lettershell/log.h"

void MiniShell_Init(void);

void MiniShell_Run(void);