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
// Created by 86189 on 2023/8/29.
//
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "controller.h"
#include "motor.h"
#include "commonDefine.h"

#define MOTOR_MAX 1000
#define THROTTLE_MAX 1000
#define STEERING_MAX 1000

typedef  struct {
    int32_t throttle;
    int32_t steering;

    int32_t output[4];

} Controller_t;

static Controller_t gsController;
void controllerInit(void) {
    motorEnable();
    memset(&gsController, 0, sizeof(Controller_t));
}

void controllerRun(int32_t throttle, int32_t steering) {
    if (throttle == 0 && steering == 0) {
        motorDisable();
        return;
    }else {
        motorEnable();
    }

    throttle = CONSTRAIN_INT32(throttle, -THROTTLE_MAX, THROTTLE_MAX);
    steering = CONSTRAIN_INT32(steering, -STEERING_MAX, STEERING_MAX);

    gsController.throttle = throttle;
    gsController.steering = steering;

//    printf("throttle: %d, steering: %d\n", throttle, steering);
    const float steeringAgain = 1.0f;
    steering = steering * steeringAgain;
    gsController.output[0] = throttle + steering;
    gsController.output[1] = throttle - steering;
    gsController.output[2] = throttle + steering;
    gsController.output[3] = throttle - steering;

    for (int i = 0; i < 4; i++) {
        gsController.output[i] = CONSTRAIN_INT32(gsController.output[i], -MOTOR_MAX, MOTOR_MAX);
    }

    motorSetSpeed(gsController.output[0],
                  gsController.output[1], gsController.output[2], gsController.output[3]);
}