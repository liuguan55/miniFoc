
#include "sys/MiniDebug.h"
#include "driver/hal/hal_os.h"
#include "driver/hal/hal_usb.h"
#include "log/easylogger/elog.h"
#include "driver/hal/hal_timer.h"
#include "driver/hal/hal_board.h"
#include "driver/hal/hal_dev.h"
#include "driver/hal/hal_gpio.h"

#undef LOG_TAG
#define LOG_TAG "BUZZER"


/**
 * @brief buzzer on
 */
void buzzerOn(){
    board_pinmux_info_t pinmux_info;
    HAL_BoardIoctl(HAL_BIR_GET_CFG, HAL_MKDEV(HAL_DEV_MAJOR_BUZZER, 0), (uint32_t)&pinmux_info);

    HAL_GpioWritePin(pinmux_info.pinmux[0].port, pinmux_info.pinmux[0].pin, 1);
    HAL_msleep(500);
    HAL_GpioWritePin(pinmux_info.pinmux[0].port, pinmux_info.pinmux[0].pin, 0);
}