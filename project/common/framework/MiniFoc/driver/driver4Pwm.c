//
// Created by 邓志君 on 2023/3/2.
//

#include "driver.h"
#include "common/framework/MiniCommon.h"

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
  log_d("MOT: Enable 4PWM driver.\r\n");
}

static void  setPwm(float Ua, float Ub)
{
  float duty_cycle1A =  0.0f ,duty_cycle1B = 0.0f ,duty_cycle2A = 0.0f ,duty_cycle2B = 0.0f;

  // hardware specific writing
  if( Ua > 0 )
	duty_cycle1B = _constrain(abs(Ua),0.0f,1.0f);
  else
	duty_cycle1A = _constrain(abs(Ua),0.0f,1.0f);

  if( Ub > 0 )
	duty_cycle2B = _constrain(abs(Ub),0.0f,1.0f);
  else
	duty_cycle2A = _constrain(abs(Ub),0.0f,1.0f);

  // write to hardware
  __HAL_TIM_SET_COMPARE(PWM_HANDLE, TIM_CHANNEL_2, duty_cycle1A * PWM_Period);
  __HAL_TIM_SET_COMPARE(PWM_HANDLE, TIM_CHANNEL_3, duty_cycle1B * PWM_Period);
  __HAL_TIM_SET_COMPARE(PWM_HANDLE, TIM_CHANNEL_3, duty_cycle2A * PWM_Period);
  __HAL_TIM_SET_COMPARE(PWM_HANDLE, TIM_CHANNEL_3, duty_cycle2B * PWM_Period);
}

static void pwmEnable(void)
{
  M1_Enable;
}

static void pwmDisable(void)
{
  M1_Disable;
}

static StepperDriver_t driver4Pwm = {
	.name = "4PWM",
	.init = pwmInit,
	.setPwm = setPwm,
	.enable = pwmEnable,
	.disable = pwmDisable,
};

MODULE_INIT static void init(void){
  MiniStepperDriverManager_register(&driver4Pwm);
}