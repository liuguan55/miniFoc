
#include "MiniFoc.h"
#include "StepperMotor.h"
#include "focConf.h"
#include "FOCMotor.h"
#include "log/easylogger/elog.h"
#include "CurrentSense.h"
#include "common/framework/MiniCommon.h"

#undef LOG_TAG
#define LOG_TAG "STEPPER MOTOR"

void StepperMotor_initFOC(Motor_t *motorBase, float zero_electric_offset, Direction _sensor_direction) {
  StepperMotor_t *motor = container_of(motorBase, StepperMotor_t, motorBase);
  StepperConfig_t *config = &motor->config;
  StepperContext_t *ctx = &motor->ctx;
  FocSensor_t *sensor = motor->motorBase.sensor;

  if (zero_electric_offset != 0) {
    // abosolute zero offset provided - no need to align
    config->zero_electric_angle = zero_electric_offset;
  }

  if (_sensor_direction != UNKNOWN) {
    // set the sensor direction - default CW
    config->sensor_direction = _sensor_direction;
  }

  StepperMotor_alignSensor(motorBase);    //检测零点偏移量和极对数

  //shaft_angle update
  shaftVelocity(&motorBase->LPF_velocity,
                sensor->getVelocity(sensor),
                config->sensor_direction);  //getVelocity(),make sure velocity=0 after power on
  MiniFoc_delayMs(50);
  ctx->shaft_velocity = shaftVelocity(&motorBase->LPF_velocity,
                                      sensor->getVelocity(sensor),
                                      config->sensor_direction);;  //必须调用一次，进入主循环后速度为0
  MiniFoc_delayMs(5);
  ctx->shaft_angle =
      shaftAngle(sensor->getAngle(sensor), config->sensor_offset, config->sensor_direction);// shaft angle
  if (motorBase->controller == Type_angle) {
    motorBase->target = ctx->shaft_angle;//角度模式，以当前的角度为目标角度，进入主循环后电机静止
  }

  MiniFoc_delayMs(200);
}

// Encoder alignment the absolute zero angle
// - to the index
int StepperMotor_absoluteZeroSearch(Motor_t *motorBase) {
  StepperMotor_t *motor = container_of(motorBase, StepperMotor_t, motorBase);
  FocSensor_t *sensor = motor->motorBase.sensor;
  StepperConfig_t *config = &motor->config;
  StepperContext_t *ctx = &motor->ctx;

  log_d("MOT: Index search...");
  // search the absolute zero with small velocity
  float limit_vel = config->velocity_limit;
  float limit_volt = config->voltage_limit;
  config->velocity_limit = config->velocity_index_search;
  config->voltage_limit = config->voltage_sensor_align;
  ctx->shaft_angle = 0;

  while (sensor->needsSearch(sensor) && ctx->shaft_angle < _2PI) {
    StepperMotor_angleOpenloop(motorBase, 1.5 * _2PI);
    // call important for some sensors not to loose count
    // not needed for the search
    sensor->getAngle(sensor);
  }

  // disable motor
  StepperMotor_setPhaseVoltage(motorBase, 0, 0, 0);
  // reinit the limits
  config->velocity_limit = limit_vel;
  config->voltage_limit = limit_volt;
  // check if the zero found
  if (sensor->needsSearch(sensor)) {
    log_i("MOT: Error: Not found!");
  } else {
    log_i("MOT: Success!");
  }

  return !sensor->needsSearch(sensor);
}

