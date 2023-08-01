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
// Created by 86189 on 2023/4/29.
//

#ifndef MINIFOC_F4_MINIDEBUG_H
#define MINIFOC_F4_MINIDEBUG_H
#include <stdlib.h>
#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include "sys/MiniUtil.h"

#define MINI_DEBUG_ON


/*
 * @brief Debug level
 */
#define DBG_LEVEL_MASK (0x0F)

#define MINI_LEVEL_EMERG 0

#define MINI_LEVEL_ALERT 1

#define MINI_LEVEL_CRIT 2

#define MINI_LEVEL_ERROR 3

#define MINI_LEVEL_WARNING 4

#define MINI_LEVEL_NOTICE 5

#define MINI_LEVEL_INFO 6

#define MINI_LEVEL_DEBUG 7

#define MINI_LEVEL_ALL 0x0F


/*
 * No expanded condition
 */
#define NOEXPAND 1


/*
 * module ON/OFF
 */
#define DBG_ON (1 << 4)

#define DBG_OFF (0)


/*
 * Always show message
 */
#define MOD_DBG_ALW_ON (DBG_ON | MINI_LEVEL_ALL)


/************************************************************
 * MINI_DEBUG INTERFACE
 ************************************************************/
#ifdef MINI_DEBUG_ON

#define MINI_DEBUG_PRINT(msg, arg...)  printf(msg, ##arg)

#define MINI_DEBUG_ABORT()    \
	do { \
		printf("system aborted!"); \
		sys_abort(); \
	} while (0)


#define _MINI_DEBUG(modules, dlevel, expand, msg, arg...) \
		do { \
			if ( \
				((modules) & DBG_ON) && \
				(((modules) & DBG_LEVEL_MASK) >= dlevel) && \
				(expand)) { \
				MINI_DEBUG_PRINT(msg, ##arg); \
			} \
		} while (0)


#define _MINI_INFO(modules, dlevel, expand, msg, arg...) \
		do { \
			if ( \
				(((int16_t)(modules) & DBG_LEVEL_MASK) >= dlevel) && \
				(expand)) { \
				MINI_DEBUG_PRINT(msg, ##arg); \
			} \
		} while (0)


#define _MINI_ASSERT(assert, modules, dlevel, msg, arg...) \
	_MINI_DEBUG(modules, dlevel, !(assert), \
	          "[Assert] At %s line %d fun %s: " msg, \
	          __FILE__, __LINE__, __func__, ##arg)


#define _MINI_ASSERT_ABORT(assert, modules, dlevel, msg, arg...)    \
	do { \
		if ((((int16_t)(modules) & DBG_LEVEL_MASK) >= dlevel) && !(assert)) { \
			MINI_DEBUG_PRINT("[Assert] At %s line %d fun %s: " msg, \
			               __FILE__, __LINE__, __func__, ##arg); \
			MINI_DEBUG_ABORT(); \
		} \
	} while (0)


#define MINI_ERROR(modules, expand, msg, arg...) \
	_MINI_DEBUG(modules, MINI_LEVEL_ERROR, expand, msg, ##arg)

#define MINI_ALERT(modules, expand, msg, arg...) \
	_MINI_DEBUG(modules, MINI_LEVEL_ALERT, expand, msg, ##arg)

#define MINI_CRIT(modules, expand, msg, arg...) \
	_MINI_DEBUG(modules, MINI_LEVEL_CRIT, expand, msg, ##arg)

#define MINI_EMERG(modules, expand, msg, arg...) \
	_MINI_DEBUG(modules, MINI_LEVEL_EMERG, MINI_xpand, msg, ##arg)

#define MINI_WARN(modules, expand, msg, arg...) \
	_MINI_DEBUG(modules, MINI_LEVEL_WARNING, expand, msg, ##arg)

#define MINI_NOTICE(modules, expand, msg, arg...) \
	_MINI_DEBUG(modules, MINI_LEVEL_NOTICE, expand, msg, ##arg)

#define MINI_INFO(modules, expand, msg, arg...) \
	_MINI_DEBUG(modules, MINI_LEVEL_INFO, expand, msg, ##arg)

#define MINI_DEBUG(modules, expand, msg, arg...) \
	_MINI_DEBUG(modules, MINI_LEVEL_DEBUG, expand, msg, ##arg)


#define MINI_ASSERT(assert, modules, msg, arg...) \
	_MINI_ASSERT(assert, modules, MINI_LEVEL_ALL, msg, ##arg)

#define MINI_ASSERT_ABORT(assert, modules, msg, arg...) \
	_MINI_ASSERT_ABORT(assert, modules, MINI_LEVEL_ALL, msg, ##arg)

#define  MINI_ASSERT_MSG(condition, msg) \
    do { \
        if (!(condition)) { \
            MINI_DEBUG_PRINT("[Assert] At %s line %d fun %s: " msg, \
                           __FILE__, __LINE__, __func__); \
            MINI_DEBUG_ABORT(); \
        } \
    } while (0)

#ifndef assert
#define assert(condition) MINI_ASSERT(condition, MOD_DBG_ALW_ON, "condition %s is fault.\n", #condition)
#endif

#define MINI_ENTRY(modules, mname) \
	MINI_DEBUG(modules, NOEXPAND, mname "entry %s().\n", __func__)

#define MINI_RET(modules, mname, ret) \
	MINI_DEBUG(modules, NOEXPAND, mname "exit %s() with return %d.\n", __func__, ret)

#define MINI_RET_NOVAL(modules, mname) \
	MINI_DEBUG(modules, NOEXPAND, mname "exit %s().\n", __func__)


#else /* MDEBUG_ON */

#define MINI_DEBUG_PRINT(msg, arg...)

#define MINI_DEBUG_ABORT()


#define _MINI_DEBUG(modules, dlevel, expand, msg, arg...)

#define _MINI_INFO(modules, dlevel, expand, msg, arg...)

#define _MINI_ASSERT(assert, modules, dlevel, msg, arg...)

#define _MINI_ASSERT_ABORT(assert, modules, dlevel, msg, arg...)


#define MINI_ERROR(modules, expand, msg, arg...)

#define MINI_ALERT(modules, expand, msg, arg...)

#define MINI_CRIT(modules, expand, msg, arg...)

#define MINI_EMERG(modules, expand, msg, arg...)

#define MINI_WARN(modules, expand, msg, arg...)

#define MINI_NOTICE(modules, expand, msg, arg...)

#define MINI_INFO(modules, expand, msg, arg...)

#define MINI_DEBUG(modules, expand, msg, arg...)


#define MINI_ASSERT(assert, modules, msg, arg...)

#define MINI_ASSERT_ABORT(assert, modules, msg, arg...)

#ifndef assert
#define assert(condition)
#endif


#define MINI_ENTRY(modules, mname)

#define MINI_RET(modules, mname, ret)

#define MINI_RET_NOVAL(modules, mname)

#endif /* MINI_DEBUG_ON */




#endif //MINIFOC_F4_MINIDEBUG_H
