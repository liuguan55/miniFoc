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

#include <stdbool.h>
#include "driver/driver_chip.h"
#include "driver/hal/hal_gpio.h"
#include "driver/hal/hal_base.h"

#define GPIO_PIN_TO_NUM(GPIO,pin)    ((GPIO) * 16 + (pin))
#define GPIO_NUM_TO_PIN(num)         ((num) % 16)
#define GPIO_NUM_TO_PORT(num)        ((num) / 16)
#define GPIO_MAX_PORT_NUM (HAL_GPIO_PORT_NR)
#define GPIO_MAX_NUM  (HAL_GPIO_PORT_NR * 16)
/* useful macros */
#define GPIO_PINS(PinNum)		(1U << PinNum)
#define GPIO_PINS_MASK(pinNum)	((1U << pinNum) - 1)


typedef struct {
    GPIO_IRQCallback   callback;
    void              *arg;
} GPIO_PinCallback;

typedef struct {
    GPIO_InitTypeDef   *initTypeDef[GPIO_MAX_NUM];
    GPIO_PinCallback   pinCb[HAL_GPIO_PIN_NR];
} GPIO_Private;


static const uint8_t gGpioIrqPinNum[HAL_GPIO_PIN_NR] = {
        EXTI0_IRQn,
        EXTI1_IRQn,
        EXTI2_IRQn,
        EXTI3_IRQn,
        EXTI4_IRQn,
        EXTI9_5_IRQn,
        EXTI9_5_IRQn,
        EXTI9_5_IRQn,
        EXTI9_5_IRQn,
        EXTI9_5_IRQn,
        EXTI15_10_IRQn,
        EXTI15_10_IRQn,
        EXTI15_10_IRQn,
        EXTI15_10_IRQn,
        EXTI15_10_IRQn,
        EXTI15_10_IRQn,
};

static  GPIO_TypeDef * const gGpioPortCtrl[HAL_GPIO_PORT_NR] = {
        GPIOA,
        GPIOB,
        GPIOC,
        GPIOD,
        GPIOE,
};

static const uint32_t gGpioModeMap[HAL_GPIO_MODE_NR] = {
        GPIO_MODE_INPUT,
        GPIO_MODE_OUTPUT_PP,
        GPIO_MODE_OUTPUT_OD,
        GPIO_MODE_IT_RISING,
        GPIO_MODE_IT_FALLING,
        GPIO_MODE_IT_RISING_FALLING,
        GPIO_MODE_ANALOG,
        GPIO_MODE_AF_PP,
        GPIO_MODE_AF_OD,
};

static const uint32_t gGpioPullMap[HAL_GPIO_PULL_NR] = {
        GPIO_NOPULL,
        GPIO_PULLUP,
        GPIO_PULLDOWN,
};

static const uint32_t gGpioSpeedMap[HAL_GPIO_DRIVING_LEVEL_NR] = {
        GPIO_SPEED_FREQ_LOW,
        GPIO_SPEED_FREQ_MEDIUM,
        GPIO_SPEED_FREQ_HIGH,
#ifdef TARGET_MCU_STM32F4
        GPIO_SPEED_FREQ_VERY_HIGH,
#endif
};

static const uint32_t gGpioIrqEvent[GPIO_IRQ_EVT_NR] = {
        GPIO_MODE_IT_RISING,
        GPIO_MODE_IT_FALLING,
        GPIO_MODE_IT_RISING_FALLING,
};

static uint32_t gGpioPinStatus[HAL_GPIO_PORT_NR];
static GPIO_Private gGpioPrivate;

static void GPIO_clockInit(GPIO_Port port);
static void GPIO_clockDeinit(GPIO_Port port);
/**
 * @brief if all pins of the port are disabled, disable the clock of the port
 * @param port
 * @return
 */
__STATIC_INLINE bool GPIO_IsAllPinDisabled(GPIO_Port port)
{
    return (gGpioPinStatus[port] == 0);
}

void GPIO_SetPinStatus(GPIO_Port port, GPIO_Pin pin)
{
    if (GPIO_IsAllPinDisabled(port)) {
        GPIO_clockInit(port);
    }

    HAL_SET_BIT(gGpioPinStatus[port], HAL_BIT(pin));
}

__STATIC_INLINE void GPIO_ClearPinStatus(GPIO_Port port, GPIO_Pin pin)
{
    HAL_CLR_BIT(gGpioPinStatus[port], HAL_BIT(pin));

    if (GPIO_IsAllPinDisabled(port)) {
        GPIO_clockDeinit(port);
    }
}

/**
 * @brief This function handles EXTI line0 interrupt.
 * @param GPIO_Pin
 */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    if (gGpioPrivate.pinCb[GPIO_Pin].callback){
        gGpioPrivate.pinCb[GPIO_Pin].callback(gGpioPrivate.pinCb[GPIO_Pin].arg);
    }
}


/**
 * @brief get gpio instanceet
 * @param port
 * @return
 */
