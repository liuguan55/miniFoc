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
#include "stm32f4xx_hal.h"
#include "driver/hal/hal_base.h"
#include "driver/hal/hal_debug.h"
#include "common/board/BoardDebug.h"
#include "driver/hal/hal_gpio.h"
#include "driver/hal/hal_uart.h"
#include "driver/hal/hal_adc.h"
#include "driver/hal/hal_i2c.h"
#include "driver/hal/hal_spi.h"
#include "driver/hal/hal_sdio.h"
#include "driver/hal/hal_usb.h"
#include "common/board/BoardCommon.h"


static const GPIO_PinMuxParam g_pinmux_button[] = {
        {HAL_GPIO_PORT_C, HAL_GPIO_PIN_14, {HAL_GPIO_MODE_INPUT, HAL_GPIO_PULL_NONE, HAL_GPIO_DRIVING_LEVEL_0, HAL_GPIO_AF_NONE}},
        {HAL_GPIO_PORT_C, HAL_GPIO_PIN_15, {HAL_GPIO_MODE_INPUT, HAL_GPIO_PULL_NONE, HAL_GPIO_DRIVING_LEVEL_0, HAL_GPIO_AF_NONE}},
        {HAL_GPIO_PORT_B, HAL_GPIO_PIN_8, {HAL_GPIO_MODE_INPUT, HAL_GPIO_PULL_NONE, HAL_GPIO_DRIVING_LEVEL_0, HAL_GPIO_AF_NONE}},
        {HAL_GPIO_PORT_B, HAL_GPIO_PIN_9, {HAL_GPIO_MODE_INPUT, HAL_GPIO_PULL_NONE, HAL_GPIO_DRIVING_LEVEL_0, HAL_GPIO_AF_NONE}},
};

static const GPIO_PinMuxParam g_pinmux_pwm[] = {
        {HAL_GPIO_PORT_A, HAL_GPIO_PIN_1, {HAL_GPIO_MODE_AF_PP, HAL_GPIO_PULL_NONE, HAL_GPIO_DRIVING_LEVEL_3, HAL_GPIO_AF1_TIM2}},
        {HAL_GPIO_PORT_A, HAL_GPIO_PIN_2, {HAL_GPIO_MODE_AF_PP, HAL_GPIO_PULL_NONE, HAL_GPIO_DRIVING_LEVEL_3, HAL_GPIO_AF1_TIM2}},
        {HAL_GPIO_PORT_A, HAL_GPIO_PIN_3, {HAL_GPIO_MODE_AF_PP, HAL_GPIO_PULL_NONE, HAL_GPIO_DRIVING_LEVEL_3, HAL_GPIO_AF1_TIM2}},
};

static const GPIO_PinMuxParam g_pinmux_adc1[] = {
        {HAL_GPIO_PORT_A, HAL_GPIO_PIN_4, {HAL_GPIO_MODE_ANALOG, HAL_GPIO_PULL_NONE, HAL_GPIO_DRIVING_LEVEL_0, HAL_GPIO_AF_NONE}},
};

static const GPIO_PinMuxParam g_pinmux_Gpio[] = {
        {HAL_GPIO_PORT_C, HAL_GPIO_PIN_4, {HAL_GPIO_MODE_OUTPUT_PP, HAL_GPIO_PULL_NONE, HAL_GPIO_DRIVING_LEVEL_0, HAL_GPIO_AF_NONE}}, //MOTOR EN
        {HAL_GPIO_PORT_C, HAL_GPIO_PIN_13, {HAL_GPIO_MODE_OUTPUT_PP, HAL_GPIO_PULL_NONE, HAL_GPIO_DRIVING_LEVEL_0, HAL_GPIO_AF_NONE}},//LED
        { HAL_GPIO_PORT_A, HAL_GPIO_PIN_8,  { HAL_GPIO_MODE_OUTPUT_PP,   HAL_GPIO_PULL_NONE, HAL_GPIO_DRIVING_LEVEL_3 , HAL_GPIO_AF_NONE} }, //LCD RST
        { HAL_GPIO_PORT_B, HAL_GPIO_PIN_14,  { HAL_GPIO_MODE_OUTPUT_PP,   HAL_GPIO_PULL_NONE, HAL_GPIO_DRIVING_LEVEL_3 , HAL_GPIO_AF_NONE} }, //LCD RS
};

