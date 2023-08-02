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
#include "driver/driver_chip.h"
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
#include "sys/MiniDebug.h"


static const __unused GPIO_PinMuxParam g_pinmux_button[] = {
        {HAL_GPIO_PORT_C, HAL_GPIO_PIN_14, {HAL_GPIO_MODE_INPUT, HAL_GPIO_PULL_NONE, HAL_GPIO_DRIVING_LEVEL_0, HAL_GPIO_AF_NONE}},
        {HAL_GPIO_PORT_C, HAL_GPIO_PIN_15, {HAL_GPIO_MODE_INPUT, HAL_GPIO_PULL_NONE, HAL_GPIO_DRIVING_LEVEL_0, HAL_GPIO_AF_NONE}},
        {HAL_GPIO_PORT_B, HAL_GPIO_PIN_8, {HAL_GPIO_MODE_INPUT, HAL_GPIO_PULL_NONE, HAL_GPIO_DRIVING_LEVEL_0, HAL_GPIO_AF_NONE}},
        {HAL_GPIO_PORT_B, HAL_GPIO_PIN_9, {HAL_GPIO_MODE_INPUT, HAL_GPIO_PULL_NONE, HAL_GPIO_DRIVING_LEVEL_0, HAL_GPIO_AF_NONE}},
};


static const GPIO_PinMuxParam g_pinmux_adc1[] = {
        {HAL_GPIO_PORT_A, HAL_GPIO_PIN_4, {HAL_GPIO_MODE_ANALOG, HAL_GPIO_PULL_NONE, HAL_GPIO_DRIVING_LEVEL_0, HAL_GPIO_AF_NONE}},
};

static const __unused GPIO_PinMuxParam g_pinmux_Gpio[] = {
        {HAL_GPIO_PORT_C, HAL_GPIO_PIN_4, {HAL_GPIO_MODE_OUTPUT_PP, HAL_GPIO_PULL_NONE, HAL_GPIO_DRIVING_LEVEL_0, HAL_GPIO_AF_NONE}}, //MOTOR EN
        {HAL_GPIO_PORT_C, HAL_GPIO_PIN_13, {HAL_GPIO_MODE_OUTPUT_PP, HAL_GPIO_PULL_NONE, HAL_GPIO_DRIVING_LEVEL_0, HAL_GPIO_AF_NONE}},//LED
        { HAL_GPIO_PORT_A, HAL_GPIO_PIN_8,  { HAL_GPIO_MODE_OUTPUT_PP,   HAL_GPIO_PULL_NONE, HAL_GPIO_DRIVING_LEVEL_2 , HAL_GPIO_AF_NONE} }, //LCD RST
        { HAL_GPIO_PORT_B, HAL_GPIO_PIN_14,  { HAL_GPIO_MODE_OUTPUT_PP,   HAL_GPIO_PULL_NONE, HAL_GPIO_DRIVING_LEVEL_2 , HAL_GPIO_AF_NONE} }, //LCD RS
};

static const GPIO_PinMuxParam g_pinmux_uart0[] = {
        { HAL_GPIO_PORT_A, HAL_GPIO_PIN_9,  { HAL_GPIO_MODE_AF_PP,   HAL_GPIO_PULL_NONE, HAL_GPIO_DRIVING_LEVEL_2 , HAL_GPIO_AF7_USART1} }, /* TX */
        { HAL_GPIO_PORT_A, HAL_GPIO_PIN_10,  { HAL_GPIO_MODE_AF_PP,   HAL_GPIO_PULL_NONE, HAL_GPIO_DRIVING_LEVEL_2 ,HAL_GPIO_AF7_USART1} }, /* RX */
};


static const GPIO_PinMuxParam g_pinmux_spi1[] = {
        { HAL_GPIO_PORT_B, HAL_GPIO_PIN_3,  { HAL_GPIO_MODE_AF_PP,   HAL_GPIO_PULL_NONE, HAL_GPIO_DRIVING_LEVEL_2 , HAL_GPIO_AF5_SPI1} }, /* CLK */
        { HAL_GPIO_PORT_B, HAL_GPIO_PIN_4,  { HAL_GPIO_MODE_AF_PP,   HAL_GPIO_PULL_NONE, HAL_GPIO_DRIVING_LEVEL_2 ,HAL_GPIO_AF5_SPI1} }, /* MISO */
        { HAL_GPIO_PORT_B, HAL_GPIO_PIN_5,  { HAL_GPIO_MODE_AF_PP,   HAL_GPIO_PULL_NONE, HAL_GPIO_DRIVING_LEVEL_2 ,HAL_GPIO_AF5_SPI1} }, /* MOSI */
};

