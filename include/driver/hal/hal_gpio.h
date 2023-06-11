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

#ifndef MINIFOC_F4_HAL_GPIO_H
#define MINIFOC_F4_HAL_GPIO_H
#include <stdint.h>
#include "driver/hal/hal_def.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
* @brief GPIO port definition
*/
enum {
    HAL_GPIO_PORT_A = 0,
    HAL_GPIO_PORT_B,
    HAL_GPIO_PORT_C,
    HAL_GPIO_PORT_D,
    HAL_GPIO_PORT_E,
    HAL_GPIO_PORT_NR,
};
typedef uint8_t GPIO_Port;

/**
 * @brief GPIO pin number definition
 */
enum {
    HAL_GPIO_PIN_0 = 0,
    HAL_GPIO_PIN_1,
    HAL_GPIO_PIN_2,
    HAL_GPIO_PIN_3,
    HAL_GPIO_PIN_4,
    HAL_GPIO_PIN_5,
    HAL_GPIO_PIN_6,
    HAL_GPIO_PIN_7,
    HAL_GPIO_PIN_8,
    HAL_GPIO_PIN_9,
    HAL_GPIO_PIN_10,
    HAL_GPIO_PIN_11,
    HAL_GPIO_PIN_12,
    HAL_GPIO_PIN_13,
    HAL_GPIO_PIN_14,
    HAL_GPIO_PIN_15,
    HAL_GPIO_PIN_NR,
};
typedef uint8_t GPIO_Pin;


/**
 * @brief GPIO working mode (function) definition
 */
enum {
    HAL_GPIO_MODE_INPUT = 0,
    HAL_GPIO_MODE_OUTPUT_PP,
    HAL_GPIO_MODE_OUTPUT_OD,
    HAL_GPIO_MODE_IT_RISING,
    HAL_GPIO_MODE_IT_FALLING,
    HAL_GPIO_MODE_IT_RISING_FALLING,
    HAL_GPIO_MODE_ANALOG,
    HAL_GPIO_MODE_AF_PP,
    HAL_GPIO_MODE_AF_OD,
    HAL_GPIO_MODE_NR,
};
typedef uint8_t GPIO_WorkMode;

/**
  * @brief   AF 15 selection
  */
#define GPIO_AF15_EVENTOUT      ((uint8_t)0x0F)  /* EVENTOUT Alternate Function mapping */
enum {
    HAL_GPIO_AF0_RTC_50Hz = 0,
    HAL_GPIO_AF0_MCO = 0,
    HAL_GPIO_AF0_TAMPER = 0,
    HAL_GPIO_AF0_SWJ = 0,
    HAL_GPIO_AF0_TRACE = 0,

    HAl_GPIO_AF1_TIM1 = 1,
    HAL_GPIO_AF1_TIM2 = 1,

    HAL_GPIO_AF2_TIM3 = 2,
    HAL_GPIO_AF2_TIM4 = 2,
    HAL_GPIO_AF2_TIM5 = 2,

    HAL_GPIO_AF3_TIM8 = 3,
    HAL_GPIO_AF3_TIM9 = 3,
    HAL_GPIO_AF3_TIM10 = 3,
    HAL_GPIO_AF3_TIM11 = 3,

    HAL_GPIO_AF4_I2C1 = 4,
    HAL_GPIO_AF4_I2C2 = 4,
    HAL_GPIO_AF4_I2C3 = 4,

    HAL_GPIO_AF5_SPI1 = 5,
    HAL_GPIO_AF5_SPI2 = 5,
    HAL_GPIO_AF5_SI2S3ext = 5,

    HAL_GPIO_AF6_SPI3 = 6,
    HAL_GPIO_AF6_I2S2ext = 6,

    HAL_GPIO_AF7_USART1 = 7,
    HAL_GPIO_AF7_USART2 = 7,
    HAL_GPIO_AF7_USART3 = 7,
    HAL_GPIO_AF7_I2S3ext = 7,