int StepperMotor_alignSensor(Motor_t *motorBase) {
  int32_t i = 0;
  float angle;
  float mid_angle, end_angle;
  float moved;

  StepperMotor_t *motor = container_of(motorBase, StepperMotor_t, motorBase);
  FocSensor_t *sensor = motor->motorBase.sensor;

  StepperConfig_t *config = &motor->config;
  elog_d("foc", "MOT: Align sensor.\r\n");

  if (config->sensor_direction == UNKNOWN)  //没有设置，需要检测
  {
    // find natural direction
    if (sensor->needsSearch(sensor)) {
      if (StepperMotor_absoluteZeroSearch(motorBase)) {
        return MINIFOC_NOT_FOUND_INDEX;
      }
    }
    // move one electrical revolution forward
    for (i = 0; i <= 500; i++) {
      angle = _3PI_2 + _2PI * i / 500.0;
      StepperMotor_setPhaseVoltage(motorBase, config->voltage_sensor_align, 0, angle);
      MiniFoc_delayMs(2);
    }
    mid_angle = sensor->getAngle(sensor);

    for (i = 500; i >= 0; i--) {
      angle = _3PI_2 + _2PI * i / 500.0;
      StepperMotor_setPhaseVoltage(motorBase, config->voltage_sensor_align, 0, angle);
      MiniFoc_delayMs(2);
    }
    end_angle = sensor->getAngle(sensor);
    StepperMotor_setPhaseVoltage(motorBase, 0, 0, 0);
    MiniFoc_delayMs(200);

    elog_d("foc", "mid_angle=%.4f\r\n", mid_angle);
    elog_d("foc", "end_angle=%.4f\r\n", end_angle);

    moved = fabs(mid_angle - end_angle);
    if ((mid_angle == end_angle) || (moved < 0.01))  //相等或者几乎没有动
    {
      elog_d("foc", "MOT: Failed to notice movement loop222.\r\n");
      motor->driver->disable();    //电机检测不正常，关闭驱动

      return 0;
    } else if (mid_angle < end_angle) {
      elog_d("foc", "MOT: sensor_direction==CCW\r\n");
      config->sensor_direction = CCW;
    } else {
      elog_d("foc", "MOT: sensor_direction==CW\r\n");
      config->sensor_direction = CW;
    }

    elog_d("foc", "MOT: PP check: ");    //计算Pole_Pairs
    if (fabs(moved * config->pole_pairs - _2PI) > 0.5)  // 0.5 is arbitrary number it can be lower or higher!
    {
      elog_d("foc", "fail - estimated pp:");
      config->pole_pairs = _2PI / moved + 0.5;     //浮点数转整形，四舍五入
      elog_d("foc", "%d\r\n", config->pole_pairs);
    } else
      elog_d("foc", "OK!\r\n");
  } else
    elog_d("foc", "MOT: Skip dir calib.\r\n");

  if (config->zero_electric_angle == 0)  //没有设置，需要检测
  {
    StepperMotor_setPhaseVoltage(motorBase, config->voltage_sensor_align, 0, _3PI_2);  //计算零点偏移角度
    MiniFoc_delayMs(700);
    float angle = sensor->getAngle(sensor);;
    config->zero_electric_angle =
        _normalizeAngle(_electricalAngle(config->sensor_direction * angle, config->pole_pairs));
    MiniFoc_delayMs(20);
    elog_d("foc", "MOT: Zero elec. angle:");
    elog_d("foc", "%.4f\r\n", config->zero_electric_angle);
    StepperMotor_setPhaseVoltage(motorBase, 0, 0, 0);
    MiniFoc_delayMs(200);
  } else {
    elog_d("foc", "MOT: Skip offset calib.\r\n");
  }

  return MINIFOC_OK;
}

