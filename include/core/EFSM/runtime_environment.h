/*
 * @Author: liuguan
 * @Date: 2022-07-10 21:14:13
 * @LastEditors: liuguan
 * @LastEditTime: 2022-07-10 21:14:14
 * @FilePath: /esp8266-rtos/src/EFSM/runtime_environment.h
 * @Description: 
 * 
 * Copyright (c) 2022 by liuguan, All Rights Reserved. 
 */
/*------------------------------------------------------------------
 * runtime_environment.h -- Abstractions 
 *
 * Februrary 2002, Bo Berry
 *
 * Copyright (c) 2002-2011 by Cisco Systems, Inc.
 * All rights reserved. 
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use,
 * copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following
 * conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT.  IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 *------------------------------------------------------------------
 */

#ifndef __RUNTIME_ENVIRONMENT_H__
#define __RUNTIME_ENVIRONMENT_H__


#define LOCK_INTERRUPTS()
#define UNLOCK_INTERRUPTS()


extern void
lock_interrupts(void *context);

extern void
lock_uninterrupts(void *context);


#endif 