    HAL_GPIO_AF8_UART4 = 8,
    HAL_GPIO_AF8_UART5 = 8,
    HAL_GPIO_AF8_USART6 = 8,

    HAL_GPIO_AF9_CAN1 = 9,
    HAL_GPIO_AF9_CAN2 = 9,
    HAL_GPIO_AF9_TIM12 = 9,
    HAL_GPIO_AF9_TIM13 = 9,
    HAL_GPIO_AF9_TIM14 = 9,

    HAL_GPIO_AF10_OTG_FS = 10,
    HAL_GPIO_AF10_OTG_HS = 10,

    HAL_GPIO_AF12_FSMC = 12,
    HAL_GPIO_AF12_OTG_HS_FS = 12,
    HAL_GPIO_AF12_SDIO = 12,

    HAL_GPIO_AF15_EVENTOUT = 15,

    HAL_GPIO_AF_NR = 0XFF,
    HAL_GPIO_AF_NONE = 0,
};
typedef uint8_t GPIO_AF;
/**
 * @brief GPIO pull type definition
 */
enum {
    HAL_GPIO_PULL_NONE = 0,
    HAL_GPIO_PULL_UP,
    HAL_GPIO_PULL_DOWN,
    HAL_GPIO_PULL_NR,
};
typedef uint8_t GPIO_PullType;

/**
 * @brief GPIO driving level definition
 */
enum {
    HAL_GPIO_DRIVING_LEVEL_0 = 0U,
    HAL_GPIO_DRIVING_LEVEL_1 = 1U,
    HAL_GPIO_DRIVING_LEVEL_2 = 2U,
    HAL_GPIO_DRIVING_LEVEL_3 = 3U,
    HAL_GPIO_DRIVING_LEVEL_NR,
};
typedef uint8_t GPIO_DrivingLevel;


/**
 * @brief GPIO interrupt trigger event definition
 */
typedef enum {
    GPIO_IRQ_EVT_RISING_EDGE  = 0U,
    GPIO_IRQ_EVT_FALLING_EDGE ,
    GPIO_IRQ_EVT_BOTH_EDGE   ,
    GPIO_IRQ_EVT_NR ,
} GPIO_IrqEvent;


/**
 * @brief GPIO pin state definition
 */
typedef enum {
    GPIO_PIN_LOW  = 0,
    GPIO_PIN_HIGH = 1,
} HAL_GPIO_PinState;


/**
 * @brief GPIO initialization parameters
 */
typedef struct {
    GPIO_WorkMode mode;
    GPIO_PullType pull;
    GPIO_DrivingLevel speed;
    GPIO_AF     alternate;
} GPIO_InitParam;


/**
 * @brief GPIO pin configuration definitions
 */
typedef struct {
    GPIO_Port port;
    GPIO_Pin pin;
    GPIO_InitParam config;
} GPIO_PinMuxParam;

/** @brief Type define of GPIO IRQ callback function */
typedef void (*GPIO_IRQCallback) (void *arg);

/**
 * @brief GPIO interrupt enable parameters
 */
typedef struct {
    GPIO_IrqEvent       event;
    uint8_t             priority;
    GPIO_IRQCallback    callback;
    void                *arg;
} GPIO_IrqParam;


typedef struct board_pinmux_info {
    const GPIO_PinMuxParam *pinmux;
    uint32_t count;
}board_pinmux_info_t;

#define BOARD_PINMUX_INFO_MAX    2

/**
 * @brief Initialize the specified GPIO
 * @param[in] port GPIO port
 * @param[in] pin GPIO pin number
 * @param[in] param Pointer to GPIO_InitParam structure
 * @return HAL_STATUS_OK on success, otherwise HAL_ERR_PARA
 */
HAL_Status HAL_GpioInit(GPIO_Port port, GPIO_Pin pin, const GPIO_InitParam *param);

