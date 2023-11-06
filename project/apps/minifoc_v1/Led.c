/**
  ******************************************************************************
  * @file    key.c
  * @author  Hare
  * @version V1.0
  * @date    
  * @brief   ����Ӧ�ú����ӿ�
  ******************************************************************************
  */
#include "sys/MiniDebug.h"
#include "driver/hal/hal_os.h"
#include "driver/hal/hal_usb.h"
#include "log/easylogger/elog.h"
#include "driver/hal/hal_timer.h"
#include "driver/hal/hal_board.h"
#include "driver/hal/hal_dev.h"
#include "driver/hal/hal_gpio.h"
#include "main.h"
#include "Led.h"

#undef LOG_TAG
#define LOG_TAG "LED"


void ledOn(){
    board_pinmux_info_t pinmux_info;
    HAL_BoardIoctl(HAL_BIR_GET_CFG, HAL_MKDEV(HAL_DEV_MAJOR_GPIO, REMOTE_LED_ID), (uint32_t)&pinmux_info);

    HAL_GpioWritePin(pinmux_info.pinmux[0].port, pinmux_info.pinmux[0].pin, GPIO_PIN_HIGH);
}

void ledOff(){
    board_pinmux_info_t pinmux_info;
    HAL_BoardIoctl(HAL_BIR_GET_CFG, HAL_MKDEV(HAL_DEV_MAJOR_GPIO, REMOTE_LED_ID), (uint32_t)&pinmux_info);

    HAL_GpioWritePin(pinmux_info.pinmux[0].port, pinmux_info.pinmux[0].pin, GPIO_PIN_LOW);
}

void ledToggle(){
    board_pinmux_info_t pinmux_info;
    HAL_BoardIoctl(HAL_BIR_GET_CFG, HAL_MKDEV(HAL_DEV_MAJOR_GPIO, REMOTE_LED_ID), (uint32_t)&pinmux_info);

    HAL_GpioTogglePin(pinmux_info.pinmux[0].port, pinmux_info.pinmux[0].pin);
}
/**
  * @brief  The led task.
  *
  * @retval int
  */
void ledTask(void  *args){
    UNUSED(args);
    board_pinmux_info_t pinmux_info;
    HAL_BoardIoctl(HAL_BIR_GET_CFG, HAL_MKDEV(HAL_DEV_MAJOR_GPIO, REMOTE_LED_ID), (uint32_t)&pinmux_info);

    HAL_GpioTogglePin(pinmux_info.pinmux[0].port, pinmux_info.pinmux[0].pin);
}

/**
  * @brief  start led task.
  *
  * @retval int
  */
void ledTaskStart(){
    static HAL_Timer timer;
    HAL_TimerInit(&timer, HAL_TIMER_PERIODIC, ledTask, NULL);
    HAL_TimerStart(&timer, 1000);
}