static const GPIO_PinMuxParam g_pinmux_uart0[] = {
        { HAL_GPIO_PORT_A, HAL_GPIO_PIN_9,  { HAL_GPIO_MODE_AF_PP,   HAL_GPIO_PULL_NONE, HAL_GPIO_DRIVING_LEVEL_3 , HAL_GPIO_AF7_USART1} }, /* TX */
        { HAL_GPIO_PORT_A, HAL_GPIO_PIN_10,  { HAL_GPIO_MODE_AF_PP,   HAL_GPIO_PULL_NONE, HAL_GPIO_DRIVING_LEVEL_3 ,HAL_GPIO_AF7_USART1} }, /* RX */
};

static const GPIO_PinMuxParam g_pinmux_i2c1[] = {
        { HAL_GPIO_PORT_B, HAL_GPIO_PIN_6,  { HAL_GPIO_MODE_AF_OD,   HAL_GPIO_PULL_UP, HAL_GPIO_DRIVING_LEVEL_3 , HAL_GPIO_AF4_I2C1} }, /* SCL */
        { HAL_GPIO_PORT_B, HAL_GPIO_PIN_7,  { HAL_GPIO_MODE_AF_OD,   HAL_GPIO_PULL_UP, HAL_GPIO_DRIVING_LEVEL_3 ,HAL_GPIO_AF4_I2C1} }, /* SDA */
};

static const GPIO_PinMuxParam g_pinmux_spi1[] = {
        { HAL_GPIO_PORT_B, HAL_GPIO_PIN_3,  { HAL_GPIO_MODE_AF_PP,   HAL_GPIO_PULL_NONE, HAL_GPIO_DRIVING_LEVEL_3 , HAL_GPIO_AF5_SPI1} }, /* CLK */
        { HAL_GPIO_PORT_B, HAL_GPIO_PIN_4,  { HAL_GPIO_MODE_AF_PP,   HAL_GPIO_PULL_NONE, HAL_GPIO_DRIVING_LEVEL_3 ,HAL_GPIO_AF5_SPI1} }, /* MISO */
        { HAL_GPIO_PORT_B, HAL_GPIO_PIN_5,  { HAL_GPIO_MODE_AF_PP,   HAL_GPIO_PULL_NONE, HAL_GPIO_DRIVING_LEVEL_3 ,HAL_GPIO_AF5_SPI1} }, /* MOSI */
};

static const GPIO_PinMuxParam  g_pinmux_spi1_cs[] = {
        { HAL_GPIO_PORT_A, HAL_GPIO_PIN_15,  { HAL_GPIO_MODE_OUTPUT_PP,   HAL_GPIO_PULL_UP, HAL_GPIO_DRIVING_LEVEL_3 , HAL_GPIO_AF_NONE} }, /* CS */
};

static const GPIO_PinMuxParam  g_pinmux_spi2[] = {
        { HAL_GPIO_PORT_C, HAL_GPIO_PIN_2,  { HAL_GPIO_MODE_AF_PP,   HAL_GPIO_PULL_NONE, HAL_GPIO_DRIVING_LEVEL_3 , HAL_GPIO_AF5_SPI2} }, /* CLK */
        { HAL_GPIO_PORT_C, HAL_GPIO_PIN_3,  { HAL_GPIO_MODE_AF_PP,   HAL_GPIO_PULL_NONE, HAL_GPIO_DRIVING_LEVEL_3 ,HAL_GPIO_AF5_SPI2} }, /* MISO */
        { HAL_GPIO_PORT_B, HAL_GPIO_PIN_13,  { HAL_GPIO_MODE_AF_PP,   HAL_GPIO_PULL_NONE, HAL_GPIO_DRIVING_LEVEL_3 ,HAL_GPIO_AF5_SPI2} }, /* MOSI */
};

