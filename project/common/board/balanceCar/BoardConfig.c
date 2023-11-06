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
#include "driver/hal/hal_pwm.h"
#include "common/board/BoardCommon.h"
#include "sys/MiniDebug.h"


static const  GPIO_PinMuxParam g_pinmux_button[] = {
        {HAL_GPIO_PORT_A, HAL_GPIO_PIN_0, {HAL_GPIO_MODE_INPUT, HAL_GPIO_PULL_UP, HAL_GPIO_DRIVING_LEVEL_0, HAL_GPIO_AF_NONE}},
        {HAL_GPIO_PORT_A, HAL_GPIO_PIN_12, {HAL_GPIO_MODE_INPUT, HAL_GPIO_PULL_UP, HAL_GPIO_DRIVING_LEVEL_0, HAL_GPIO_AF_NONE}},
        {HAL_GPIO_PORT_B, HAL_GPIO_PIN_14, {HAL_GPIO_MODE_INPUT, HAL_GPIO_PULL_UP, HAL_GPIO_DRIVING_LEVEL_0, HAL_GPIO_AF_NONE}},
};


static const GPIO_PinMuxParam g_pinmux_adc1[] = {
        {HAL_GPIO_PORT_A, HAL_GPIO_PIN_1, {HAL_GPIO_MODE_ANALOG, HAL_GPIO_PULL_NONE, HAL_GPIO_DRIVING_LEVEL_0, HAL_GPIO_AF_NONE}},
};

