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

#ifndef MINIFOC_F4_HAL_DEF_H
#define MINIFOC_F4_HAL_DEF_H
#include "stdint.h"

#ifdef __cplusplus
extern "C" {
#endif

#define HAL_ALIGN_UP(x, a) (((x) + (a) - 1) & ~((a) - 1))
#define HAL_ALIGN_DOWN(x, a) ((x) & ~((a) - 1))
#define HAL_PAD_SIZE(x, a)  (((x) + (a) - 1) & ~((a) - 1) - (x))
#define HAL_DIV_ROUND_UP(x, a) (((x) + (a) - 1) / (a))
#define HAL_DIV_ROUND_DOWN(x, a) ((x) / (a))

/*
 * Bitwise operation
 */
#define HAL_BIT(pos)                        (1U << (pos))

#define HAL_SET_BIT(reg, mask)              ((reg) |= (mask))
#define HAL_CLR_BIT(reg, mask)              ((reg) &= ~(mask))
#define HAL_GET_BIT(reg, mask)              ((reg) & (mask))
#define HAL_GET_BIT_VAL(reg, shift, vmask)  (((reg) >> (shift)) & (vmask))

#define HAL_MODIFY_REG(reg, clr_mask, set_mask) \
    ((reg) = (((reg) & (~(clr_mask))) | (set_mask)))

/*
 * Macros for accessing LSBs of a 32-bit register (little endian only)
 */
#define HAL_REG_32BIT(reg_addr)  (*((__IO uint32_t *)(reg_addr)))
#define HAL_REG_16BIT(reg_addr)  (*((__IO uint16_t *)(reg_addr)))
#define HAL_REG_8BIT(reg_addr)   (*((__IO uint8_t  *)(reg_addr)))

/* Macro for counting the element number of an array */
#define HAL_ARRAY_SIZE(a)   (sizeof((a)) / sizeof((a)[0]))

/* Wait forever timeout value */
#define HAL_WAIT_FOREVER    OS_WAIT_FOREVER

/**
 * @brief HAL Status value
 */
typedef enum {
    HAL_STATUS_OK      = 0,    /* success */
    HAL_STATUS_ERROR   = -1,   /* general error */
    HAL_STATUS_BUSY    = -2,   /* device or resource busy */
    HAL_STATUS_TIMEOUT = -3,   /* wait timeout */
    HAL_STATUS_INVALID = -4,    /* invalid argument */
    HAL_STATUS_MEM_FAILED = -5    /* memory failed */
} HAL_Status;

#ifdef __cplusplus
}
#endif


#endif //MINIFOC_F4_HAL_DEF_H