static const GPIO_PinMuxParam  g_pinmux_spi2_cs[] = {
        { HAL_GPIO_PORT_B, HAL_GPIO_PIN_12,  { HAL_GPIO_MODE_OUTPUT_PP,   HAL_GPIO_PULL_UP, HAL_GPIO_DRIVING_LEVEL_3 , HAL_GPIO_AF_NONE} }, /* CS */
        { HAL_GPIO_PORT_C, HAL_GPIO_PIN_5,  { HAL_GPIO_MODE_OUTPUT_PP,   HAL_GPIO_PULL_UP, HAL_GPIO_DRIVING_LEVEL_3 , HAL_GPIO_AF_NONE} }, /* CS */
};


static const GPIO_PinMuxParam  g_pinmux_usb_fs[] = {
        { HAL_GPIO_PORT_A, HAL_GPIO_PIN_11,  { HAL_GPIO_MODE_AF_PP,   HAL_GPIO_PULL_NONE, HAL_GPIO_DRIVING_LEVEL_3 , HAL_GPIO_AF10_OTG_FS} }, /* CS */
        { HAL_GPIO_PORT_A, HAL_GPIO_PIN_12,  { HAL_GPIO_MODE_AF_PP,   HAL_GPIO_PULL_NONE, HAL_GPIO_DRIVING_LEVEL_3 , HAL_GPIO_AF10_OTG_FS} }, /* CS */

};

static const GPIO_PinMuxParam  g_pimux_sdc[] = {
        { HAL_GPIO_PORT_C, HAL_GPIO_PIN_8,  { HAL_GPIO_MODE_AF_PP,   HAL_GPIO_PULL_NONE, HAL_GPIO_DRIVING_LEVEL_3 , HAL_GPIO_AF12_SDIO} }, /* CS */
        { HAL_GPIO_PORT_C, HAL_GPIO_PIN_9,  { HAL_GPIO_MODE_AF_PP,   HAL_GPIO_PULL_NONE, HAL_GPIO_DRIVING_LEVEL_3 , HAL_GPIO_AF12_SDIO} }, /* CS */
        { HAL_GPIO_PORT_C, HAL_GPIO_PIN_10,  { HAL_GPIO_MODE_AF_PP,   HAL_GPIO_PULL_NONE, HAL_GPIO_DRIVING_LEVEL_3 , HAL_GPIO_AF12_SDIO} }, /* CS */
        { HAL_GPIO_PORT_C, HAL_GPIO_PIN_11,  { HAL_GPIO_MODE_AF_PP,   HAL_GPIO_PULL_NONE, HAL_GPIO_DRIVING_LEVEL_3 , HAL_GPIO_AF12_SDIO} }, /* CS */
        { HAL_GPIO_PORT_C, HAL_GPIO_PIN_12,  { HAL_GPIO_MODE_AF_PP,   HAL_GPIO_PULL_NONE, HAL_GPIO_DRIVING_LEVEL_3 , HAL_GPIO_AF12_SDIO} }, /* CS */
        { HAL_GPIO_PORT_D, HAL_GPIO_PIN_2,  { HAL_GPIO_MODE_AF_PP,   HAL_GPIO_PULL_NONE, HAL_GPIO_DRIVING_LEVEL_3 , HAL_GPIO_AF12_SDIO} }, /* CS */
};

