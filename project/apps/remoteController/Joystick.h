#ifndef __JOYSTICK_H__
#define __JOYSTICK_H__

#include "driver/driver_chip.h"

#define ADC_CHANNEL_NUM 7

typedef struct
{
	uint16_t Ly;																		
	uint16_t Lx;    
	uint16_t Ry;
	uint16_t Rx;
	uint16_t Rz;
	uint16_t Lz;
	uint16_t Battery;

    uint16_t Lx_offset;
    uint16_t Ly_offset;
    uint16_t Rx_offset;
    uint16_t Ry_offset;
}Joystick_t;

Joystick_t* Joystick_Get(void);
void Joystick_Init(void);
Joystick_t* Joystick_GetData(void);
#endif


