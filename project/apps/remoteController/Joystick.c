#include "joystick.h"
#include "stdlib.h"
#include "hal_adc.h"

#define CONTAIN(x, min, max) ((x) < (min) ? (min) : ((x) > (max) ? (max) : (x)))

static uint16_t g_joystickData[ADC_CHANNEL_NUM] = {0};
static Joystick_t g_Joystick;

Joystick_t* Joystick_Get(void) {
    return &g_Joystick;
}

void Joystick_Init(void) {
    HAL_adcInit(HAL_ADC_1);
    HAL_adcStart(HAL_ADC_1);
}


Joystick_t* Joystick_GetData(void) {
    uint32_t temp = 0;
    float battery = 0;

    HAL_adcReadMulti(HAL_ADC_1, g_joystickData, ADC_CHANNEL_NUM, 1000);

    printf("stickData %d %d %d %d %d %d %d\n", g_joystickData[0], g_joystickData[1],
         g_joystickData[2], g_joystickData[3], g_joystickData[4], g_joystickData[5], g_joystickData[6]);

    g_Joystick.Lx = CONTAIN(g_joystickData[1] * 1000/3700 + 1000, 1000, 2000);
    g_Joystick.Ly = CONTAIN(g_joystickData[0] * 1000/3700 + 1000, 1000, 2000);

    g_Joystick.Rx = CONTAIN(g_joystickData[3] * 1000/3700 + 1000, 1000, 2000);
    g_Joystick.Ry = CONTAIN(g_joystickData[2] * 1000/3700 + 1000, 1000, 2000);

    g_Joystick.LZ = CONTAIN(g_joystickData[5] - 600 + 1000, 1000, 2000);
    g_Joystick.Rz = CONTAIN(g_joystickData[4] - 2400 + 1000, 1000, 2000);

    g_Joystick.Battery = g_joystickData[6]*3.3/4096*20;

    printf("joystick %d",g_Joystick.Ly, g_Joystick.Lx, g_Joystick.Ry, g_Joystick.Rx, g_Joystick.Rz, g_Joystick.Battery);

    return &g_Joystick;
}