static const  GPIO_PinMuxParam g_pinmux_gpio[] = {
        {HAL_GPIO_PORT_B, HAL_GPIO_PIN_12, {HAL_GPIO_MODE_OUTPUT_PP, HAL_GPIO_PULL_NONE, HAL_GPIO_DRIVING_LEVEL_0, HAL_GPIO_AF_NONE, .initLevel = GPIO_PIN_LOW}},//buzzer
        { HAL_GPIO_PORT_A, HAL_GPIO_PIN_6,  { HAL_GPIO_MODE_OUTPUT_PP,   HAL_GPIO_PULL_NONE, HAL_GPIO_DRIVING_LEVEL_2 , HAL_GPIO_AF_NONE,.initLevel = GPIO_PIN_HIGH} }, //led
        { HAL_GPIO_PORT_B, HAL_GPIO_PIN_7,  { HAL_GPIO_MODE_OUTPUT_PP,   HAL_GPIO_PULL_NONE, HAL_GPIO_DRIVING_LEVEL_2 , HAL_GPIO_AF_NONE,.initLevel = GPIO_PIN_HIGH} }, //nrf34l01 ce
        { HAL_GPIO_PORT_A, HAL_GPIO_PIN_15,  { HAL_GPIO_MODE_INPUT,   HAL_GPIO_PULL_NONE, HAL_GPIO_DRIVING_LEVEL_2 , HAL_GPIO_AF_NONE,.initLevel = GPIO_PIN_HIGH} }, //nrf34l01 IRQ
        {HAL_GPIO_PORT_C, HAL_GPIO_PIN_13, {HAL_GPIO_MODE_OUTPUT_PP, HAL_GPIO_PULL_NONE, HAL_GPIO_DRIVING_LEVEL_0, HAL_GPIO_AF_NONE, .initLevel = GPIO_PIN_LOW}},//motor en
        {HAL_GPIO_PORT_A, HAL_GPIO_PIN_2, {HAL_GPIO_MODE_OUTPUT_PP, HAL_GPIO_PULL_NONE, HAL_GPIO_DRIVING_LEVEL_0, HAL_GPIO_AF_NONE, .initLevel = GPIO_PIN_LOW}},//gpio pin
        {HAL_GPIO_PORT_A, HAL_GPIO_PIN_3, {HAL_GPIO_MODE_OUTPUT_PP, HAL_GPIO_PULL_NONE, HAL_GPIO_DRIVING_LEVEL_0, HAL_GPIO_AF_NONE, .initLevel = GPIO_PIN_LOW}},//gpio pin
        {HAL_GPIO_PORT_A, HAL_GPIO_PIN_4, {HAL_GPIO_MODE_OUTPUT_PP, HAL_GPIO_PULL_NONE, HAL_GPIO_DRIVING_LEVEL_0, HAL_GPIO_AF_NONE, .initLevel = GPIO_PIN_LOW}},//gpio pin
        {HAL_GPIO_PORT_A, HAL_GPIO_PIN_5, {HAL_GPIO_MODE_OUTPUT_PP, HAL_GPIO_PULL_NONE, HAL_GPIO_DRIVING_LEVEL_0, HAL_GPIO_AF_NONE, .initLevel = GPIO_PIN_LOW}},//gpio pin

//        { HAL_GPIO_PORT_A, HAL_GPIO_PIN_8,  { HAL_GPIO_MODE_OUTPUT_PP,   HAL_GPIO_PULL_NONE, HAL_GPIO_DRIVING_LEVEL_2 , HAL_GPIO_AF_NONE,.initLevel = GPIO_PIN_LOW} }, //timer1 ch 1
//        { HAL_GPIO_PORT_B, HAL_GPIO_PIN_15,  { HAL_GPIO_MODE_OUTPUT_PP,   HAL_GPIO_PULL_NONE, HAL_GPIO_DRIVING_LEVEL_2 , HAL_GPIO_AF_NONE,.initLevel = GPIO_PIN_LOW} }, //timer1 ch 3
//        { HAL_GPIO_PORT_A, HAL_GPIO_PIN_11 , { HAL_GPIO_MODE_OUTPUT_PP,   HAL_GPIO_PULL_NONE, HAL_GPIO_DRIVING_LEVEL_2 , HAL_GPIO_AF_NONE,.initLevel = GPIO_PIN_LOW} }, //timer1 ch 4
//        { HAL_GPIO_PORT_B, HAL_GPIO_PIN_10,  { HAL_GPIO_MODE_OUTPUT_PP,   HAL_GPIO_PULL_NONE, HAL_GPIO_DRIVING_LEVEL_2 , HAL_GPIO_AF_NONE,.initLevel = GPIO_PIN_LOW} }, //timer2 ch 3
//        { HAL_GPIO_PORT_B, HAL_GPIO_PIN_11,  { HAL_GPIO_MODE_OUTPUT_PP,   HAL_GPIO_PULL_NONE, HAL_GPIO_DRIVING_LEVEL_2 , HAL_GPIO_AF_NONE,.initLevel = GPIO_PIN_LOW} }, //timer2 ch 4
//        { HAL_GPIO_PORT_A, HAL_GPIO_PIN_7,  { HAL_GPIO_MODE_OUTPUT_PP,   HAL_GPIO_PULL_NONE, HAL_GPIO_DRIVING_LEVEL_2 , HAL_GPIO_AF_NONE,.initLevel = GPIO_PIN_LOW} }, //timer3 ch 2
//        { HAL_GPIO_PORT_B, HAL_GPIO_PIN_0,  { HAL_GPIO_MODE_OUTPUT_PP,   HAL_GPIO_PULL_NONE, HAL_GPIO_DRIVING_LEVEL_2 , HAL_GPIO_AF_NONE,.initLevel = GPIO_PIN_LOW} }, //timer3 ch 3
//        { HAL_GPIO_PORT_B, HAL_GPIO_PIN_1,  { HAL_GPIO_MODE_OUTPUT_PP,   HAL_GPIO_PULL_NONE, HAL_GPIO_DRIVING_LEVEL_2 , HAL_GPIO_AF_NONE,.initLevel = GPIO_PIN_LOW} }, //timer3 ch 4
};

