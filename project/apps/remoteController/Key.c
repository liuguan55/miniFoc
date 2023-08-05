/**
  ******************************************************************************
  * @file    key.c
  * @author  Hare
  * @version V1.0
  * @date    
  * @brief   ����Ӧ�ú����ӿ�
  ******************************************************************************
  */

#include <stdio.h>
#include "common/framework/util/MiniCommon.h"
#include "common/framework/multiButton/multi_button.h"
#include "driver/hal/hal_board.h"
#include "driver/hal/hal_gpio.h"
#include "key.h"
#include "main.h"
#include "event.h"

#undef LOG_TAG
#define LOG_TAG "MiniButton"



static struct Button leftButton;
static struct Button rightButton;
static struct Button confirmButton;

static uint8_t readLeftButtonLevel(uint8_t button_id_) {
    UNUSED(button_id_);

    board_pinmux_info_t pinmux_info;
    HAL_BoardIoctl(HAL_BIR_GET_CFG, HAL_MKDEV(HAL_DEV_MAJOR_GPIO_BUTTON, REMOTE_BUTTON_LEFT), (uint32_t)&pinmux_info);

    return HAL_GpioReadPin(pinmux_info.pinmux[0].port, pinmux_info.pinmux[0].pin);
}

static uint8_t readRightButtonLevel(uint8_t button_id_) {
    UNUSED(button_id_);

    board_pinmux_info_t pinmux_info;
    HAL_BoardIoctl(HAL_BIR_GET_CFG, HAL_MKDEV(HAL_DEV_MAJOR_GPIO_BUTTON, REMOTE_BUTTON_RIGHT), (uint32_t)&pinmux_info);

    return HAL_GpioReadPin(pinmux_info.pinmux[0].port, pinmux_info.pinmux[0].pin);
}

static uint8_t readConfirmButtonLevel(uint8_t button_id_) {
    UNUSED(button_id_);

    board_pinmux_info_t pinmux_info;
    HAL_BoardIoctl(HAL_BIR_GET_CFG, HAL_MKDEV(HAL_DEV_MAJOR_GPIO_BUTTON, REMOTE_BUTTON_CONFIRM), (uint32_t)&pinmux_info);

    return HAL_GpioReadPin(pinmux_info.pinmux[0].port, pinmux_info.pinmux[0].pin);
}


static void buttonCallback(void *args) {
    struct Button *btn = (struct Button *) args;

//    log_i("id %d event %d", btn->button_id, btn->event);
    switch (btn->button_id) {
        case REMOTE_BUTTON_LEFT:
            eventSend(EVENT_TYPE_KEY, KEY_LEFT, 0);
            break;
        case REMOTE_BUTTON_RIGHT:
            eventSend(EVENT_TYPE_KEY, KEY_RIGHT, 0);
            break;
        case REMOTE_BUTTON_CONFIRM:
            eventSend(EVENT_TYPE_KEY, KEY_OK, 0);
            break;
    }
}

static void buttonTickTask(void *args) {
    UNUSED(args);

    button_ticks();
}

void keyInit(void) {
    button_init(&leftButton, readLeftButtonLevel, 0, REMOTE_BUTTON_LEFT);
    button_attach(&leftButton, SINGLE_CLICK, buttonCallback);
    button_start(&leftButton);

    button_init(&rightButton, readRightButtonLevel, 0, REMOTE_BUTTON_RIGHT);
    button_attach(&rightButton, SINGLE_CLICK, buttonCallback);
    button_start(&rightButton);

    button_init(&confirmButton, readConfirmButtonLevel, 0, REMOTE_BUTTON_CONFIRM);
    button_attach(&confirmButton, SINGLE_CLICK, buttonCallback);
    button_start(&confirmButton);

#ifdef USE_RTOS_SYSTEM
    osTimerId_t btnTimer = osTimerNew(buttonTickTask, osTimerPeriodic, NULL, NULL);
    osTimerStart(btnTimer, 10);
#endif
}