void StepperMotor_loopFOC(Motor_t *motorBase) {
  StepperMotor_t *motor = container_of(motorBase, StepperMotor_t, motorBase);

  if (motorBase->controller == Type_angle_openloop || motorBase->controller == Type_velocity_openloop) {
    return;
  }

  StepperContext_t *ctx = &motor->ctx;
  StepperConfig_t *config = &motor->config;
  FocSensor_t *sensor = motor->motorBase.sensor;

  ctx->shaft_angle =
      shaftAngle(sensor->getAngle(sensor), config->sensor_offset, config->sensor_direction);;// shaft angle
  ctx->electrical_angle = electricalAngle(ctx->shaft_angle,
                                          config->pole_pairs,
                                          config->sensor_offset,
                                          config->zero_electric_angle);// electrical angle - need shaftAngle to be called first

  ctx->voltage.q = ctx->current_sp;
  ctx->voltage.d = 0;
  // set the phase voltage - FOC heart function :)
  StepperMotor_setPhaseVoltage(motorBase, ctx->voltage.q, ctx->voltage.d, ctx->electrical_angle);
}
/******************************************************************************/
void StepperMotor_move(Motor_t *motorBase, float new_target) {
  StepperMotor_t *motor = container_of(motorBase, StepperMotor_t, motorBase);

  FocSensor_t *sensor = motor->motorBase.sensor;
  StepperContext_t *ctx = &motor->ctx;
  StepperConfig_t *config = &motor->config;

  ctx->shaft_velocity = shaftVelocity(&motorBase->LPF_velocity, sensor->getVelocity(sensor), config->sensor_direction);;

  static uint32_t start = 0;
  if (MiniFoc_millis() - start > 3000) {
    start = MiniFoc_millis();
    log_i("shaft_angle %4.2f shaft_velocity %4.2f", ctx->shaft_angle, ctx->shaft_velocity);
  }

  float now_angle, angle_error;
  switch (motorBase->controller) {
    case Type_torque: ctx->current_sp = new_target; // if current/foc_current torque control
      break;
    case Type_angle:
      // angle set point
      ctx->shaft_angle_sp = new_target;
      // calculate velocity set point
      ctx->shaft_velocity_sp = PIDoperator(&motorBase->P_angle, (ctx->shaft_angle_sp - ctx->shaft_angle));
      // calculate the torque command
      ctx->current_sp = PIDoperator(&motorBase->PID_velocity,
                                    (ctx->shaft_velocity_sp - ctx->shaft_velocity)); // if voltage torque control
      break;
    case Type_spring:
      // angle set point
      new_target = _normalizeAngle(new_target);
      now_angle = _normalizeAngle(ctx->shaft_angle);
      angle_error = new_target - now_angle;
      if (angle_error < -_PI) new_target += _2PI;
      else if (angle_error > _PI) new_target -= _2PI;

      // calculate the torque command
      ctx->current_sp = LPFoperator(&motorBase->LPF_spring,
                                    PIDoperator(&motorBase->P_spring,
                                                (new_target - now_angle))); // if voltage torque control
      break;
    case Type_springWithDump:
      // angle set point
      new_target = _normalizeAngle(new_target);
      now_angle = _normalizeAngle(ctx->shaft_angle);
      angle_error = new_target - now_angle;
      if (angle_error < -_PI) new_target += _2PI;
      else if (angle_error > _PI) new_target -= _2PI;

      // calculate the torque command
      ctx->current_sp = LPFoperator(&motorBase->LPF_spring,
                                    PIDoperator(&motorBase->P_springWithDump,
                                                (new_target - now_angle))); // if voltage torque control
      break;
    case Type_dump:
      // angle set point
      new_target = _normalizeAngle(new_target);
      now_angle = _normalizeAngle(ctx->shaft_angle);
      angle_error = new_target - now_angle;
      if (angle_error < -_PI) new_target += _2PI;
      else if (angle_error > _PI) new_target -= _2PI;

      // calculate the torque command
      ctx->current_sp = LPFoperator(&motorBase->LPF_spring,
                                    PIDoperator(&motorBase->P_dump,
                                                (new_target - now_angle))); // if voltage torque control
      break;
    case Type_zero_resistance:
      // angle set point
      new_target = _normalizeAngle(new_target);
      now_angle = _normalizeAngle(ctx->shaft_angle);
      angle_error = new_target - now_angle;
      if (angle_error < -_PI) new_target += _2PI;
      else if (angle_error > _PI) new_target -= _2PI;

      // calculate the torque command
      ctx->current_sp = LPFoperator(&motorBase->LPF_spring,
                                    PIDoperator(&motorBase->P_zero_resistance,
                                                (new_target - now_angle))); // if voltage torque control
      break;
    case Type_KNOB:
      // angle set point
      now_angle = _normalizeAngle(ctx->shaft_angle);
      uint8_t now_sector = (uint8_t)floor(now_angle * DEF_KNOD_SECTOR_NUM / _2PI);
      float target_angle = now_sector * _2PI / DEF_KNOD_SECTOR_NUM + _PI / DEF_KNOD_SECTOR_NUM;

      angle_error = target_angle - now_angle;
      if (angle_error < -_PI) target_angle += _2PI;
      else if (angle_error > _PI) target_angle -= _2PI;
      // calculate the torque command
      ctx->current_sp = PIDoperator(&motorBase->P_knod, (target_angle - now_angle)); // if voltage torque control
      break;
    case Type_velocity:
      // velocity set point
      ctx->shaft_velocity_sp = new_target;
      // calculate the torque command
      ctx->current_sp = PIDoperator(&motorBase->PID_velocity,
                                    (ctx->shaft_velocity_sp
                                        - ctx->shaft_velocity)); // if current/foc_current torque control
      break;
    case Type_velocity_openloop:
      // velocity control in open loop
      ctx->shaft_velocity_sp = new_target;
      ctx->current_sp = StepperMotor_velocityOpenloop(motorBase,
                                                      ctx->shaft_velocity_sp); // returns the voltage that is set to the motor
      break;
    case Type_angle_openloop:
      // angle control in open loop
      ctx->shaft_angle_sp = new_target;
      ctx->current_sp =
          StepperMotor_angleOpenloop(motorBase, ctx->shaft_angle_sp); // returns the voltage that is set to the motor
      break;
      case Type_control_end:
          break;
  }
}