static const GPIO_PinMuxParam  g_pinmux_spi1_cs[] = {
        { HAL_GPIO_PORT_A, HAL_GPIO_PIN_15,  { HAL_GPIO_MODE_OUTPUT_PP,   HAL_GPIO_PULL_UP, HAL_GPIO_DRIVING_LEVEL_2 , HAL_GPIO_AF_NONE} }, /* CS */
};

static const GPIO_PinMuxParam  g_pinmux_spi2[] = {
        { HAL_GPIO_PORT_C, HAL_GPIO_PIN_2,  { HAL_GPIO_MODE_AF_PP,   HAL_GPIO_PULL_NONE, HAL_GPIO_DRIVING_LEVEL_2 , HAL_GPIO_AF5_SPI2} }, /* CLK */
        { HAL_GPIO_PORT_C, HAL_GPIO_PIN_3,  { HAL_GPIO_MODE_AF_PP,   HAL_GPIO_PULL_NONE, HAL_GPIO_DRIVING_LEVEL_2 ,HAL_GPIO_AF5_SPI2} }, /* MISO */
        { HAL_GPIO_PORT_B, HAL_GPIO_PIN_13,  { HAL_GPIO_MODE_AF_PP,   HAL_GPIO_PULL_NONE, HAL_GPIO_DRIVING_LEVEL_2 ,HAL_GPIO_AF5_SPI2} }, /* MOSI */
};

static const GPIO_PinMuxParam  g_pinmux_spi2_cs[] = {
        { HAL_GPIO_PORT_B, HAL_GPIO_PIN_12,  { HAL_GPIO_MODE_OUTPUT_PP,   HAL_GPIO_PULL_UP, HAL_GPIO_DRIVING_LEVEL_2 , HAL_GPIO_AF_NONE} }, /* CS */
        { HAL_GPIO_PORT_C, HAL_GPIO_PIN_5,  { HAL_GPIO_MODE_OUTPUT_PP,   HAL_GPIO_PULL_UP, HAL_GPIO_DRIVING_LEVEL_2 , HAL_GPIO_AF_NONE} }, /* CS */
};

static const GPIO_PinMuxParam  g_pinmux_led[] = {
        { HAL_GPIO_PORT_C, HAL_GPIO_PIN_13,  { HAL_GPIO_MODE_OUTPUT_PP,   HAL_GPIO_PULL_UP, HAL_GPIO_DRIVING_LEVEL_2 , HAL_GPIO_AF_NONE} }, /* SCL */
};



/**
  * @brief System Clock Configuration
  * @retval None
  */
void board_chip_clock_init(void)
{
    RCC_OscInitTypeDef RCC_OscInitStruct = {0};
    RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
    RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

    /** Initializes the RCC Oscillators according to the specified parameters
    * in the RCC_OscInitTypeDef structure.
    */
    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
    RCC_OscInitStruct.HSEState = RCC_HSE_ON;
    RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
    RCC_OscInitStruct.HSIState = RCC_HSI_ON;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
    RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
    RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
    {
        assert(NULL);
    }

    /** Initializes the CPU, AHB and APB buses clocks
    */
    RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                                  |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
    RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

    if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
    {
        assert(NULL);
    }
    PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_ADC;
    PeriphClkInit.AdcClockSelection = RCC_ADCPCLK2_DIV6;
    if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
    {
        assert(NULL);
    }
}

static HAL_Status board_get_pinmux_info(uint32_t major, uint32_t minor, uint32_t param,
                                        struct board_pinmux_info info[])
{
    UNUSED(param);
    HAL_Status ret = HAL_STATUS_OK;

    switch (major) {
        case HAL_DEV_MAJOR_I2C:
           
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
           
            break;
        case HAL_DEV_MAJOR_SDC:
           
            break;
        case HAL_DEV_MAJOR_PWM:
           
            break;
        case HAL_DEV_MAJOR_ADC:
            if (minor == HAL_ADC_1) {
                info[0].pinmux = g_pinmux_adc1;
                info[0].count = HAL_ARRAY_SIZE(g_pinmux_adc1);
            } else {
                ret = HAL_STATUS_INVALID;
            }
            break;
        case HAL_DEV_MAJOR_FLASHC:
          
            break;
        case HAL_DEV_MAJOR_LED:
            if (minor < HAL_ARRAY_SIZE(g_pinmux_led)){
                info[0].pinmux = &g_pinmux_led[minor];
                info[0].count = 1;//HAL_ARRAY_SIZE(g_pinmux_led);
            }
            break;
        default:
            BOARD_ERR("unknow major %lu\n", major);
            ret = HAL_STATUS_INVALID;
    }

    return ret;
}

static HAL_Status board_get_cfg(uint32_t major, uint32_t minor, uint32_t param)
{
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
            BOARD_ERR("unknow major %lu\n", major);
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
        BOARD_ERR("req %d, param0 %#lx, param1 %#lx, ret %d\n", req, param0, param1, ret);
    }

    return ret;
}