static const GPIO_PinMuxParam g_pinmux_uart0[] = {
        { HAL_GPIO_PORT_A, HAL_GPIO_PIN_9,  { HAL_GPIO_MODE_AF_PP,   HAL_GPIO_PULL_NONE, HAL_GPIO_DRIVING_LEVEL_2 , HAL_GPIO_AF7_USART1} }, /* TX */
        { HAL_GPIO_PORT_A, HAL_GPIO_PIN_10,  { HAL_GPIO_MODE_AF_PP,   HAL_GPIO_PULL_NONE, HAL_GPIO_DRIVING_LEVEL_2 ,HAL_GPIO_AF7_USART1} }, /* RX */
};

static const GPIO_PinMuxParam g_pinmux_i2c1[] = {
        { HAL_GPIO_PORT_B, HAL_GPIO_PIN_8,  { HAL_GPIO_MODE_AF_OD,   HAL_GPIO_PULL_UP, HAL_GPIO_DRIVING_LEVEL_2 , HAL_GPIO_AF4_I2C1} }, /* SCL */
        { HAL_GPIO_PORT_B, HAL_GPIO_PIN_9,  { HAL_GPIO_MODE_AF_OD,   HAL_GPIO_PULL_UP, HAL_GPIO_DRIVING_LEVEL_2 ,HAL_GPIO_AF4_I2C1} }, /* SDA */
};

static const GPIO_PinMuxParam g_pinmux_spi1[] = {
        { HAL_GPIO_PORT_B, HAL_GPIO_PIN_3,  { HAL_GPIO_MODE_AF_PP,   HAL_GPIO_PULL_NONE, HAL_GPIO_DRIVING_LEVEL_2 , HAL_GPIO_AF5_SPI1} }, /* CLK */
        { HAL_GPIO_PORT_B, HAL_GPIO_PIN_4,  { HAL_GPIO_MODE_AF_PP,   HAL_GPIO_PULL_NONE, HAL_GPIO_DRIVING_LEVEL_2 ,HAL_GPIO_AF5_SPI1} }, /* MISO */
        { HAL_GPIO_PORT_B, HAL_GPIO_PIN_5,  { HAL_GPIO_MODE_AF_PP,   HAL_GPIO_PULL_NONE, HAL_GPIO_DRIVING_LEVEL_2 ,HAL_GPIO_AF5_SPI1} }, /* MOSI */
};

static const GPIO_PinMuxParam  g_pinmux_spi1_cs[] = {
        { HAL_GPIO_PORT_B, HAL_GPIO_PIN_6,  { HAL_GPIO_MODE_OUTPUT_PP,   HAL_GPIO_PULL_UP, HAL_GPIO_DRIVING_LEVEL_2 , HAL_GPIO_AF_NONE,.initLevel = GPIO_PIN_HIGH} }, /* CS */
};

static const GPIO_PinMuxParam  g_pinmux_spi2[] = {
};

static const GPIO_PinMuxParam  g_pinmux_spi2_cs[] = {
};


static const ADC_Channel_t g_adc1_channel[] = {
        {ADC_CHANNEL_1,ADC_REGULAR_RANK_1,ADC_SAMPLETIME_239CYCLES_5},
};
static const ADC_Config_t g_adc_info[HAL_ADC_NR] = {
        {g_adc1_channel, HAL_ARRAY_SIZE(g_adc1_channel), ADC_EXTERNALTRIGCONV_T1_CC1, TIM1, TIM_CHANNEL_1, 71, 9999,
         DMA1_Channel1}
};


