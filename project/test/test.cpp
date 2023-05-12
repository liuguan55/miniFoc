//
// Created by 86189 on 2023/4/27.
//
#include <stdio.h>
#include "AP_HAL/AP_HAL_Board.h"
#include "app.h"
#include "unity/unity.h"
#include "UnityManager.h"

extern "C" void app_init(void){
    printf("enter app.\n");

    hal.init(1, nullptr);

    UnityManager_run();
}