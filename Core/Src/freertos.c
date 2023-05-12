/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "../../framework/MiniCommon.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
#undef LOG_TAG
#define LOG_TAG  "FREERTOS"
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END Variables */
/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */
/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */

/* USER CODE END Variables */
/* Definitions for defaultTask */
osThreadId_t defaultTaskHandle;
const osThreadAttr_t defaultTask_attributes = {
        .name = "defaultTask",
        .stack_size = 4096 * 4,
        .priority = (osPriority_t) osPriorityLow,
};

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

/* USER CODE END FunctionPrototypes */

void StartDefaultTask(void *argument);

extern void MX_USB_DEVICE_Init(void);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
    /* USER CODE BEGIN Init */

    /* USER CODE END Init */

    /* USER CODE BEGIN RTOS_MUTEX */
    /* add mutexes, ... */
    /* USER CODE END RTOS_MUTEX */

    /* USER CODE BEGIN RTOS_SEMAPHORES */
    /* add semaphores, ... */
    /* USER CODE END RTOS_SEMAPHORES */

    /* USER CODE BEGIN RTOS_TIMERS */
    /* start timers, add new ones, ... */
    /* USER CODE END RTOS_TIMERS */

    /* USER CODE BEGIN RTOS_QUEUES */
    /* add queues, ... */
    /* USER CODE END RTOS_QUEUES */

    /* Create the thread(s) */
    /* creation of defaultTask */
    defaultTaskHandle = osThreadNew(StartDefaultTask, NULL, &defaultTask_attributes);

    /* USER CODE BEGIN RTOS_THREADS */
    /* add threads, ... */
    /* USER CODE END RTOS_THREADS */

    /* USER CODE BEGIN RTOS_EVENTS */
    /* add events, ... */
    /* USER CODE END RTOS_EVENTS */

}

/* USER CODE BEGIN Header_StartDefaultTask */
/**
  * @brief  Function implementing the defaultTask thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_StartDefaultTask */
#include "test/test.h"
void StartDefaultTask(void *argument) {
    UNUSED(argument);
    /* init code for USB_DEVICE */
    MX_USB_DEVICE_Init();
    /* USER CODE BEGIN StartDefaultTask */
    platform_init();

    app_init();
    /* Infinite loop */
    for (;;) {
        MiniCommon_led1Toggle();
        MiniCommon_delayMs(1000);
    }


    /* USER CODE END StartDefaultTask */
}

#if 0
/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

void SuspendToRunOtherTask(osThreadId_t other_task) {
  if (other_task == taskSelectTaskHandle) {
    MiniCommon_led1On(LED_ON);
  } else {
    MiniCommon_led1On(LED_OFF);
  }
  button_reset_all_flags();
  FOC_SVPWM(0, 0, 0);
  osThreadResume(other_task);
  osThreadSuspend(osThreadGetId());
}