static const GPIO_PinMuxParam  g_pinmux_flashc[] = {
        { HAL_GPIO_PORT_C, HAL_GPIO_PIN_2,  { HAL_GPIO_MODE_AF_PP,   HAL_GPIO_PULL_NONE, HAL_GPIO_DRIVING_LEVEL_3 , HAL_GPIO_AF5_SPI2} }, /* CLK */
        { HAL_GPIO_PORT_C, HAL_GPIO_PIN_3,  { HAL_GPIO_MODE_AF_PP,   HAL_GPIO_PULL_NONE, HAL_GPIO_DRIVING_LEVEL_3 ,HAL_GPIO_AF5_SPI2} }, /* MISO */
        { HAL_GPIO_PORT_B, HAL_GPIO_PIN_13,  { HAL_GPIO_MODE_AF_PP,   HAL_GPIO_PULL_NONE, HAL_GPIO_DRIVING_LEVEL_3 ,HAL_GPIO_AF5_SPI2} }, /* MOSI */
        { HAL_GPIO_PORT_C, HAL_GPIO_PIN_5,  { HAL_GPIO_MODE_OUTPUT_PP,   HAL_GPIO_PULL_UP, HAL_GPIO_DRIVING_LEVEL_3 , HAL_GPIO_AF_NONE} }, /* CS */
};

static const GPIO_PinMuxParam  g_pinmux_led[] = {
        { HAL_GPIO_PORT_C, HAL_GPIO_PIN_13,  { HAL_GPIO_MODE_OUTPUT_PP,   HAL_GPIO_PULL_UP, HAL_GPIO_DRIVING_LEVEL_3 , HAL_GPIO_AF_NONE} }, /* SCL */
};

static HAL_Status board_get_pinmux_info(uint32_t major, uint32_t minor, uint32_t param,
                                        struct board_pinmux_info info[])
{
    uint8_t i;
    HAL_Status ret = HAL_STATUS_OK;

    switch (major) {
        case HAL_DEV_MAJOR_I2C:
            if (minor == HAL_I2C_1) {
                info[0].pinmux = g_pinmux_i2c1;
                info[0].count = HAL_ARRAY_SIZE(g_pinmux_i2c1);
            } else {
                ret = HAL_STATUS_INVALID;
            }
            break;
        case HAL_DEV_MAJOR_UART:
            if (minor == HAL_UART_1) {
                info[0].pinmux = g_pinmux_uart0;
                info[0].count = HAL_ARRAY_SIZE(g_pinmux_uart0);
            } else {
                ret = HAL_STATUS_INVALID;
            }
            break;
        case HAL_DEV_MAJOR_SPI:
            if (minor == HAL_SPI_1) {
                info[0].pinmux = g_pinmux_spi1;
                info[0].count = HAL_ARRAY_SIZE(g_pinmux_spi1);
                info[1].pinmux = g_pinmux_spi1_cs;
                info[1].count = HAL_ARRAY_SIZE(g_pinmux_spi1_cs);
            } else if (minor == HAL_SPI_2) {
                info[0].pinmux = g_pinmux_spi2;
                info[0].count = HAL_ARRAY_SIZE(g_pinmux_spi2);
                info[1].pinmux = g_pinmux_spi2_cs;
                info[1].count = HAL_ARRAY_SIZE(g_pinmux_spi2_cs);
            } else {
                ret = HAL_STATUS_INVALID;
            }
            break;

        case HAL_DEV_MAJOR_USB:
            if (minor == HAL_USB_FS) {
                info[0].pinmux = g_pinmux_usb_fs;
                info[0].count = HAL_ARRAY_SIZE(g_pinmux_usb_fs);
            } else {
                ret = HAL_STATUS_INVALID;
            }
            break;
        case HAL_DEV_MAJOR_SDC:
            if (minor == HAL_SDC_1) {
                info[0].pinmux = g_pimux_sdc;
                info[0].count = HAL_ARRAY_SIZE(g_pimux_sdc);
            } else {
                ret = HAL_STATUS_INVALID;
            }
            break;
        case HAL_DEV_MAJOR_PWM:
            if (minor < HAL_ARRAY_SIZE(g_pinmux_pwm)){
                info[0].pinmux = &g_pinmux_pwm[minor];
                info[0].count = 1;
            } else {
                ret = HAL_STATUS_INVALID;
            }
            break;
        case HAL_DEV_MAJOR_ADC:
            if (minor == HAL_ADC_1) {
                info[0].pinmux = g_pinmux_adc1;
                info[0].count = HAL_ARRAY_SIZE(g_pimux_sdc);
            } else {
                ret = HAL_STATUS_INVALID;
            }
            break;
        case HAL_DEV_MAJOR_FLASHC:
            info[0].pinmux = g_pinmux_flashc;
            info[0].count = HAL_ARRAY_SIZE(g_pinmux_flashc);
            break;
        case HAL_DEV_MAJOR_LED:
            if (minor < HAL_ARRAY_SIZE(g_pinmux_led)){
                info[0].pinmux = &g_pinmux_led[minor];
                info[0].count = 1;//HAL_ARRAY_SIZE(g_pinmux_led);
            }
            break;
        default:
            BOARD_ERR("unknow major %u\n", major);
            ret = HAL_STATUS_INVALID;
    }

