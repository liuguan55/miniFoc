// /* Copyright (C) 2023 dengzhijun. All rights reserved.
//  *
//  *  Redistribution and use in source and binary forms, with or without
//  *  modification, are permitted provided that the following conditions
//  *  are met:
//  *    1. Redistributions of source code must retain the above copyright
//  *       notice, this list of conditions and the following disclaimer.
//  *    2. Redistributions in binary form must reproduce the above copyright
//  *       notice, this list of conditions and the following disclaimer in the
//  *       documentation and/or other materials provided with the
//  *       distribution.
//  *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
//  *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
//  *  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
//  *  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
//  *  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
//  *  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
//  *  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
//  *  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
//  *  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
//  *  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
//  *  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//  *

//
// Created by 邓志君 on 2023/3/2.
//

#include "MiniMotor.h"
#include "MiniFoc.h"
#include "common/framework/util/MiniCommon.h"

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