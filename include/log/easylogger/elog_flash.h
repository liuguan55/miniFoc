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

#ifndef __ELOG_FLASH_H__
#define __ELOG_FLASH_H__

#include "elog.h"
#include "elog_flash_cfg.h"

#ifdef __cplusplus
extern "C" {
#endif

#if !defined(ELOG_FLASH_BUF_SIZE)
    #error "Please configure RAM buffer size (in elog_flash_cfg.h)"
#endif

/* EasyLogger flash log plugin's software version number */
#define ELOG_FLASH_SW_VERSION                "V2.0.1"

/* elog_flash.c */
ElogErrCode elog_flash_init(void);
void elog_flash_output(size_t pos, size_t size);
void elog_flash_output_all(void);
void elog_flash_output_recent(size_t size);
void elog_flash_set_filter(uint8_t level,const char *tag,const char *keyword);
void elog_flash_write(const char *log, size_t size);
void elog_flash_clean(void);
void elog_flash_lock_enabled(bool enabled);

#ifdef ELOG_FLASH_USING_BUF_MODE
void elog_flash_flush(void);
#endif

/* elog_flash_port.c */
ElogErrCode elog_flash_port_init(void);
void elog_flash_port_output(const char *log, size_t size);
void elog_flash_port_lock(void);
void elog_flash_port_unlock(void);

#ifdef __cplusplus
}
#endif

#endif /* __ELOG_FLASH_H__ */
