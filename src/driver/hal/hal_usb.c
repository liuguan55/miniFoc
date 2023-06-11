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

#include "driver/hal/hal_usb.h"
#include "usb_device.h"
#include "usbd_core.h"
#include "usbd_desc.h"
#include "usbd_cdc.h"
#include "usbd_cdc_if.h"
#include "usbd_composite.h"
#include "driver/hal/hal_os.h"
#include "driver/hal/hal_def.h"
#include "driver/hal/hal_dev.h"
#include "driver/hal/hal_board.h"
#include "core/lwrb/lwrb.h"
#include "sys/MiniDebug.h"

#define HAL_USB_RX_BUFFER_SIZE  128
typedef struct {
    USBD_HandleTypeDef *hUsbDevice;
    HAL_Status (*init)(void);
    HAL_Status (*deInit)(void);
    uint8_t rxBuffer[HAL_USB_RX_BUFFER_SIZE];
    lwrb_t rxRingBuffer;
}HAL_UsbDev_t;

static HAL_Status HAL_usbHsInit(void);
static HAL_Status HAL_usbFsInit(void);
static HAL_Status HAL_usbHsDeInit(void);
static HAL_Status HAL_usbFsDeInit(void);

static HAL_UsbDev_t HAL_usbDev[HAL_USB_NR] = {
        {
                .hUsbDevice = &hUsbDeviceFS,
                .init = HAL_usbFsInit,
                .deInit = HAL_usbFsDeInit,
        },
        {
                .hUsbDevice = NULL,
                .init = HAL_usbHsInit,
                .deInit = HAL_usbHsDeInit,
        }
};

extern PCD_HandleTypeDef hpcd_USB_OTG_FS;
/**
  * @brief This function handles USB On The Go FS global interrupt.
  */
void OTG_FS_IRQHandler(void) {
    /* USER CODE BEGIN OTG_FS_IRQn 0 */

    /* USER CODE END OTG_FS_IRQn 0 */
    HAL_PCD_IRQHandler(&hpcd_USB_OTG_FS);
    /* USER CODE BEGIN OTG_FS_IRQn 1 */

    /* USER CODE END OTG_FS_IRQn 1 */
}

static HAL_Status HAL_usbHsInit(void){
    return HAL_STATUS_OK;
}

static void usbFs_Status_Init(void) {
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    /* GPIO Ports Clock Enable */
    __HAL_RCC_GPIOA_CLK_ENABLE();

    /*Configure GPIO pin Output Level */
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_11 | GPIO_PIN_12, GPIO_PIN_RESET);

    /*Configure GPIO pin : W25Q256_CS_Pin */
    GPIO_InitStruct.Pin = GPIO_PIN_11 | GPIO_PIN_12;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_PULLDOWN;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    //假如不行的话，下面的延时加长即可。
    HAL_Delay(20);
}
static HAL_Status HAL_usbFsInit(void){
    usbFs_Status_Init();
    HAL_BoardIoctl(HAL_BIR_PINMUX_INIT, HAL_MKDEV(HAL_DEV_MAJOR_USB, HAL_USB_FS) , 0);
    /* Init Device Library, add supported class and start the library. */
    if (USBD_Init(&hUsbDeviceFS, &usbCmpsitFS_Desc, DEVICE_FS) != USBD_OK)
    {
        assert(NULL);
    }
    /* if (USBD_RegisterClass(&hUsbDeviceFS, &USBD_MSC) != USBD_OK)
    {
      assert(NULL);
    } */

    if(USBD_RegisterClassComposite(&hUsbDeviceFS, &USBD_CDC,CLASS_TYPE_CDC,0) != USBD_OK)
    {
        assert(NULL);
    }

    if(USBD_CDC_RegisterInterface(&hUsbDeviceFS, &USBD_Interface_fops_FS) != USBD_OK)
    {
        assert(NULL);
    }
//  if(USBD_RegisterClassComposite(&hUsbDeviceFS, &USBD_MSC,CLASS_TYPE_MSC,0) != USBD_OK)
//  {
//    assert(NULL);
//  }
//  if (USBD_MSC_RegisterStorage(&hUsbDeviceFS, &USBD_Storage_Interface_fops_FS) != USBD_OK)
//  {
//     assert(NULL);
//  }
    if (USBD_Start(&hUsbDeviceFS) != USBD_OK)
    {
        assert(NULL);
    }

    return HAL_STATUS_OK;
}

static HAL_Status HAL_usbFsDeInit(void){
    HAL_BoardIoctl(HAL_BIR_PINMUX_DEINIT, HAL_MKDEV(HAL_DEV_MAJOR_USB, HAL_USB_FS) , 0);
    return HAL_STATUS_OK;
}

static HAL_Status HAL_usbHsDeInit(void){
    return HAL_STATUS_OK;
}

/**
 * @brief HAL_usbInit
 * @param type usb type
 * @return  HAL_STATUS_OK or HAL_STATUS_ERROR
 */
HAL_Status HAL_usbInit(HAL_USB_TYPE type){
    HAL_Status ret;

    if (type >= HAL_USB_NR){
        return HAL_STATUS_ERROR;
    }

    ret = HAL_usbDev[type].init();

    lwrb_init(&HAL_usbDev[type].rxRingBuffer, HAL_usbDev[type].rxBuffer, HAL_USB_RX_BUFFER_SIZE);

    return ret;
}

/**
 * @brief HAL_usbDeInit
 * @param type
 * @return
 */
HAL_Status HAL_usbDeInit(HAL_USB_TYPE type){
    HAL_Status ret;
    if (type >= HAL_USB_NR){
        return HAL_STATUS_ERROR;
    }

    ret = HAL_usbDev[type].deInit();
    lwrb_free(&HAL_usbDev[type].rxRingBuffer);

    return ret;
}

void HAL_recvCallback(uint8_t *buf, uint32_t len){
    HAL_UsbDev_t *dev = &HAL_usbDev[HAL_USB_FS];
    lwrb_write(&dev->rxRingBuffer, buf, len);
}

HAL_Status HAL_usbCdcSend(uint8_t *buf, uint32_t len){
    usb_write_data((const char *)buf, len);

    return  HAL_STATUS_OK;
}

HAL_Status  HAL_usbCdcRecv(uint8_t *buf, uint32_t len, uint32_t *readLen){
    HAL_UsbDev_t *dev = &HAL_usbDev[HAL_USB_FS];
    if (dev == NULL) {
        return HAL_STATUS_ERROR;
    }
     uint32_t readSize = lwrb_read(&dev->rxRingBuffer, buf, len);
     if (readLen != NULL){
         *readLen = readSize;
     }

    return HAL_STATUS_OK;
}