void TaskSelectTask(void *argument) {
  ST7735_Init();
  ST7735_LedOn();
  gui_draw_init("FOC init", 1);
  FOC_init();
  gui_draw_init("CS init", 0);
//  cs_init();
  gui_draw_init("Calibrating", 0);
  FOC_electrical_angle_calibration();

  FOC_CONTROL_MODE mode = OPEN_LOOP_POSITION_CONTROL;
  portENTER_CRITICAL();
  gui_draw_mode_selection(mode);
  portEXIT_CRITICAL();
  TickType_t xLastWakeTime = xTaskGetTickCount();

  MiniCommon_led2On(LED_ON);
  while (1) {
    if (button_press_pending_flag) {
      if (button_left_press_pending_flag) {
        if (mode) mode--;
        else mode = FOC_CONTROL_MODE_NUM - 1;
        gui_draw_mode_selection(mode);

      } else if (button_right_press_pending_flag) {
        mode++;
        if (mode > FOC_CONTROL_MODE_NUM - 1) mode = 0;
        gui_draw_mode_selection(mode);

      } else if (button_confirm_press_pending_flag) {
        switch (mode) {
          case OPEN_LOOP_POSITION_CONTROL:
            SuspendToRunOtherTask(openLoopPosControlTaskHandle);
            break;
          case OPEN_LOOP_SPEED_CONTROL:
            SuspendToRunOtherTask(openLoopVelControlTaskHandle);
            break;
          case TORQUE_CONTROL:
            SuspendToRunOtherTask(torqueControlTaskHandle);
            break;
          case SPEED_CONTROL:
            SuspendToRunOtherTask(velocityControlTaskHandle);
            break;
          case POSITION_CONTROL:;
            SuspendToRunOtherTask(positionControlTaskHandle);
            break;
          case SPRING:
            spring_pid = &pid_spring;
            SuspendToRunOtherTask(springTaskHandle);
            break;
          case SPRING_WITH_DAMP:
            spring_pid = &pid_spring_with_damp;
            SuspendToRunOtherTask(springTaskHandle);
            break;
          case DAMP:
            spring_pid = &pid_damp;
            SuspendToRunOtherTask(springTaskHandle);
            break;
          case KNOB:
            SuspendToRunOtherTask(knobTaskHandle);
            break;
          case ZERO_RESISTANCE:
            spring_pid = &pid_zero_resistance;
            SuspendToRunOtherTask(springTaskHandle);
            break;
        }
        gui_draw_mode_selection(mode);
        continue;
      }

      button_reset_all_flags();
    }
    vTaskDelayUntil(&xLastWakeTime,
                    10);
  }
}

void OpenLoopPosControlTask(void *argument) {
  float angle = 0, Ud = 1;
  TickType_t xLastWakeTime = xTaskGetTickCount();

  while (1) {
    if (button_press_pending_flag) {
      if (button_left_press_pending_flag) {
        angle -= _PI_2;
      } else if (button_right_press_pending_flag) {
        angle += _PI_2;
      } else if (button_cancel_press_pending_flag) {
        SuspendToRunOtherTask(taskSelectTaskHandle);
        continue;
      }
      log_d("%s %4.2f",__FUNCTION__, angle);
      button_reset_all_flags();
    }
    FOC_SVPWM(0, Ud, angle);
    vTaskDelayUntil(&xLastWakeTime,
                    1); // every FOC control task need at least 1ms delay otherwise cannot detect key press normally
  }
}

void OpenLoopVelControlTask(void *argument) {
  float angle = 0, Ud = 1, delta_angle = 0;
  TickType_t xLastWakeTime = xTaskGetTickCount();
  while (1) {
    if (button_press_pending_flag) {
      if (button_left_press_pending_flag) {
        delta_angle -= 0.01;
        if (delta_angle < -_PI) delta_angle = -_PI;
      } else if (button_right_press_pending_flag) {
        delta_angle += 0.01;
        if (delta_angle > _PI) delta_angle = _PI;
      } else if (button_cancel_press_pending_flag) {
        SuspendToRunOtherTask(taskSelectTaskHandle);
        continue;
      }
      button_reset_all_flags();
    }
    angle += delta_angle;
    FOC_SVPWM(0, Ud, angle);
    vTaskDelayUntil(&xLastWakeTime,
                    1); // every FOC control task need at least 1ms delay otherwise cannot detect key press normally
  }
}

void TorqueControlTask(void *argument) {
  TickType_t xLastWakeTime = xTaskGetTickCount();
  float Uq = 0;

  while (1) {
    if (button_press_pending_flag) {
      if (button_left_press_pending_flag) {
        Uq -= 1.0;
      } else if (button_right_press_pending_flag) {
        Uq += 1.0;
      } else if (button_cancel_press_pending_flag) {
        SuspendToRunOtherTask(taskSelectTaskHandle);
        continue;
      }
      button_reset_all_flags();
    }
    FOC_open_loop_voltage_control_loop(Uq);

    vTaskDelayUntil(&xLastWakeTime,
                    1); // every FOC control task need at least 1ms delay otherwise cannot detect key press normally
  }
}