void StepperMotor_setPhaseVoltage(Motor_t *motorBase, float Uq, float Ud, float angle_el) {
  StepperMotor_t *motor = container_of(motorBase, StepperMotor_t, motorBase);

  StepperConfig_t *config = &motor->config;
  StepperDriver_t *driver = motor->driver;

  float voltage_limit = config->voltage_limit;
  float voltage_power_supply = config->voltage_power_supply;

  // Sinusoidal PWM modulation
  // Inverse Park transformation

  // angle normalization in between 0 and 2pi
  // only necessary if using _sin and _cos - approximation functions
  angle_el = _normalizeAngle(angle_el);
  float _ca = MINI_COS(angle_el);
  float _sa = MINI_SIN(angle_el);
  // Inverse park transform
  float Ualpha = _ca * Ud - _sa * Uq;  // -sin(angle) * Uq;
  float Ubeta = _sa * Ud + _ca * Uq;    //  cos(angle) * Uq;

  // limit the voltage in driver
  Ualpha = _constrain(Ualpha, 0.0, voltage_limit);
  Ubeta = _constrain(Ubeta, 0.0, voltage_limit);
  // calculate duty cycle
  // limited in [0,1]
  Ualpha = _constrain(Ualpha / voltage_power_supply, 0.0, 1.0);
  Ubeta = _constrain(Ubeta / voltage_power_supply, 0.0, 1.0);
  // set the voltages in hardware
  driver->setPwm(Ualpha, Ubeta);
}
/******************************************************************************/
float StepperMotor_velocityOpenloop(Motor_t *motorBase, float target_velocity) {
  uint32_t now_us;
  float Ts, Uq;

  StepperMotor_t *motor = container_of(motorBase, StepperMotor_t, motorBase);
  StepperContext_t *ctx = &motor->ctx;
  StepperConfig_t *config = &motor->config;

  now_us = MiniFoc_micros(); //_micros();
  if (now_us < ctx->open_loop_timestamp)Ts = (float)(ctx->open_loop_timestamp - now_us) / 9 * 1e-6;
  else
    Ts = (float)(0xFFFFFF - now_us + ctx->open_loop_timestamp) / 9 * 1e-6;
  ctx->open_loop_timestamp = now_us;  //save timestamp for next call
  // quick fix for strange cases (micros overflow)
  if (Ts == 0 || Ts > 0.5) Ts = 1e-3;

  // calculate the necessary angle to achieve target velocity
  ctx->shaft_angle = _normalizeAngle(ctx->shaft_angle + target_velocity * Ts);

  Uq = config->voltage_limit;
  // set the maximal allowed voltage (voltage_limit) with the necessary angle
  StepperMotor_setPhaseVoltage(motorBase, Uq, 0, _electricalAngle(ctx->shaft_angle, config->pole_pairs));

  return Uq;
}

float StepperMotor_angleOpenloop(Motor_t *motorBase, float target_angle) {
  uint32_t now_us;
  float Ts, Uq;

  StepperMotor_t *motor = container_of(motorBase, StepperMotor_t, motorBase);
  StepperContext_t *ctx = &motor->ctx;
  StepperConfig_t *config = &motor->config;

  now_us = MiniFoc_micros(); //_micros();
  if (now_us < ctx->open_loop_timestamp)Ts = (float)(ctx->open_loop_timestamp - now_us) / 9 * 1e-6;
  else
    Ts = (float)(0xFFFFFF - now_us + ctx->open_loop_timestamp) / 9 * 1e-6;
  ctx->open_loop_timestamp = now_us;  //save timestamp for next call
  // quick fix for strange cases (micros overflow)
  if (Ts == 0 || Ts > 0.5) Ts = 1e-3;

  // calculate the necessary angle to move from current position towards target angle
  // with maximal velocity (velocity_limit)
  if (fabs(target_angle - ctx->shaft_angle) > config->velocity_limit * Ts) {
    ctx->shaft_angle += _sign(target_angle - ctx->shaft_angle) * config->velocity_limit * Ts;
    //shaft_velocity = velocity_limit;
  } else {
    ctx->shaft_angle = target_angle;
    //shaft_velocity = 0;
  }

  Uq = config->voltage_limit;
  // set the maximal allowed voltage (voltage_limit) with the necessary angle
  StepperMotor_setPhaseVoltage(motorBase, Uq, 0, _electricalAngle(ctx->shaft_angle, config->pole_pairs));

  return Uq;
}



