#ifndef BLDCMotor_H
#define BLDCMotor_H
#include "MiniFoc.h"


/******************************************************************************/
void BLDCMotor_initFOC(Motor_t *motorBase,float zero_electric_offset, Direction _sensor_direction);
void BLDCMotor_loopFOC(Motor_t *motorBase);
void BLDCMotor_move(Motor_t  *motorBase, float new_target);
void BLDCMotor_setPhaseVoltage(Motor_t *motorBase,float Uq, float Ud, float angle_el);
int BLDCMotor_alignSensor(Motor_t *motorBase);
float BLDCMotor_velocityOpenloop(Motor_t *motorBase, float target_velocity);
float BLDCMotor_angleOpenloop(Motor_t *motorBase, float target_angle);
BLDCConfig_t * BLDCMotor_getConfig(Motor_t *motorBase);
BLDCContext_t * BLDCMotor_getContext(Motor_t *motorBase);
BldcDriver_t * BLDCMotor_getDriver(Motor_t *motorBase);
#endif