    return ret;
}

static HAL_Status board_get_cfg(uint32_t major, uint32_t minor, uint32_t param)
{
    uint8_t i;
    HAL_Status ret = HAL_STATUS_OK;

    switch (major) {
        case HAL_DEV_MAJOR_SPI:
            if (minor == HAL_SPI_1) {
                board_pinmux_info_t *info = (board_pinmux_info_t *)param;
                info->pinmux = g_pinmux_spi1_cs;
                info->count = HAL_ARRAY_SIZE(g_pinmux_spi1_cs);
            } else if (minor == HAL_SPI_2) {
                board_pinmux_info_t *info = (board_pinmux_info_t *)param;
                info->pinmux = g_pinmux_spi2_cs;
                info->count = HAL_ARRAY_SIZE(g_pinmux_spi2_cs);
            } else {
                ret = HAL_STATUS_INVALID;
            }
            break;
        case HAL_DEV_MAJOR_LED:
            if (minor < HAL_ARRAY_SIZE(g_pinmux_led)){
                board_pinmux_info_t *info = (board_pinmux_info_t *) param;
                info->pinmux = &g_pinmux_led[minor];
                info->count = 1;//HAL_ARRAY_SIZE(g_pinmux_led);
            }
            break;
        default:
            BOARD_ERR("unknow major %u\n", major);
            ret = HAL_STATUS_INVALID;
    }

    return ret;
}

HAL_Status board_ioctl(HAL_BoardIoctlReq req, uint32_t param0, uint32_t param1)
{
    HAL_Status ret = HAL_STATUS_OK;
    uint32_t major, minor, i;
    struct board_pinmux_info info[BOARD_PINMUX_INFO_MAX];

    switch (req) {
        case HAL_BIR_PINMUX_INIT:
        case HAL_BIR_PINMUX_DEINIT:
            memset(info, 0, sizeof(info));
            major = HAL_DEV_MAJOR((HAL_Dev_t)param0);
            minor = HAL_DEV_MINOR((HAL_Dev_t)param0);
            ret = board_get_pinmux_info(major, minor, param1, info);
            for (i = 0; i < BOARD_PINMUX_INFO_MAX; ++i) {
                if (info[i].pinmux != NULL && info[i].count != 0) {
                    board_pinmux_cfg(req, info[i].pinmux, info[i].count);
                } else {
                    break;
                }
            }
            break;
        case HAL_BIR_CHIP_CLOCK_INIT:
            board_chip_clock_init();
            break;
        case HAL_BIR_GET_CFG:
            major = HAL_DEV_MAJOR((HAL_Dev_t)param0);
            minor = HAL_DEV_MINOR((HAL_Dev_t)param0);
            ret = board_get_cfg(major, minor, param1);
            break;
        default:
            BOARD_ERR("req %d not suppport\n", req);
            ret = HAL_STATUS_INVALID;
            break;
    }

    if (ret != HAL_STATUS_OK) {
        BOARD_ERR("req %d, param0 %#x, param1 %#x, ret %d\n", req, param0, param1, ret);
    }

    return ret;
}