__STATIC_INLINE GPIO_TypeDef *GPIO_GetCtrlInstance(GPIO_Port port)
{
    return gGpioPortCtrl[port];
}

/**
 * @brief get gpio init handle
 * @param gpioNum
 * @return
 */
__STATIC_INLINE GPIO_InitTypeDef *GPIO_GetInitHandle(uint32_t gpioNum)
{
    return gGpioPrivate.initTypeDef[gpioNum];
}

/**
 * @brief set gpio init handle
 * @param gpioNum
 * @param initTypeDef
 * @return
 */
__STATIC_INLINE uint32_t GPIO_SetInitHandle(uint32_t gpioNum, GPIO_InitTypeDef *initTypeDef)
{
    gGpioPrivate.initTypeDef[gpioNum] = initTypeDef;

    return 0;
}

/**
 * @brief init clock according to gpio port
 * @param port
 */
static void GPIO_clockInit(GPIO_Port port)
{
    switch (port) {
        case HAL_GPIO_PORT_A:
            __HAL_RCC_GPIOA_CLK_ENABLE();
            break;
        case HAL_GPIO_PORT_B:
            __HAL_RCC_GPIOB_CLK_ENABLE();
            break;
        case HAL_GPIO_PORT_C:
            __HAL_RCC_GPIOC_CLK_ENABLE();
            break;
        case HAL_GPIO_PORT_D:
            __HAL_RCC_GPIOD_CLK_ENABLE();
            break;
        case HAL_GPIO_PORT_E:
            __HAL_RCC_GPIOE_CLK_ENABLE();
            break;
        default:
            break;
    }
}

static void GPIO_clockDeinit(GPIO_Port port)
{
    switch (port) {
        case HAL_GPIO_PORT_A:
            __HAL_RCC_GPIOA_CLK_DISABLE();
            break;
        case HAL_GPIO_PORT_B:
            __HAL_RCC_GPIOB_CLK_DISABLE();
            break;
        case HAL_GPIO_PORT_C:
            __HAL_RCC_GPIOC_CLK_DISABLE();
            break;
        case HAL_GPIO_PORT_D:
            __HAL_RCC_GPIOD_CLK_DISABLE();
            break;
        case HAL_GPIO_PORT_E:
            __HAL_RCC_GPIOE_CLK_DISABLE();
            break;
        default:
            break;
    }
}

/**
 * @brief Initialize the specified GPIO
 * @param[in] port GPIO port
 * @param[in] pin GPIO pin number
 * @param[in] param Pointer to GPIO_InitParam structure
 * @return None
 */
HAL_Status HAL_GpioInit(GPIO_Port port, GPIO_Pin pin, const GPIO_InitParam *param)
{
    GPIO_SetPinStatus(port, pin);

    GPIO_InitTypeDef *pInitTypeDef = GPIO_GetInitHandle(GPIO_PIN_TO_NUM(port, pin));
    if (pInitTypeDef == NULL) {
        pInitTypeDef = (GPIO_InitTypeDef *)HAL_Malloc(sizeof(GPIO_InitTypeDef));
        if (pInitTypeDef == NULL) {
            return HAL_STATUS_MEM_FAILED;
        }

        GPIO_SetInitHandle(GPIO_PIN_TO_NUM(port, pin), pInitTypeDef);
    }else {
        printf("[%d:%d]gpio already init\n", port, pin);
        return HAL_STATUS_BUSY;
    }

    memset(pInitTypeDef, 0, sizeof(GPIO_InitTypeDef));

    GPIO_TypeDef *gpiox = GPIO_GetCtrlInstance(port);

    pInitTypeDef->Pin = GPIO_PINS(pin);
    pInitTypeDef->Mode = gGpioModeMap[param->mode];
    pInitTypeDef->Pull = gGpioPullMap[param->pull];
    pInitTypeDef->Speed = gGpioSpeedMap[param->speed];
#ifdef TARGET_MCU_STM32F4
    pInitTypeDef->Alternate = param->alternate;
#endif
    HAL_GPIO_Init(gpiox, pInitTypeDef);
    HAL_GPIO_WritePin(gpiox, GPIO_PINS(pin), param->initLevel);

    return HAL_STATUS_OK;
}

/**
 * @brief Deinitialize the specified GPIO
 * @param[in] port GPIO port
 * @param[in] pin GPIO pin number
 * @return None
 */
void HAL_GpioDeinit(GPIO_Port port, GPIO_Pin pin)
{
    GPIO_ClearPinStatus(port, pin);

    GPIO_TypeDef *gpiox = GPIO_GetCtrlInstance(port);

    HAL_GPIO_DeInit(gpiox, GPIO_PINS(pin));

    HAL_Free(GPIO_GetInitHandle(GPIO_PIN_TO_NUM(port, pin)));
    GPIO_SetInitHandle(GPIO_PIN_TO_NUM(port, pin), NULL);
}

