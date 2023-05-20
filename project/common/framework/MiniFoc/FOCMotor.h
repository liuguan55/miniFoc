#ifndef FOCMOTOR_H
#define FOCMOTOR_H

#include "foc_utils.h"
#include "MiniFoc.h"
/******************************************************************************/

/******************************************************************************/
float shaftAngle(float angle,float sensor_offset,  Direction sensor_direction);
float shaftVelocity(LowPassFilter *LPF_velocity, float velocity,  Direction sensor_direction);
float electricalAngle(float shaft_angle, int pole_pairs, float sensor_offset, float zero_electric_angle);
/******************************************************************************/

#endif

