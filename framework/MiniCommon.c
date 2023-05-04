//
// Created by 邓志君 on 2023/1/14.
//

#include "MiniCommon.h"
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"
#include <stdio.h>

#undef LOG_TAG
#define LOG_TAG "COMMON"

void MiniCommon_led1On(int on) {
  if (on) {
    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_SET);
  } else {
    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_RESET);
  }
}
void MiniCommon_led1Toggle(void) {
  HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_13);
}

void *MiniCommon_malloc(size_t size) {
  void *ptr = malloc(size);
  if (!ptr) {
    log_e("malloc size %d failed");
  }

  return ptr;
}

void *MiniCommon_calloc(size_t memb, size_t size) {
  void *ptr = calloc(memb, size);
  if (!ptr) {
    log_e("calloc size %d failed");
  }

  return ptr;
}

void MiniCommon_free(void *ptr) {
  if (ptr) {
    free(ptr);
  }
}