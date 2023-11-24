//
// Created by 邓志君 on 2023/3/2.
//

#include "driver.h"
#include "common/framework/util/MiniCommon.h"
#include "driver/hal/hal_gpio.h"
#include "hal_board.h"
#include "main.h"
#include "driver/hal/hal_pwm.h"

#define PWM_HANDLE      HAL_PWM_ID_2
#define  PWM_Period     (99)

typedef enum {
    PWM_ID_0 = 0,
    PWM_ID_1,
    PWM_ID_2,
    PWM_ID_NR,
}PWM_ID;

static void pwmSetDuty(PWM_ID pwmId,  uint16_t duty)
{
    if (duty > PWM_Period) {
        duty = PWM_Period;
    }

    switch (pwmId) {
        case PWM_ID_0:
            HAL_pwmOutput(HAL_PWM_ID_2, HAL_PWM_CH1, duty);
            break;
        case PWM_ID_1:
            HAL_pwmOutput(HAL_PWM_ID_2, HAL_PWM_CH2, duty);
            break;
        case PWM_ID_2:
            HAL_pwmOutput(HAL_PWM_ID_2, HAL_PWM_CH3, duty);
            break;
        default:
            break;
    }
}

static void M1_Enable(void) {
    board_pinmux_info_t pinmux_info;
    HAL_BoardIoctl(HAL_BIR_GET_CFG, HAL_MKDEV(HAL_DEV_MAJOR_GPIO, REMOTE_MOTOR_EN), (uint32_t) &pinmux_info);

    HAL_GpioWritePin(pinmux_info.pinmux[0].port, pinmux_info.pinmux[0].pin, GPIO_PIN_HIGH);
}

static void M1_Disable(void) {
    board_pinmux_info_t pinmux_info;
    HAL_BoardIoctl(HAL_BIR_GET_CFG, HAL_MKDEV(HAL_DEV_MAJOR_GPIO, REMOTE_MOTOR_EN), (uint32_t) &pinmux_info);

    HAL_GpioWritePin(pinmux_info.pinmux[0].port, pinmux_info.pinmux[0].pin, GPIO_PIN_LOW);
}

static void pwmInit(void) {
    HAL_pwmInit(PWM_HANDLE);

    M1_Enable();
    log_d("MOT: Enable 3PWM driver.\r\n");
}

static void setPwm(float Ta, float Tb, float Tc) {
//    log_d("Ta %4.2f %4.2f %4.2f\n",Ta,Tb,Tc);
    pwmSetDuty(PWM_ID_0, (uint16_t)(Ta * PWM_Period));
    pwmSetDuty(PWM_ID_1, (uint16_t)(Tb * PWM_Period));
    pwmSetDuty(PWM_ID_2, (uint16_t)(Tc * PWM_Period));
}

// Set voltage to the pwm pin
static void setPhaseState(int sa, int sb, int sc) {

}

static void pwmEnable(void) {
    M1_Enable();
}

static void pwmDisable(void) {
    M1_Disable();
}

static BldcDriver_t pwm3Driver = {
        .name = "3PWM",
        .init = pwmInit,
        .setPhaseState = setPhaseState,
        .setPwm = setPwm,
        .enable = pwmEnable,
        .disable = pwmDisable,
};

MODULE_INIT static void init(void) {
    MiniBldcDriverManager_register(&pwm3Driver);
}