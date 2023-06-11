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
// Created by 86189 on 2023/5/21.
//

#ifndef MINIFOC_F4_DEFS_H
#define MINIFOC_F4_DEFS_H
#include "types.h"
#include "compiler.h"

#ifndef __DEQUALIFY
#define __DEQUALIFY(type, var) ((type)(uintptr_t)(const volatile void *)(var))
#endif

#ifndef offsetof
#define offsetof(type, field) \
	((size_t)(uintptr_t)((const volatile void *)&((type *)0)->field))
#endif

#ifndef __offsetof
#define __offsetof(type, field)	offsetof(type, field)
#endif

#ifndef __containerof
#define __containerof(ptr, type, field) \
	__DEQUALIFY(type *, (const volatile char *)(ptr) - offsetof(type, field))
#endif

#ifndef container_of
#define container_of(ptr, type, field) __containerof(ptr, type, field)
#endif


/*
 * Definitions for byte order, according to byte significance from low
 * address to high.
 */
#define _LITTLE_ENDIAN  1234    /* LSB first: i386, vax */
#define _BIG_ENDIAN     4321    /* MSB first: 68000, ibm, net */

#define LITTLE_ENDIAN   _LITTLE_ENDIAN
#define BIG_ENDIAN      _BIG_ENDIAN

#define _BYTE_ORDER     _LITTLE_ENDIAN
#define BYTE_ORDER      _BYTE_ORDER


#endif //MINIFOC_F4_DEFS_H
