//
// Created by 邓志君 on 2023/1/15.
//

#ifndef MINI_FOC_LIBS_SIMPLEFOC_SIMPLEFOC_H_
#define MINI_FOC_LIBS_SIMPLEFOC_SIMPLEFOC_H_
#include "lowpass_filter.h"
#include "pid.h"
#include "foc_utils.h"
#include "sensor/Sensor.h"
#include "driver/driver.h"
#include "current/Current.h"
#include "focConf.h"

#define _HIGH_IMPEDANCE (0)
#define _ACTIVE 		(1)

typedef enum{
  OPEN_LOOP_POSITION_CONTROL = 0,
  OPEN_LOOP_SPEED_CONTROL,
  TORQUE_CONTROL,
  SPEED_CONTROL,
  POSITION_CONTROL,
  SPRING,
  SPRING_WITH_DAMP,
  DAMP,
  KNOB,
  ZERO_RESISTANCE
} FOC_CONTROL_MODE;

#define FOC_CONTROL_MODE_NUM 10

/**
 *  Direction structure
 */
typedef enum
{
  CW      = 1,  //clockwise
  CCW     = -1, // counter clockwise
  UNKNOWN = 0   //not yet known or invalid state
} Direction;
/**
 *  Motiron control type
 */
typedef enum
{
  Type_torque,//!< Torque control
  Type_velocity,//!< Velocity motion control
  Type_angle,//!< Position/angle motion control
  Type_spring,
  Type_springWithDump,
  Type_dump,
  Type_zero_resistance,
  Type_KNOB,
  Type_velocity_openloop,
  Type_angle_openloop,
  Type_control_end,
} MotionControlType;

/**
 *  Motiron control type
 */
typedef enum
{
  Type_voltage, //!< Torque control using voltage
  Type_dc_current, //!< Torque control using DC current (one current magnitude)
  Type_foc_current, //!< torque control using dq currents
  Type_Torque_end,
} TorqueControlType;

/**
 *  FOC modulation type
 */
typedef enum
{
  SinePWM, //!< Sinusoidal PWM modulation
  SpaceVectorPWM, //!< Space vector modulation method
  Trapezoid_120,
  Trapezoid_150
}FOCModulationType;

typedef enum{
  MOTOR_TYPE_BLDC = 0,
  MOTOR_TYPE_STEPPER,
}MotorType_t;

enum{
  MINIFOC_OK,
  MINIFOC_NOT_SENSOR,
  MINIFOC_NOT_DRIVER,
  MINIFOC_NOT_SENSE,
  MINIFOC_INVALID_CONTROLLER,
  MINIFOC_INVALID_TORQUE,
  MINIFOC_VBUS_ERROR,
  MINIFOC_DISABLED,
  MINIFOC_NOT_FOUND_INDEX,
};

typedef struct {
  MotorType_t motorType;
  float target;
  int8_t enabled ;

  TorqueControlType torque_controller;
  MotionControlType controller;

  LowPassFilter  LPF_current_q;
  LowPassFilter  LPF_current_d;
  LowPassFilter  LPF_velocity;
  LowPassFilter  LPF_vBus;
  LowPassFilter  LPF_spring;

  PIDController  PID_current_q;
  PIDController  PID_current_d;
  PIDController  PID_velocity;
  PIDController  P_angle;
  PIDController  P_spring;
  PIDController  P_springWithDump;
  PIDController  P_dump;
  PIDController  P_zero_resistance;
  PIDController  P_knod;

  FocSensor_t *sensor;
}Motor_t;

typedef struct {
  Direction sensor_direction;
  float voltage_power_supply;
  float voltage_limit;
  float velocity_index_search;
  float voltage_sensor_align;
  int pole_pairs;
  float velocity_limit;
  float current_limit;

  float sensor_offset;
  float zero_electric_angle;

  FOCModulationType foc_modulation;
  int modulation_centered;
}BLDCConfig_t;

typedef struct {
  uint32_t open_loop_timestamp;

  float shaft_angle;//!< current motor angle
  float electrical_angle;
  float shaft_velocity;
  float current_sp;
  float shaft_velocity_sp;
  float shaft_angle_sp;
  DQVoltage_s voltage;
  DQCurrent_s current;

  float Ua;
  float Ub;
  float Uc;

  float vbus;
}BLDCContext_t;

typedef struct {
  Motor_t motorBase;
  BldcDriver_t *driver;
  FocCurrent_t *currentSense;

  BLDCConfig_t config;
  BLDCContext_t ctx;
}BLDCMotor_t;

typedef struct {
  Direction sensor_direction;
  float voltage_power_supply;
  float voltage_limit;
  float velocity_index_search;
  float voltage_sensor_align;
  int pole_pairs;
  float velocity_limit;
  float current_limit;

  float sensor_offset;
  float zero_electric_angle;

  FOCModulationType foc_modulation;
  int modulation_centered;
}StepperConfig_t;

typedef struct {
  uint32_t open_loop_timestamp;

  float shaft_angle;//!< current motor angle
  float electrical_angle;
  float shaft_velocity;
  float current_sp;
  float shaft_velocity_sp;
  float shaft_angle_sp;
  DQVoltage_s voltage;
  DQCurrent_s current;

  float Ua;
  float Ub;
  float Uc;

  float vbus;
}StepperContext_t;

typedef struct {
  Motor_t motorBase;
  StepperDriver_t *driver;
  FocCurrent_t *currentSense;

  StepperConfig_t config;
  StepperContext_t ctx;
}StepperMotor_t;


int MiniFoc_init(Motor_t *motor, MotorType_t motorType, char *sensorName, char *driverName);
int MiniFoc_run(Motor_t *motor);
int MiniFoc_setTarget(Motor_t *motor, int target);
int MiniFoc_setController(Motor_t *motor, MotionControlType controller);
int MiniFoc_setTorque(Motor_t *motor, TorqueControlType torque);
int MiniFoc_setZeroElectricOffset(Motor_t *motor, float zero_electric_offset);
int MiniFoc_setSensorDirection(Motor_t *motor, Direction sensor_direction);
#endif //MINI_FOC_LIBS_SIMPLEFOC_SIMPLEFOC_H_
