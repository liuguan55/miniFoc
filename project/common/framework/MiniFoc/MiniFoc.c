//
// Created by 邓志君 on 2023/1/15.
//
#include "MiniFoc.h"
#include "BLDCMotor.h"
#include "common/framework/MiniCommon.h"

#undef LOG_TAG
#define LOG_TAG "MINIFOC"

int MiniFoc_nativeInit(Motor_t *motor, MotorType_t motorType, char *sensorName, char *driverName) {
  FocSensor_t *sensor = MiniSensorManager_findSensorByName(sensorName);
  if (!sensor) {
    log_e("not find foc sensor\n");
    return MINIFOC_NOT_SENSOR;
  }

  sensor->init(sensor);     //初始化编码器参数 和 I2C或者SPI
  motor->sensor = sensor;

  if (motorType == MOTOR_TYPE_BLDC) {
    BldcDriver_t *driver = MiniBldcDriverManager_findByName(driverName);
    if (!driver) {
      log_e("not find driver\n");
      return MINIFOC_NOT_DRIVER;
    }
    driver->init();

    BLDCMotor_t *bldcMotor = (BLDCMotor_t *)motor;
    bldcMotor->driver = driver;
  }

  return MINIFOC_OK;
}

int MiniFoc_setDefaultConfig(BLDCConfig_t *config) {
  config->voltage_power_supply = DEF_POWER_SUPPLY;   //V 电源电压
  config->pole_pairs = DEF_POLEPAIRS;              //电机极对数，按照实际设置，虽然可以上电检测但有失败的概率
  config->velocity_index_search = DEF_INDEX_SEARCH_TARGET_VELOCITY;
  config->voltage_sensor_align =
      DEF_VOLTAGE_SENSOR_ALIGN;    //V alignSensor() use it，大功率电机设置的值小一点比如0.5-1，小电机设置的大一点比如2-3
  config->voltage_limit = DEF_VOLTAGE_LIMIT;           //V，主要为限制电机最大电流，最大值需小于12/1.732=6.9
  config->velocity_limit = DEF_VEL_LIM;         //rad/s 角度模式时限制最大转速，力矩模式和速度模式不起作用
  config->current_limit = DEF_CURRENT_LIM;          //A，foc_current和dc_current模式限制电流，不能为0。速度模式和位置模式起作用
  if (config->voltage_sensor_align > config->voltage_limit) {
    config->voltage_sensor_align = config->voltage_limit;
  }

  config->foc_modulation = SpaceVectorPWM;
  config->modulation_centered = 1;

  return MINIFOC_OK;
}

int MiniFoc_lpfInit(Motor_t *motor) {
  LPF_init(&motor->LPF_current_q, DEF_CURRENT_Q_FILTER_Tf);
  LPF_init(&motor->LPF_current_d, DEF_CURRENT_D_FILTER_Tf);
  //Tf设置小一点，配合爬升斜率设置PID_velocity.output_ramp，速度切换更平稳；如果没有爬升模式的斜率限制，Tf太小电机容易抖动。
  LPF_init(&motor->LPF_velocity, DEF_VEL_FILTER_Tf);
  LPF_init(&motor->LPF_vBus, DEF_VBUS_FILTER_Tf);
  LPF_init(&motor->LPF_spring, DEF_SPRING_FILTER_Tf);

  return MINIFOC_OK;
}

int MiniFoc_pidControllerInit(Motor_t *motor) {
  float velocity_limit = 0, voltage_limit = 0, current_limit = 0;

  if (motor->motorType == MOTOR_TYPE_BLDC) {
    BLDCMotor_t *bldcMotor = (BLDCMotor_t *)motor;
    BLDCConfig_t *config = &bldcMotor->config;

    velocity_limit = config->velocity_limit;
    voltage_limit = config->voltage_limit;
    current_limit = config->current_limit;
  }

  int PID_velocity_limit = 0;
  // velocity control loop controls current
  // 如果是电压模式限制电压，如果是电流模式限制电流
  if (motor->torque_controller == Type_voltage) {
    PID_velocity_limit = voltage_limit;  //速度模式的电流限制
  } else {
    PID_velocity_limit = current_limit;
  }

  PID_init(&motor->PID_velocity, DEF_PID_VEL_P, DEF_PID_VEL_I, DEF_PID_VEL_D, PID_velocity_limit, DEF_PID_VEL_RAMP);
  PID_init(&motor->P_angle, DEF_P_ANGLE_P, DEF_P_ANGLE_I, DEF_P_ANGLE_D, velocity_limit, DEF_P_ANGLE_RAMP);
  PID_init(&motor->P_spring, DEF_P_SPRING_P, DEF_P_SPRING_I, DEF_P_SPRING_D, DEF_SPRING_VEL_LIM, DEF_P_SPRING_RAMP);
  PID_init(&motor->P_springWithDump,
           DEF_P_SPRING_WITH_DUMP_P,
           DEF_P_SPRING_WITH_DUMP_I,
           DEF_P_SPRING_WITH_DUMP_D,
           DEF_SPRING_WITH_DUMP_VEL_LIM,
           DEF_P_SPRING_WITH_DUMP_RAMP);
  PID_init(&motor->P_dump, DEF_P_DUMP_P, DEF_P_DUMP_I, DEF_P_DUMP_D, DEF_DUMP_VEL_LIM, DEF_P_DUMP_RAMP);
  PID_init(&motor->P_zero_resistance,
           DEF_P_ZERO_RESISTANCE_P,
           DEF_P_ZERO_RESISTANCE_I,
           DEF_P_ZERO_RESISTANCE_D,
           DEF_ZERO_RESISTANCE_VEL_LIM,
           DEF_P_ZERO_RESISTANCE_RAMP);
  PID_init(&motor->P_knod, DEF_P_KNOD_P, DEF_P_KNOD_I, DEF_P_KNOD_D, DEF_KNOD_VEL_LIM, DEF_P_KNOD_RAMP);

  //航模电机，速度闭环，不能大于1，否则容易失控
  // 电流环I参数不太好调试，只用P参数也可以
  PID_init(&motor->PID_current_q,
           DEF_PID_CURR_Q_P,
           DEF_PID_CURR_Q_I,
           DEF_PID_CURR_Q_D,
           voltage_limit,
           DEF_PID_CURR_Q_RAMP);
  PID_init(&motor->PID_current_d,
           DEF_PID_CURR_D_P,
           DEF_PID_CURR_D_I,
           DEF_PID_CURR_D_D,
           voltage_limit,
           DEF_PID_CURR_D_RAMP);

  return MINIFOC_OK;
}

