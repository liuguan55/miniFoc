//
// Created by 邓志君 on 2023/1/18.
//
#include "main.h"
#include "multiButton/multi_button.h"
#include "MiniButton.h"
#include <stdio.h>
#include "MiniCommon.h"

#undef LOG_TAG
#define LOG_TAG "MiniButton"

uint8_t button_press_pending_flag = 0;
uint8_t button_left_press_pending_flag = 0;
uint8_t button_right_press_pending_flag = 0;
uint8_t button_confirm_press_pending_flag = 0;
uint8_t button_cancel_press_pending_flag = 0;

struct Button leftButton;
struct Button rightButton;
struct Button confirmButton;
struct Button cancelButton;

uint8_t readLeftButtonLevel(uint8_t button_id_) {
    UNUSED(button_id_);

    return HAL_GPIO_ReadPin(BUTTON_LEFT_Port, BUTTON_LEFT_PIN);
}

uint8_t readRightButtonLevel(uint8_t button_id_) {
    UNUSED(button_id_);

    return HAL_GPIO_ReadPin(BUTTON_RIGHT_Port, BUTTON_RIGHT_PIN);
}

uint8_t readConfirmButtonLevel(uint8_t button_id_) {
    UNUSED(button_id_);

    return HAL_GPIO_ReadPin(BUTTON_CONFIRM_Port, BUTTON_CONFIRM_PIN);
}

uint8_t readCancelButtonLevel(uint8_t button_id_) {
    UNUSED(button_id_);

    return HAL_GPIO_ReadPin(BUTTON_CANCEL, BUTTON_CANCEL_PIN);
}

uint8_t MiniButton_read(uint8_t id) {
    uint8_t buttonState = 0;

    switch (id) {
        case 0: {
            buttonState = readLeftButtonLevel(id);
        }
            break;
        case 1: {
            buttonState = readRightButtonLevel(id);
        }
            break;
        case 2: {
            buttonState = readConfirmButtonLevel(id);
        }
            break;
        case 3: {
            buttonState = readCancelButtonLevel(id);
        }
            break;
        default:
            break;
    }

    return buttonState;
}

void buttonCallback(void *args) {
    struct Button *btn = (struct Button *) args;

    button_press_pending_flag = 1;
    log_d("id %d event %d", btn->button_id, btn->event);
    switch (btn->button_id) {
        case 0: {
            button_left_press_pending_flag = 1;
//	  log_d("press left\n");
        }
            break;
        case 1: {
            button_right_press_pending_flag = 1;
//	  log_d("press right\n");
        }
            break;
        case 2: {
            button_confirm_press_pending_flag = 1;
//	  log_d("press confirm\n");
        }
            break;
        case 3: {
            button_cancel_press_pending_flag = 1;
//	  log_d("press press\n");
        }
            break;
    }
}

void button_reset_all_flags() {
    button_press_pending_flag = 0;

    button_left_press_pending_flag = 0;
    button_right_press_pending_flag = 0;
    button_confirm_press_pending_flag = 0;
    button_cancel_press_pending_flag = 0;
}

void MiniButton_init(void) {
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
}

void buttonTickTask(void *args) {
    UNUSED(args);

    button_ticks();
}

void MiniButton_run(void) {
    osTimerId_t btnTimer = osTimerNew(buttonTickTask, osTimerPeriodic, NULL, NULL);
    osTimerStart(btnTimer, 10);
}

//void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {
//  button_press_pending_flag = 1;
//
//  if (HAL_GPIO_ReadPin(BUTTON_Port, GPIO_Pin) == 0) {
//	switch (GPIO_Pin) {
//	  case BUTTON_LEFT:
//		button_left_press_pending_flag = 1;
//		break;
//	  case BUTTON_RIGHT:
//		button_right_press_pending_flag = 1;
//		break;
//	  case BUTTON_CONFIRM:
//		button_confirm_press_pending_flag = 1;
//		break;
//	  case BUTTON_CANCEL:
//		button_cancel_press_pending_flag = 1;
//		break;
//	}
//  }
//}