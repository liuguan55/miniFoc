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
// Created by 86189 on 2023/5/22.
//

#ifndef MINIFOC_F4_BOARD_DEBUG_H
#define MINIFOC_F4_BOARD_DEBUG_H


#include <stdio.h>
#include "sys/MiniUtil.h"

#ifdef __cplusplus
extern "C" {
#endif

#define BOARD_DBG_ON    1
#define BOARD_WRN_ON    1
#define BOARD_ERR_ON    1
#define BOARD_ABORT_ON  0

#define BOARD_SYSLOG    printf
#define BOARD_ABORT()   sys_abort()

#define BOARD_LOG(flags, fmt, arg...)   \
	do {                                \
		if (flags)                      \
			BOARD_SYSLOG(fmt, ##arg);   \
	} while (0)

#define BOARD_DBG(fmt, arg...) BOARD_LOG(BOARD_DBG_ON, "[board] "fmt, ##arg)
#define BOARD_WRN(fmt, arg...) BOARD_LOG(BOARD_WRN_ON, "[board WRN] "fmt, ##arg)

#define BOARD_ERR(fmt, arg...)                              \
	do {                                                    \
		BOARD_LOG(BOARD_ERR_ON, "[board ERR] %s():%d, "fmt, \
		          __func__, __LINE__, ##arg);               \
		if (BOARD_ABORT_ON)                                 \
			BOARD_ABORT();                                  \
	} while (0)

#ifdef __cplusplus
}
#endif

#endif //MINIFOC_F4_HAL_DEBUG_H
