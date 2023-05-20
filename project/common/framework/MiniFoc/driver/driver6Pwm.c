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
  log_d("MOT:  Enable 6PWM driver.\r\n");
}

static void  setPwm(float Ta, float Tb , float Tc)
{

  __HAL_TIM_SET_COMPARE(PWM_HANDLE, TIM_CHANNEL_2, Ta * PWM_Period);
  __HAL_TIM_SET_COMPARE(PWM_HANDLE, TIM_CHANNEL_3, Tb * PWM_Period);
  __HAL_TIM_SET_COMPARE(PWM_HANDLE, TIM_CHANNEL_4, Tc * PWM_Period);
}

// Set voltage to the pwm pin
static void setPhaseState(int sa, int sb, int sc)
{
  // disable if needed
//  if (_isset(enableA_pin) && _isset(enableB_pin) && _isset(enableC_pin))
//  {
//	digitalWrite(enableA_pin, sa == _HIGH_IMPEDANCE ? LOW : HIGH);
//	digitalWrite(enableB_pin, sb == _HIGH_IMPEDANCE ? LOW : HIGH);
//	digitalWrite(enableC_pin, sc == _HIGH_IMPEDANCE ? LOW : HIGH);
//  }
}

static void pwmEnable(void)
{
  M1_Enable;
}

static void pwmDisable(void)
{
  M1_Disable;
}

static BldcDriver_t pwm6Driver = {
	.name = "6PWM",
	.init = pwmInit,
	.setPhaseState = setPhaseState,
	.setPwm = setPwm,
	.enable = pwmEnable,
	.disable = pwmDisable,
};

MODULE_INIT static void init(void){
  MiniBldcDriverManager_register(&pwm6Driver);
}