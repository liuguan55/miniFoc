//
// Created by 86189 on 2023/4/27.
//
#include <stdio.h>
#include "AP_HAL/AP_HAL_Board.h"
#include "app.h"
#include "unity/unity.h"
#include "UnityManager.h"

void setUp(void) {
    // set stuff up here
}

void tearDown(void) {
    // clean stuff up here
}

static void test_app() {
    // All of these should pass
    TEST_ASSERT_EQUAL(1, 1);
    TEST_ASSERT_EQUAL(0, 0);
    TEST_ASSERT_EQUAL(-1, -1);
}

static void test_run(){
    UNITY_BEGIN();
    RUN_TEST(test_app);
    UNITY_END();
}

UnityManager_add_test(test_run);

extern "C" void app_init(void){
    printf("enter app.\n");
    hal.init(1, nullptr);

    UnityManager_run();
}