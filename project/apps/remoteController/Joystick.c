#include <stdio.h>
#include "joystick.h"
#include "hal_adc.h"
#include "driver/hal/hal_os.h"
#include "event.h"
#include "wireless.h"

#define CONTAIN(x, min, max) ((x) < (min) ? (min) : ((x) > (max) ? (max) : (x)))
typedef struct {
    uint16_t maxLeftX;
    uint16_t maxLeftY;
    uint16_t maxRightX;
    uint16_t maxRightY;
    uint16_t minLeftX;
    uint16_t minLeftY;
    uint16_t minRightX;
    uint16_t minRightY;
    uint16_t deadZone;
}JoyStickCalib_t;

static uint16_t g_joystickData[ADC_CHANNEL_NUM] = {0};
static Joystick_t g_Joystick;
static JoyStickCalib_t g_JoyStickCalib = {
        .maxLeftX = 3722 ,
        .maxLeftY = 3737 ,
        .maxRightX = 3808 ,
        .maxRightY = 3902 ,
        .minLeftX = 206 ,
        .minLeftY = 252 ,
        .minRightX = 351 ,
        .minRightY = 332 ,
        .deadZone = 100,
};


Joystick_t* Joystick_Get(void) {
    return &g_Joystick;
}

void Joystick_callback(uint16_t *data, uint32_t size) {
//    printf("Joystick_callback %d %d %d %d %d %d %d\n", data[0], data[1],
//         data[2], data[3], data[4], data[5], data[6]);

    static uint32_t now = 0;
    if (HAL_millis() - now < 10 || wirelessState()){
        return;
    }
    now = HAL_millis();

    g_Joystick.update = 1;
}

void JoyStick_calibration(void){
    uint32_t maxLeftX = 0;
    uint32_t maxLeftY = 0;
    uint32_t maxRightX = 0;
    uint32_t maxRightY = 0;
    uint32_t minLeftX = UINT32_MAX;
    uint32_t minLeftY = UINT32_MAX;
    uint32_t minRightX = UINT32_MAX;
    uint32_t minRightY = UINT32_MAX;

    uint32_t now = HAL_millis();
    while ((HAL_millis() - now) < 60000){
        HAL_adcReadMulti(HAL_ADC_1, g_joystickData, ADC_CHANNEL_NUM, 1000);
//        printf("stickData %d %d %d %d %d %d %d\n", g_joystickData[0], g_joystickData[1],
//               g_joystickData[2], g_joystickData[3], g_joystickData[4], g_joystickData[5], g_joystickData[6]);

        if (g_joystickData[0] > maxLeftY)
            maxLeftY = g_joystickData[0];

        if (g_joystickData[1] > maxLeftX)
            maxLeftX = g_joystickData[1];

        if (g_joystickData[2] > maxRightY)
            maxRightY = g_joystickData[2];

        if (g_joystickData[3] > maxRightX)
            maxRightX = g_joystickData[3];

        if (g_joystickData[0] < minLeftY)
            minLeftY = g_joystickData[0];

        if (g_joystickData[1] < minLeftX)
            minLeftX = g_joystickData[1];

        if (g_joystickData[2] < minRightY)
            minRightY = g_joystickData[2];

        if (g_joystickData[3] < minRightX)
            minRightX = g_joystickData[3];


        HAL_Delay(100);

        printf("min %lu %lu %lu %lu max %lu %lu %lu %lu\n", minLeftX, minLeftY, minRightX, minRightY,maxLeftX, maxLeftY, maxRightX, maxRightY);
    }


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

}


Joystick_t* Joystick_GetData(void) {
    HAL_adcReadMulti(HAL_ADC_1, g_joystickData, ADC_CHANNEL_NUM, 1000);

//    printf("stickData %d %d %d %d %d %d %d\n", g_joystickData[0], g_joystickData[1],
//         g_joystickData[2], g_joystickData[3], g_joystickData[4], g_joystickData[5], g_joystickData[6]);

    g_Joystick.Lx = abs(g_joystickData[1] - g_JoyStickCalib.minLeftY) * 1.0 / (g_JoyStickCalib.maxLeftY - g_JoyStickCalib.minLeftY) * 1000 + 1000;
    g_Joystick.Lx = CONTAIN(g_Joystick.Lx, 1000, 2000);
    if (abs(g_Joystick.Lx - 1500) < g_JoyStickCalib.deadZone){
        g_Joystick.Lx = 1500;
    }


    g_Joystick.Ly = abs(g_joystickData[0] - g_JoyStickCalib.minLeftX) * 1.0 / (g_JoyStickCalib.maxLeftX - g_JoyStickCalib.minLeftX) * 1000 + 1000;
    g_Joystick.Ly = CONTAIN(g_Joystick.Ly, 1000, 2000);
    if (abs(g_Joystick.Ly - 1500) < g_JoyStickCalib.deadZone){
        g_Joystick.Ly = 1500;
    }

    g_Joystick.Rx = abs(g_joystickData[3] - g_JoyStickCalib.minRightY) * 1.0 / (g_JoyStickCalib.maxRightY - g_JoyStickCalib.minRightY) * 1000 + 1000;
    g_Joystick.Rx = CONTAIN(g_Joystick.Rx, 1000, 2000);
    if (abs(g_Joystick.Rx - 1500) < g_JoyStickCalib.deadZone){
        g_Joystick.Rx = 1500;
    }

    g_Joystick.Ry = abs(g_joystickData[2] - g_JoyStickCalib.minRightX) * 1.0 / (g_JoyStickCalib.maxRightX - g_JoyStickCalib.minRightX) * 1000 + 1000;
    g_Joystick.Ry = CONTAIN(g_Joystick.Ry, 1000, 2000);
    if (abs(g_Joystick.Ry - 1500) < g_JoyStickCalib.deadZone){
        g_Joystick.Ry = 1500;
    }

    g_Joystick.Battery = g_joystickData[6]*3.3/4096*20;

//    printf("joystick lx %d ly %d rx %d ry %d lz %d rz %d bat %d\n", g_Joystick.Lx, g_Joystick.Ly,g_Joystick.Rx, g_Joystick.Ry, g_Joystick.Lz, g_Joystick.Rz, g_Joystick.Battery);

    return &g_Joystick;
}




