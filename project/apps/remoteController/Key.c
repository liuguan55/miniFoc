/**
  ******************************************************************************
  * @file    key.c
  * @author  Hare
  * @version V1.0
  * @date    
  * @brief   ����Ӧ�ú����ӿ�
  ******************************************************************************
  */

#include "common/framework/multiButton/multi_button.h"
#include <stdio.h>
#include "common/framework/util/MiniCommon.h"

#undef LOG_TAG
#define LOG_TAG "MiniButton"



static struct Button leftButton;
static struct Button rightButton;
static struct Button confirmButton;
static struct Button cancelButton;

static uint8_t readLeftButtonLevel(uint8_t button_id_) {
    UNUSED(button_id_);

    return HAL_GPIO_ReadPin(BUTTON_LEFT_Port, BUTTON_LEFT_PIN);
}

static uint8_t readRightButtonLevel(uint8_t button_id_) {
    UNUSED(button_id_);

    return HAL_GPIO_ReadPin(BUTTON_RIGHT_Port, BUTTON_RIGHT_PIN);
}

static uint8_t readConfirmButtonLevel(uint8_t button_id_) {
    UNUSED(button_id_);

    return HAL_GPIO_ReadPin(BUTTON_CONFIRM_Port, BUTTON_CONFIRM_PIN);
}

static uint8_t readCancelButtonLevel(uint8_t button_id_) {
    UNUSED(button_id_);

    return HAL_GPIO_ReadPin(BUTTON_CANCEL, BUTTON_CANCEL_PIN);
}

static void buttonCallback(void *args) {
    struct Button *btn = (struct Button *) args;

    button_press_pending_flag = 1;
//    log_d("id %d event %d", btn->button_id, btn->event);
    switch (btn->button_id) {
        case 0: {
//	  log_d("press left\n");
        }
            break;
        case 1: {
//	  log_d("press right\n");
        }
            break;
        case 2: {
//	  log_d("press confirm\n");
        }
            break;
        case 3: {
//	  log_d("press press\n");
        }
            break;
    }
}

static void buttonTickTask(void *args) {
    UNUSED(args);

    button_ticks();
}

void keyInit(void) {
    button_init(&leftButton, readLeftButtonLevel, 0, 0);
    button_attach(&leftButton, SINGLE_CLICK, buttonCallback);
    button_start(&leftButton);

    button_init(&rightButton, readRightButtonLevel, 0, 1);
    button_attach(&rightButton, SINGLE_CLICK, buttonCallback);
    button_start(&rightButton);

    button_init(&confirmButton, readConfirmButtonLevel, 0, 2);
    button_attach(&confirmButton, SINGLE_CLICK, buttonCallback);
    button_start(&confirmButton);

    button_init(&cancelButton, readCancelButtonLevel, 0, 3);
    button_attach(&cancelButton, SINGLE_CLICK, buttonCallback);
    button_start(&cancelButton);

#ifdef USE_RTOS_SYSTEM
    osTimerId_t btnTimer = osTimerNew(buttonTickTask, osTimerPeriodic, NULL, NULL);
    osTimerStart(btnTimer, 10);
#endif
}