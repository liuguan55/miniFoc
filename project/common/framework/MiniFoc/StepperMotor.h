//
// Created by 邓志君 on 2023/3/14.
//

#ifndef MINIFOC_F4_LIBS_MINIFOC_STEPPERMOTOR_H_
#define MINIFOC_F4_LIBS_MINIFOC_STEPPERMOTOR_H_


#include "MiniFoc.h"

/******************************************************************************/
void StepperMotor_initFOC(Motor_t *motorBase, float zero_electric_offset, Direction _sensor_direction);
void StepperMotor_loopFOC(Motor_t *motorBase);
void StepperMotor_move(Motor_t *motorBase, float new_target);
void StepperMotor_setPhaseVoltage(Motor_t *motorBase, float Uq, float Ud, float angle_el);
int  StepperMotor_alignSensor(Motor_t *motorBase);
float StepperMotor_velocityOpenloop(Motor_t *motorBase, float target_velocity);
float StepperMotor_angleOpenloop(Motor_t *motorBase, float target_angle);


#endif //MINIFOC_F4_LIBS_MINIFOC_STEPPERMOTOR_H_
