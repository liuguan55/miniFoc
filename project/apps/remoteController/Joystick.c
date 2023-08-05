#include <stdio.h>
#include "joystick.h"
#include "hal_adc.h"
#include "driver/hal/hal_os.h"
#include "event.h"

#define CONTAIN(x, min, max) ((x) < (min) ? (min) : ((x) > (max) ? (max) : (x)))

static uint16_t g_joystickData[ADC_CHANNEL_NUM] = {0};
static Joystick_t g_Joystick;

Joystick_t* Joystick_Get(void) {
    return &g_Joystick;
}

void Joystick_callback(uint16_t *data, uint32_t size) {
//    printf("Joystick_callback %d %d %d %d %d %d %d\n", data[0], data[1],
//         data[2], data[3], data[4], data[5], data[6]);

    static uint32_t now = 0;
    if (HAL_millis() - now < 100){
        return;
    }
    now = HAL_millis();

    eventSend(EVENT_TYPE_JOYSTICK, 0, 0);
}

void JoyStick_calibration(void){
    HAL_adcReadMulti(HAL_ADC_1, g_joystickData, ADC_CHANNEL_NUM, 1000);
    printf("stickData %d %d %d %d %d %d %d\n", g_joystickData[0], g_joystickData[1],
           g_joystickData[2], g_joystickData[3], g_joystickData[4], g_joystickData[5], g_joystickData[6]);

    g_Joystick.Lx_offset = g_joystickData[1] - 3700/2 ;
    g_Joystick.Ly_offset = g_joystickData[0] - 3700/2 ;
    g_Joystick.Rx_offset = g_joystickData[3] - 3700/2 ;
    g_Joystick.Ry_offset = g_joystickData[2] - 3700/2 ;

    printf("offset %d %d %d %d\n", g_Joystick.Lx_offset, g_Joystick.Ly_offset, g_Joystick.Rx_offset, g_Joystick.Ry_offset);
}

void Joystick_Init(void) {
    HAL_adcInit(HAL_ADC_1);
    HAL_adcSetCallback(HAL_ADC_1, Joystick_callback);
    HAL_adcStart(HAL_ADC_1);

    JoyStick_calibration();
}


Joystick_t* Joystick_GetData(void) {
    HAL_adcReadMulti(HAL_ADC_1, g_joystickData, ADC_CHANNEL_NUM, 1000);

//    printf("stickData %d %d %d %d %d %d %d\n", g_joystickData[0], g_joystickData[1],
//         g_joystickData[2], g_joystickData[3], g_joystickData[4], g_joystickData[5], g_joystickData[6]);

    g_Joystick.Lx = CONTAIN((g_joystickData[1] - g_Joystick.Lx_offset) * 1000/3700 + 1000, 1000, 2000);
    g_Joystick.Ly = CONTAIN((g_joystickData[0] - g_Joystick.Ly_offset) * 1000/3700 + 1000, 1000, 2000);

    g_Joystick.Rx = CONTAIN((g_joystickData[3] - g_Joystick.Rx_offset) * 1000/3700 + 1000, 1000, 2000);
    g_Joystick.Ry = CONTAIN((g_joystickData[2] - g_Joystick.Ry_offset) * 1000/3700 + 1000, 1000, 2000);

    g_Joystick.Lz = CONTAIN(g_joystickData[5] - 600 + 1000, 1000, 2000);
    g_Joystick.Rz = CONTAIN(g_joystickData[4] - 2400 + 1000, 1000, 2000);

    g_Joystick.Battery = g_joystickData[6]*3.3/4096*20;

//    printf("joystick lx %d ly %d rx %d ry %d lz %d rz %d bat %d\n", g_Joystick.Lx, g_Joystick.Ly,g_Joystick.Rx, g_Joystick.Ry, g_Joystick.Lz, g_Joystick.Rz, g_Joystick.Battery);

    return &g_Joystick;
}