void VelocityControlTask(void *argument) {
  float velocity = 0;
  TickType_t xLastWakeTime = xTaskGetTickCount();

  while (1) {
    if (button_press_pending_flag) {
      if (button_left_press_pending_flag) {
        velocity -= 5;
      } else if (button_right_press_pending_flag) {
        velocity += 5;
      } else if (button_cancel_press_pending_flag) {
        SuspendToRunOtherTask(taskSelectTaskHandle);
        continue;
      }
      button_reset_all_flags();
    }

    FOC_velocity_control_loop(velocity);
    vTaskDelayUntil(&xLastWakeTime,
                    1); // every FOC control task need at least 1ms delay otherwise cannot detect key press normally
  }
}

void PositionControlTask(void *argument) {
  float angle = 0;
  TickType_t xLastWakeTime = xTaskGetTickCount();
  gui_draw_position_mode(angle, 1);

  while (1) {
    if (button_press_pending_flag) {
      if (button_left_press_pending_flag) {
        angle -= _PI_2;
      } else if (button_right_press_pending_flag) {
        angle += _PI_2;
      } else if (button_cancel_press_pending_flag) {
        SuspendToRunOtherTask(taskSelectTaskHandle);
        gui_draw_position_mode(angle, 1);
        continue;
      }
      button_reset_all_flags();
      gui_draw_position_mode(angle, 0);
    }

    FOC_position_control_loop(angle);
    vTaskDelayUntil(&xLastWakeTime,
                    1); // every FOC control task need at least 1ms delay otherwise cannot detect key press normally
  }
}

void SpringTask(void *argument) {
  TickType_t xLastWakeTime = xTaskGetTickCount();
  while (1) {
    if (button_press_pending_flag) {
      if (button_cancel_press_pending_flag) {
        SuspendToRunOtherTask(taskSelectTaskHandle);
        continue;
      }
      button_reset_all_flags();
    }
    FOC_spring_loop(0, spring_pid);
    vTaskDelayUntil(&xLastWakeTime,
                    1); // every FOC control task need at least 1ms delay otherwise cannot detect key press normally
  }
}

void DampTask(void *argument) {

}

void KnobTask(void *argument) {
  TickType_t xLastWakeTime = xTaskGetTickCount();

  // Settings
  uint8_t select_param = 0;
  const uint8_t max_select_param = 3;
  uint8_t change_param = 0;

  uint8_t values[3] = {4, 5, 4}; // [0]: sector_num [1]: k [2]: max_force

  pid_knob.Kp = values[1];
  pid_knob.max_u = values[2];
  pid_knob.min_u = -values[2];
  gui_draw_knob_mode(values[0], values[1], values[2], select_param, change_param, 1);

  while (1) {
    if (button_press_pending_flag) {
      if (button_confirm_press_pending_flag) {
        if (select_param) {
          change_param = !change_param;
        }
      } else if (button_left_press_pending_flag) {
        if (change_param) {
          values[select_param - 1]--;

          // reconfigure
          pid_knob.Kp = values[1];
          pid_knob.max_u = values[2];
          pid_knob.min_u = -values[2];
          gui_draw_knob_mode(values[0], values[1], values[2], select_param, change_param, 0);
        } else {
          if (select_param == 0) {
            select_param = max_select_param;
          } else select_param--;
        }
      } else if (button_right_press_pending_flag) {
        if (change_param) {
          values[select_param - 1]++;

          // reconfigure
          pid_knob.Kp = values[1];
          pid_knob.max_u = values[2];
          pid_knob.min_u = -values[2];
          gui_draw_knob_mode(values[0], values[1], values[2], select_param, change_param, 0);
        } else {
          if (select_param == max_select_param) {
            select_param = 0;
          } else select_param++;
        }
      } else if (button_cancel_press_pending_flag) {
        SuspendToRunOtherTask(taskSelectTaskHandle);
        gui_draw_knob_mode(values[0], values[1], values[2], select_param, change_param, 1);
        continue;
      }
      button_reset_all_flags();
      gui_draw_knob_mode(values[0], values[1], values[2], select_param, change_param, 0);
    }

    FOC_knob_loop(values[0]);
    vTaskDelayUntil(&xLastWakeTime,
                    1); // every FOC control task need at least 1ms delay otherwise cannot detect key press normally
  }
}

#endif


/* USER CODE END Application */

