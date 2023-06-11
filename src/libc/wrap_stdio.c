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
// Created by 86189 on 2023/5/24.
//

#include <stdarg.h>
#include <string.h>
#include "libc/stdio.h"
#include "driver/hal/hal_os.h"
#include "sys/interrupt.h"
#include "cmsis_gcc.h"

#define WRAP_STDOUT_BUF_SIZE	256 //1024

static char s_stdout_buf[WRAP_STDOUT_BUF_SIZE];

static stdio_write_fn s_stdio_write = NULL;
static HAL_Mutex s_stdout_mutex;

/* case of critical context
 *    - IRQ disabled
 *    - FIQ disabled
 *    - Execute in ISR context
 *    - Scheduler is not running
 */
static int stdio_is_critical_context(void)
{
    return (__get_PRIMASK()   ||
            __get_FAULTMASK() ||
            __get_IPSR()      ||
            !HAL_ThreadIsSchedulerRunning());
}

void stdout_mutex_lock(void)
{
    if (stdio_is_critical_context()) {
        return;
    }

    if (!s_stdout_mutex) {
        HAL_MutexInit(&s_stdout_mutex);
    }

    HAL_MutexLock(&s_stdout_mutex, HAL_OS_WAIT_FOREVER);
}

void stdout_mutex_unlock(void)
{
    if (stdio_is_critical_context()) {
        return;
    }

    if (s_stdout_mutex) {
        HAL_MutexUnlock(&s_stdout_mutex);
    }
}

void stdio_set_write(stdio_write_fn fn)
{
    stdout_mutex_lock();
    s_stdio_write = fn;
    stdout_mutex_unlock();
}

static __inline int stdio_wrap_write(char *buf, int len, int max)
{
#ifndef __CONFIG_LIBC_PRINTF_FLOAT
    /* BUG: If "__CONFIG_LIBC_PRINTF_FLOAT" is not defined, the return value
     *      of vsnprintf() is wrong when the string format including float
     *      conversion characters "efgEFG".
     * FIX: Use strlen() to calc the real length.
     */
    if (len > 0 && len <= max && buf[len] == 0 && buf[len-1] != 0) {
        /* The len seems to be correct. */
    } else {
        buf[max] = 0;
        len = strlen(buf);
    }
#endif
    return s_stdio_write(buf, len);
}

int __wrap_printf(const char *format, ...)
{
    int len;
    va_list ap;

    stdout_mutex_lock();

    if (s_stdio_write == NULL) {
        len = 0;
    } else {
        va_start(ap, format);
        len = vsnprintf(s_stdout_buf, WRAP_STDOUT_BUF_SIZE, format, ap);
        va_end(ap);
        len = stdio_wrap_write(s_stdout_buf, len, WRAP_STDOUT_BUF_SIZE - 1);
    }

    stdout_mutex_unlock();

    return len;
}

int __wrap_vprintf(const char *format, va_list ap)
{
    int len;

    stdout_mutex_lock();

    if (s_stdio_write == NULL) {
        len = 0;
    } else {
        len = vsnprintf(s_stdout_buf, WRAP_STDOUT_BUF_SIZE, format, ap);
        len = stdio_wrap_write(s_stdout_buf, len, WRAP_STDOUT_BUF_SIZE - 1);
    }

    stdout_mutex_unlock();

    return len;
}

int __wrap_puts(const char *s)
{
    int len;

    stdout_mutex_lock();

    if (s_stdio_write == NULL) {
        len = 0;
    } else {
        len = s_stdio_write(s, strlen(s));
        len += s_stdio_write("\n", 1);
    }

    stdout_mutex_unlock();

    return len;
}

int __wrap_fprintf(FILE *stream, const char *format, ...)
{
    int len;
    va_list ap;

    if (stream != stdout && stream != stderr)
        return 0;

    stdout_mutex_lock();

    if (s_stdio_write == NULL) {
        len = 0;
    } else {
        va_start(ap, format);
        len = vsnprintf(s_stdout_buf, WRAP_STDOUT_BUF_SIZE, format, ap);
        va_end(ap);
        len = stdio_wrap_write(s_stdout_buf, len, WRAP_STDOUT_BUF_SIZE - 1);
    }

    stdout_mutex_unlock();

    return len;
}

int __wrap_vfprintf(FILE *stream, const char *format, va_list ap)
{
    if (stream != stdout && stream != stderr)
        return 0;

    return __wrap_vprintf(format, ap);
}


int __wrap_fputs(const char *s, FILE *stream)
{
    if (stream != stdout && stream != stderr)
        return 0;

    return __wrap_puts(s);
}

int __wrap_putchar(int c)
{
    int len;
    char cc;

    stdout_mutex_lock();

    if (s_stdio_write == NULL) {
        len = 0;
    } else {
        cc = c;
        len = s_stdio_write(&cc, 1);
    }

    stdout_mutex_unlock();

    return len;
}

int __wrap_putc(int c, FILE *stream)
{
    if (stream != stdout && stream != stderr)
        return 0;

    return __wrap_putchar(c);
}

int __wrap_fputc(int c, FILE *stream)
{
    return __wrap_putc(c, stream);
}

int __wrap_fflush(FILE *stream)
{
    UNUSED(stream);

    return 0;
}