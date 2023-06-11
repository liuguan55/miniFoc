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
// Created by 86189 on 2023/6/4.
//

#ifndef MINIFOC_F4_HAL_USB_H
#define MINIFOC_F4_HAL_USB_H
#include "stdint.h"
#include "driver/hal/hal_def.h"

#ifdef __cplusplus
extern "C" {
#endif

enum {
    HAL_USB_FS = 0,
    HAL_USB_HS,
    HAL_USB_NR
};
typedef uint8_t HAL_USB_TYPE;


/**
 * @brief HAL_usbInit
 * @param type usb type
 * @return  HAL_STATUS_OK or HAL_STATUS_ERROR
 */
HAL_Status HAL_usbInit(HAL_USB_TYPE type);

/**
 * @brief HAL_usbDeInit
 * @param type
 * @return
 */
HAL_Status HAL_usbDeInit(HAL_USB_TYPE type);


/**
 * @brief Usb recv callback
 * @param buf Recv data buffer
 * @param len Recv data length
 */
void HAL_recvCallback(uint8_t *buf, uint32_t len);

/**
 * @brief send data to usb avoid CDC interface
 * @param buf pointer to data buffer
 * @param len pointer to data length
 * @return  HAL_STATUS_OK or HAL_STATUS_ERROR
 */
HAL_Status HAL_usbCdcSend(uint8_t *buf, uint32_t len);

/**
 * @brief Recv data from usb avoid CDC interface
 * @param buf   pointer to data buffer
 * @param len   pointer to data length
 * @param readLen   pointer to real read data length
 * @return HAL_STATUS_OK or HAL_STATUS_ERROR
 */
HAL_Status  HAL_usbCdcRecv(uint8_t *buf, uint32_t len, uint32_t *readLen);

#ifdef __cplusplus
}
#endif
#endif //MINIFOC_F4_HAL_USB_H