int MiniFoc_init(Motor_t *motor, MotorType_t motorType, char *sensorName, char *driverName) {
  assert(motor != NULL);

  motor->motorType = motorType;
  MiniFoc_nativeInit(motor, motorType, sensorName, driverName);

  if (motorType == MOTOR_TYPE_BLDC) {
    BLDCMotor_t *bldcMotor = (BLDCMotor_t *)motor;
    MiniFoc_setDefaultConfig(&bldcMotor->config);
  }

  MiniFoc_lpfInit(motor);

  MiniFoc_pidControllerInit(motor);

  motor->torque_controller = Type_voltage;//Type_foc_current;  //Type_dc_current;//  Type_foc_current;  //Type_voltage;
  motor->controller = Type_angle_openloop;//Type_velocity;  //Type_torque;  //Type_velocity;  //Type_angle;
  motor->target = 0;

  log_i("MOT: Init");

  if (motorType == MOTOR_TYPE_BLDC) {
    BLDCConfig_t *config = BLDCMotor_getConfig(motor);
    BLDCContext_t *ctx = BLDCMotor_getContext(motor);

    BLDCMotor_initFOC(motor,
                      config->zero_electric_angle,
                      config->sensor_direction);  //(2.2,CW);(0,UNKNOWN);  //如果填入零点偏移角度和方向，将跳过上电检测。电机极对数要设置正确。

    ctx->vbus = LPFoperator(&motor->LPF_vBus, MiniFoc_getVbus());
    elog_d("foc", "vBus:%4.2f ,Motor ready.", ctx->vbus);
  if (ctx->vbus > 6.0f) {
    motor->enabled = 1;
  }
  }

  return MINIFOC_OK;
}

int MiniFoc_run(Motor_t *motor) {
  if (!motor->enabled) {
    return MINIFOC_DISABLED;
  }

  if (motor->motorType == MOTOR_TYPE_BLDC) {
    BLDCMotor_move(motor, motor->target);
    BLDCMotor_loopFOC(motor);
  }

  return MINIFOC_OK;
}

int MiniFoc_setTarget(Motor_t *motor, int target) {
  motor->target = target;

  return MINIFOC_OK;
}

int MiniFoc_setController(Motor_t *motor, MotionControlType controller) {
  if (controller >= Type_control_end) {
    return MINIFOC_INVALID_CONTROLLER;
  }

  motor->controller = controller;

  return MINIFOC_OK;
}

int MiniFoc_setTorque(Motor_t *motor, TorqueControlType torque) {
  if (torque >= Type_Torque_end) {
    return MINIFOC_INVALID_TORQUE;
  }

  motor->torque_controller = torque;

  return MINIFOC_OK;
}

int MiniFoc_setZeroElectricOffset(Motor_t *motor, float zero_electric_offset) {
  if (motor->motorType == MOTOR_TYPE_BLDC) {
    BLDCMotor_t *bldcMotor = (BLDCMotor_t *)motor;
    bldcMotor->config.zero_electric_angle = zero_electric_offset;
  }

  return MINIFOC_OK;
}

int MiniFoc_setSensorDirection(Motor_t *motor, Direction sensor_direction) {
  if (motor->motorType == MOTOR_TYPE_BLDC) {
    BLDCConfig_t *config = BLDCMotor_getConfig(motor);
    config->sensor_direction = sensor_direction;
  }

  return MINIFOC_NOT_SENSE;
}

int MiniFoc_setCurrentSense(Motor_t *motor, char *name) {
  if (motor->motorType == MOTOR_TYPE_BLDC) {
    return -1;
  }

  FocCurrent_t *sense = MiniCurrentManager_findCurrentByName(name);
  if (!sense) {
    return MINIFOC_NOT_SENSE;
  }
  sense->init();

  ((BLDCMotor_t *)motor)->currentSense = sense;

  return MINIFOC_OK;
}

int MiniFoc_disable(Motor_t *motor) {
  if (motor->motorType == MOTOR_TYPE_BLDC) {
    BldcDriver_t *driver = BLDCMotor_getDriver(motor);
    driver->setPwm(0, 0, 0);
    driver->disable();
  } else if (motor->motorType == MOTOR_TYPE_STEPPER) {
  }

  motor->enabled = 0;

  return 0;
}

int MiniFoc_enable(Motor_t *motor) {
  if (motor->motorType == MOTOR_TYPE_BLDC) {
    BldcDriver_t *driver = BLDCMotor_getDriver(motor);
    driver->setPwm(0, 0, 0);
    driver->enable();
  } else if (motor->motorType == MOTOR_TYPE_STEPPER) {
  }

  motor->enabled = 1;

  return 0;
}