static const GPIO_PinMuxParam g_pinmux_pwm1[] = {
        { HAL_GPIO_PORT_A, HAL_GPIO_PIN_8,  { HAL_GPIO_MODE_AF_PP,   HAL_GPIO_PULL_NONE, HAL_GPIO_DRIVING_LEVEL_2 , 0} }, //timer1 ch 1
        { HAL_GPIO_PORT_B, HAL_GPIO_PIN_15,  { HAL_GPIO_MODE_AF_PP,   HAL_GPIO_PULL_NONE, HAL_GPIO_DRIVING_LEVEL_2 , 0} }, //timer1 ch 3
        { HAL_GPIO_PORT_A, HAL_GPIO_PIN_11 , { HAL_GPIO_MODE_AF_PP,   HAL_GPIO_PULL_NONE, HAL_GPIO_DRIVING_LEVEL_2 , 0} }, //timer1 ch 4
};

static const GPIO_PinMuxParam g_pinmux_pwm2[] = {
        { HAL_GPIO_PORT_B, HAL_GPIO_PIN_10,  { HAL_GPIO_MODE_AF_PP,   HAL_GPIO_PULL_NONE, HAL_GPIO_DRIVING_LEVEL_2 , 0} }, //timer2 ch 10
        { HAL_GPIO_PORT_B, HAL_GPIO_PIN_11,  { HAL_GPIO_MODE_AF_PP,   HAL_GPIO_PULL_NONE, HAL_GPIO_DRIVING_LEVEL_2 , 0} }, //timer2 ch 11
};

static const GPIO_PinMuxParam g_pinmux_pwm3[] = {
        { HAL_GPIO_PORT_A, HAL_GPIO_PIN_7,  { HAL_GPIO_MODE_AF_PP,   HAL_GPIO_PULL_NONE, HAL_GPIO_DRIVING_LEVEL_2 , 0} }, //timer3 ch 2
        { HAL_GPIO_PORT_B, HAL_GPIO_PIN_0,  { HAL_GPIO_MODE_AF_PP,   HAL_GPIO_PULL_NONE, HAL_GPIO_DRIVING_LEVEL_2 , 0} }, //timer3 ch 3
        { HAL_GPIO_PORT_B, HAL_GPIO_PIN_1,  { HAL_GPIO_MODE_AF_PP,   HAL_GPIO_PULL_NONE, HAL_GPIO_DRIVING_LEVEL_2 , 0} }, //timer3 ch 4
};

static const PWM_channel_t g_pwm_time1_channel[] = {
        { .channel = HAL_PWM_CH1, .complementary = 0,  .duty = 0},
        { .channel = HAL_PWM_CH3, .complementary = 1,  .duty = 0},
        { .channel = HAL_PWM_CH4, .complementary = 0, .duty = 0},
};
static const PWM_channel_t g_pwm_time2_channel[] = {
        { .channel = HAL_PWM_CH3, .complementary = 0, .duty = 0},
        { .channel = HAL_PWM_CH4, .complementary = 0, .duty = 0},
};
static const PWM_channel_t g_pwm_time3_channel[] = {
        { .channel = HAL_PWM_CH2, .complementary = 0,.duty = 0},
        { .channel = HAL_PWM_CH3, .complementary = 0, .duty = 0},
        { .channel = HAL_PWM_CH4, .complementary = 0, .duty = 0},
};

