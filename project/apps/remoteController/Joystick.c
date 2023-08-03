#include "joystick.h"
#include "stdlib.h"
#include "hal_adc.h"

static Joystick_t g_Joystick;

Joystick_t* Joystick_Get(void) {
    return &g_Joystick;
}

void Joystick_Init(void) {
    HAL_adcInit(HAL_ADC_1);
    HAL_adcStart(HAL_ADC_1);
}


void Joystick_GetData(void) {
    uint32_t temp = 0;
    float battery = 0;

    uint32_t adcLen = HAL_adcGetChannelCount(HAL_ADC_1);
    if (g_Joystick.JoystickBuff == NULL) {
        g_Joystick.JoystickBuff = (uint16_t *) malloc(sizeof(uint16_t) * adcLen);
    }

    HAL_adcReadMulti(HAL_ADC_1, g_Joystick.JoystickBuff, adcLen, 1000);
}