/**
 * @brief Deinitialize the specified GPIO
 * @param[in] port GPIO port
 * @param[in] pin GPIO pin number
 * @return None
 * @note After deinitialization, the GPIO is in its reset state:
 *       (GPIOx_Pn_F7_DISABLE, GPIO_DRIVING_LEVEL_1, GPIO_PULL_NONE).
 */
void HAL_GpioDeinit(GPIO_Port port, GPIO_Pin pin);

/**
 * @brief Set work mode of the specified GPIO
 * @param[in] port GPIO port
 * @param[in] pin GPIO pin number
 * @param[in] mode work mode of the specified GPIO
 * @return None
 */
void HAL_GpioSetMode(GPIO_Port port, GPIO_Pin pin, GPIO_WorkMode mode);

/**
 * @brief Set driving level of the specified GPIO
 * @param[in] port GPIO port
 * @param[in] pin GPIO pin number
 * @param[in] driving driving level of the specified GPIO
 * @return None
 */
void HAL_GpioSetDriving(GPIO_Port port, GPIO_Pin pin, GPIO_DrivingLevel driving);

/**
 * @brief Set pull type of the specified GPIO
 * @param[in] port GPIO port
 * @param[in] pin GPIO pin number
 * @param[in] pull pull type of the specified GPIO
 * @return None
 */
void HAL_GpioSetPull(GPIO_Port port, GPIO_Pin pin, GPIO_PullType pull);

/**
 * @brief Set the state of the specified GPIO
 * @param[in] port GPIO port
 * @param[in] pin GPIO pin number
 * @param[in] state GPIO pin state
 * @return None
 */
void HAL_GpioWritePin(GPIO_Port port, GPIO_Pin pin, HAL_GPIO_PinState state);

/**
 * @brief Get the state of the specified GPIO
 * @param[in] port GPIO port
 * @param[in] pin GPIO pin number
 * @return GPIO pin state
 */
HAL_GPIO_PinState HAL_GpioReadPin(GPIO_Port port, GPIO_Pin pin);

/**
 * @brief Toggle the state of the specified GPIO
 * @param port GPIO port
 * @param pin GPIO port state, bit mask of all pins
 */
void HAL_GpioTogglePin(GPIO_Port port, GPIO_Pin pin);


/**
 * @brief Enable the IRQ of the specified GPIO
 * @param[in] port GPIO port
 * @param[in] pin GPIO pin number
 * @param[in] param Pointer to GPIO_IrqParam structure
 * @retval HAL_Status, HAL_OK on success
 */
HAL_Status HAL_GpioEnableIRQ(GPIO_Port port, GPIO_Pin pin, const GPIO_IrqParam *param);

/**
 * @brief Disable the IRQ of the specified GPIO
 * @param[in] port GPIO port
 * @param[in] pin GPIO pin number
 * @retval HAL_Status, HAL_OK on success
 */
HAL_Status HAL_GpioDisableIRQ(GPIO_Port port, GPIO_Pin pin);

/**
 * @brief Configure the GPIOs pinmux by the specified parameters
 * @param[in] param Pointer to the array of GPIO_PinMuxParam structure, one
 *                  array element for one GPIO pinmux
 * @param[in] count Elements number of the GPIO pinmux parameters array
 * @return None
 */
void HAL_GpioPinMuxConfig(const GPIO_PinMuxParam *param, uint32_t count);

/**
 * @brief Deconfigure the GPIOs pinmux by the specified parameters
 * @param[in] param Pointer to the array of GPIO_PinMuxParam structure, one
 *                  array element for one GPIO pinmux, param->config is ignored.
 * @param[in] count Elements number of the GPIO pinmux parameters array
 * @return None
 */
void HAL_GpioPinMuxDeConfig(const GPIO_PinMuxParam *param, uint32_t count);

#ifdef __cplusplus
}
#endif

#endif //MINIFOC_F4_HAL_GPIO_H