static const PWM_Config_t g_pwm_info[] = {
        { .id = HAL_PWM_ID_1, .channel = g_pwm_time1_channel, .channelNum = HAL_ARRAY_SIZE(g_pwm_time1_channel), .mode = HAL_PWM_COUNTER_MODE_UP, .period = 999, .prescaler = 71},
        { .id = HAL_PWM_ID_2, .channel = g_pwm_time2_channel, .channelNum = HAL_ARRAY_SIZE(g_pwm_time2_channel), .mode = HAL_PWM_COUNTER_MODE_UP, .period = 999, .prescaler = 71},
        { .id = HAL_PWM_ID_3, .channel = g_pwm_time3_channel, .channelNum = HAL_ARRAY_SIZE(g_pwm_time3_channel), .mode = HAL_PWM_COUNTER_MODE_UP, .period = 999, .prescaler = 71},
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

/**
 * @brief Get board gpio configuration
 * @param major interface type
 * @param minor sequence
 * @param param pointer to config handle
 * @param info gpio configuration
 * @return HAL_OK if success, otherwase failed
 */
static HAL_Status board_get_pinmux_info(uint32_t major, uint32_t minor, uint32_t param,
                                        struct board_pinmux_info info[])
{
    UNUSED(param);
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
           
            break;
        case HAL_DEV_MAJOR_SDC:
           
            break;
        case HAL_DEV_MAJOR_PWM:
            if (minor == HAL_PWM_ID_1) {
                info[0].pinmux = g_pinmux_pwm1;
                info[0].count = HAL_ARRAY_SIZE(g_pinmux_pwm1);
            }else if (minor == HAL_PWM_ID_2) {
                info[0].pinmux = g_pinmux_pwm2;
                info[0].count = HAL_ARRAY_SIZE(g_pinmux_pwm2);
            }else if (minor == HAL_PWM_ID_3) {
                info[0].pinmux = g_pinmux_pwm3;
                info[0].count = HAL_ARRAY_SIZE(g_pinmux_pwm3);
            }
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
        case HAL_DEV_MAJOR_GPIO:
            info[0].pinmux = g_pinmux_gpio;
            info[0].count = HAL_ARRAY_SIZE(g_pinmux_gpio);
            break;
        case HAL_DEV_MAJOR_GPIO_BUTTON:
            info[0].pinmux = g_pinmux_button;
            info[0].count = HAL_ARRAY_SIZE(g_pinmux_button);
            break;    
        default:
            BOARD_ERR("unknow major %lu\n", major);
            ret = HAL_STATUS_INVALID;
    }

    return ret;
}

/**
 * @brief Get board config
 * @param major interface type
 * @param minor sequence
 * @param param pointer to to config handle
 * @return HAL_OK if success, otherwase failed
 */
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
        case HAL_DEV_MAJOR_ADC:
            if (minor == HAL_ADC_1) {
                board_adc_info_t *info = (board_adc_info_t *)param;
                info->config = (board_adc_info_t *)&g_adc_info[0];
                info->count = 1;
            } else {
                ret = HAL_STATUS_INVALID;
            }
            break;
        case HAL_DEV_MAJOR_GPIO:
            if (minor < HAL_ARRAY_SIZE(g_pinmux_gpio)){
                board_pinmux_info_t *info = (board_pinmux_info_t *) param;
                info->pinmux = &g_pinmux_gpio[minor];
                info->count = 1;;
            }
            break;
        case HAL_DEV_MAJOR_GPIO_BUTTON:
            if (minor < HAL_ARRAY_SIZE(g_pinmux_button)){
                board_pinmux_info_t *info = (board_pinmux_info_t *) param;
                info->pinmux = &g_pinmux_button[minor];
                info->count = 1;;
            }
            break;
        case HAL_DEV_MAJOR_PWM: {
            board_pwm_info_t *info = (board_pwm_info_t *)param;
            info->config = (board_pwm_info_t *)&g_pwm_info[0];
            info->count = HAL_ARRAY_SIZE(g_pwm_info);
            }
            break;
        default:
            BOARD_ERR("unknow major %lu\n", major);
            ret = HAL_STATUS_INVALID;
    }

    return ret;
}

/**
 * @brief set param to board
 * @param req  request cmd
 * @param param0 majon param
 * @param param1 minor param
 * @return 
 */
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

/**
 * @brief board init 
 */
void HAL_BoardInit() {
    HAL_BoardIoctl(HAL_BIR_PINMUX_INIT, HAL_MKDEV(HAL_DEV_MAJOR_GPIO, 0), 0); //gpio init
    HAL_BoardIoctl(HAL_BIR_PINMUX_INIT, HAL_MKDEV(HAL_DEV_MAJOR_GPIO_BUTTON, 0), 0); //button init
}
