//
// Created by 邓志君 on 2023/3/2.
//

#include "driver.h"
#include "../../../framework/MiniCommon.h"

#define PWM_HANDLE &htim2
#define M1_Enable    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_4, GPIO_PIN_SET);          //高电平使能
#define M1_Disable   HAL_GPIO_WritePin(GPIOC, GPIO_PIN_4, GPIO_PIN_RESET);       //低电平失能
#define  PWM_Period   (4199.0)

static void  pwmInit(void)
{
  HAL_TIM_Base_Start_IT(PWM_HANDLE);
  HAL_TIM_PWM_Start(PWM_HANDLE, TIM_CHANNEL_2);
  HAL_TIM_PWM_Start(PWM_HANDLE, TIM_CHANNEL_3);
  HAL_TIM_PWM_Start(PWM_HANDLE, TIM_CHANNEL_4);

  M1_Enable;
  log_d("MOT: Enable 2PWM driver.\r\n");
}

static void  setPwm(float Ua, float Ub)
{
//  // phase 1 direction
//  digitalWrite(dir1a, Ua >= 0 ? LOW : HIGH);
//  if( _isset(dir1b) ) digitalWrite(dir1b, Ua >= 0 ? HIGH : LOW );
//  // phase 2 direction
//  digitalWrite(dir2a, Ub >= 0 ? LOW : HIGH);
//  if( _isset(dir2b) ) digitalWrite(dir2b, Ub >= 0 ? HIGH : LOW );

  // write to hardware
  __HAL_TIM_SET_COMPARE(PWM_HANDLE, TIM_CHANNEL_2, Ua * PWM_Period);
  __HAL_TIM_SET_COMPARE(PWM_HANDLE, TIM_CHANNEL_3, Ub * PWM_Period);
}

static void pwmEnable(void)
{
  M1_Enable;
}

static void pwmDisable(void)
{
  M1_Disable;
}

static StepperDriver_t driver2Pwm = {
	.name = "2PWM",
	.init = pwmInit,
	.setPwm = setPwm,
	.enable = pwmEnable,
	.disable = pwmDisable,
};

MODULE_INIT static void init(void){
  MiniStepperDriverManager_register(&driver2Pwm);
}