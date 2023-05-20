//
// Created by 邓志君 on 2023/3/2.
//

#include "MiniMotor.h"
#include "MiniFoc/MiniFoc.h"
#include "MiniCommon.h"

#undef LOG_TAG
#define  LOG_TAG "MOTOR"
static BLDCMotor_t motor = {0};

void MiniMotor_init(void) {
  MiniFoc_setZeroElectricOffset((Motor_t *)&motor, 2.5802);
  MiniFoc_setSensorDirection((Motor_t *)&motor, CW);
  MiniFoc_init((Motor_t *)&motor, MOTOR_TYPE_BLDC, "AS5600@1", "3PWM");
}

void MiniMotor_run(void) {
  MiniFoc_run((Motor_t *)&motor);
}

void MiniMotor_setController(int argc, char *argv[]) {
  if (argc < 2) return;

  int value = atoi(argv[1]);

  if (value < Type_torque || value > Type_angle_openloop) {
    return;
  }

  log_i("controller %d", value);
  MiniFoc_setController((Motor_t *)&motor, value);
}
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0) | SHELL_CMD_TYPE(SHELL_TYPE_CMD_MAIN) | SHELL_CMD_DISABLE_RETURN,
                 focSetControl, MiniMotor_setController, set
                     foc
                     control);

void MiniMotor_setTorque(int argc, char *argv[]) {
  if (argc < 2) return;

  int value = atoi(argv[1]);

  if (value < Type_voltage || value > Type_foc_current) {
    return;
  }

  log_i("torque_controller %d", value);
  MiniFoc_setTorque((Motor_t *)&motor, value);
}
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0) | SHELL_CMD_TYPE(SHELL_TYPE_CMD_MAIN) | SHELL_CMD_DISABLE_RETURN,
                 focSetTorque, MiniMotor_setTorque, set
                     foc
                     torque);

void MiniMotor_setTarget(int argc, char *argv[]) {
  if (argc < 2) return;

  float value = atoff(argv[1]);
  log_i("target %4.2f", value);
  MiniFoc_setTarget((Motor_t *)&motor, value);
}
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0) | SHELL_CMD_TYPE(SHELL_TYPE_CMD_MAIN) | SHELL_CMD_DISABLE_RETURN,
                 focSetTarget, MiniMotor_setTarget, set
                     foc
                     target);