void HAL_GpioSetMode(GPIO_Port port, GPIO_Pin pin, GPIO_WorkMode mode)
{
    GPIO_InitTypeDef *pInitTypeDef = GPIO_GetInitHandle(GPIO_PIN_TO_NUM(port, pin));
    if (pInitTypeDef == NULL) {
        return;
    }
    GPIO_TypeDef *gpiox = GPIO_GetCtrlInstance(port);
    pInitTypeDef->Mode = mode;
    HAL_GPIO_Init(gpiox, pInitTypeDef);
}

void HAL_GpioSetDriving(GPIO_Port port, GPIO_Pin pin, GPIO_DrivingLevel driving)
{
    GPIO_InitTypeDef *pInitTypeDef = GPIO_GetInitHandle(GPIO_PIN_TO_NUM(port, pin));
    if (pInitTypeDef == NULL) {
        return;
    }

    GPIO_TypeDef *gpiox = GPIO_GetCtrlInstance(port);
    pInitTypeDef->Speed = driving;
    HAL_GPIO_Init(gpiox, pInitTypeDef);
}

void HAL_GpioSetPull(GPIO_Port port, GPIO_Pin pin, GPIO_PullType pull)
{
    GPIO_InitTypeDef *pInitTypeDef = GPIO_GetInitHandle(GPIO_PIN_TO_NUM(port, pin));
    if (pInitTypeDef == NULL) {
        return;
    }

    GPIO_TypeDef *gpiox = GPIO_GetCtrlInstance(port);
    pInitTypeDef->Pull = pull;
    HAL_GPIO_Init(gpiox, pInitTypeDef);
}

void HAL_GpioWritePin(GPIO_Port port, GPIO_Pin pin, HAL_GPIO_PinState  state)
{
    GPIO_TypeDef *gpiox = GPIO_GetCtrlInstance(port);

    HAL_GPIO_WritePin(gpiox, GPIO_PINS(pin), (GPIO_PinState) state);
}

HAL_GPIO_PinState HAL_GpioReadPin(GPIO_Port port, GPIO_Pin pin)
{
    GPIO_TypeDef *gpiox = GPIO_GetCtrlInstance(port);

    return (HAL_GPIO_PinState) HAL_GPIO_ReadPin(gpiox, GPIO_PINS(pin));
}

void HAL_GpioTogglePin(GPIO_Port port, GPIO_Pin pin)
{
    GPIO_TypeDef *gpiox = GPIO_GetCtrlInstance(port);

    HAL_GPIO_TogglePin(gpiox, GPIO_PINS(pin));
}

HAL_Status HAL_GpioEnableIRQ(GPIO_Port port, GPIO_Pin pin, const GPIO_IrqParam *param)
{
    GPIO_TypeDef *gpiox = GPIO_GetCtrlInstance(port);
    IRQn_Type irqn = (IRQn_Type)gGpioIrqPinNum[pin];

    if (param->callback == NULL) {
        return HAL_STATUS_ERROR;
    }

    GPIO_InitTypeDef *pInitTypeDef = GPIO_GetInitHandle(GPIO_PIN_TO_NUM(port, pin));
    if (pInitTypeDef == NULL) {
        return HAL_STATUS_ERROR;
    }

    pInitTypeDef->Mode = gGpioIrqEvent[param->event];
    HAL_GPIO_Init(gpiox, pInitTypeDef);

    gGpioPrivate.pinCb[pin].callback = param->callback;
    gGpioPrivate.pinCb[pin].arg = param->arg;

    HAL_NVIC_SetPriority(irqn, param->priority, 0);
    HAL_NVIC_EnableIRQ(irqn);

    return HAL_STATUS_OK;
}

HAL_Status HAL_GpioDisableIRQ(GPIO_Port port, GPIO_Pin pin)
{
    GPIO_TypeDef *gpiox = GPIO_GetCtrlInstance(port);
    IRQn_Type irqn = (IRQn_Type)gGpioIrqPinNum[pin];

    HAL_NVIC_DisableIRQ(irqn);

    GPIO_InitTypeDef *pInitTypeDef = GPIO_GetInitHandle(GPIO_PIN_TO_NUM(port, pin));
    if (pInitTypeDef == NULL) {
        return HAL_STATUS_ERROR;
    }

    pInitTypeDef->Mode = GPIO_MODE_INPUT;
    HAL_GPIO_Init(gpiox, pInitTypeDef);

    gGpioPrivate.pinCb[pin].callback = NULL;
    gGpioPrivate.pinCb[pin].arg = NULL;

    return HAL_STATUS_OK;
}

void HAL_GpioPinMuxConfig(const GPIO_PinMuxParam *param, uint32_t count)
{
    uint32_t i;

    for (i = 0; i < count; i++) {
        HAL_GpioInit(param[i].port, param[i].pin, &param[i].config);
    }
}

void HAL_GpioPinMuxDeConfig(const GPIO_PinMuxParam *param, uint32_t count)
{
    uint32_t i;

    for (i = 0; i < count; i++) {
        HAL_GpioDeinit(param[i].port, param[i].pin);
    }
}