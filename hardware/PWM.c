#include "main.h"
#include "stm32g4xx_hal_tim.h"

#define TIM_Handle htim8

extern TIM_HandleTypeDef TIM_Handle;

void PWM_Init(void)
{
    HAL_TIM_PWM_Start(&TIM_Handle, TIM_CHANNEL_1);
	HAL_TIM_PWM_Start(&TIM_Handle, TIM_CHANNEL_2);
	HAL_TIM_PWM_Start(&TIM_Handle, TIM_CHANNEL_3);

	HAL_TIMEx_PWMN_Start(&TIM_Handle, TIM_CHANNEL_1);
	HAL_TIMEx_PWMN_Start(&TIM_Handle, TIM_CHANNEL_2);
	HAL_TIMEx_PWMN_Start(&TIM_Handle, TIM_CHANNEL_3);
}

//改CCR
void PWM_SetCompare1(uint16_t Compare)
{
	__HAL_TIM_SET_COMPARE(&TIM_Handle, TIM_CHANNEL_1, Compare);
}

void PWM_SetCompare4(uint16_t Compare)
{
	__HAL_TIM_SET_COMPARE(&TIM_Handle, TIM_CHANNEL_4, Compare);
}

//改PSC
void PWM_SetPrescaler(uint16_t Prescaler)
{
	__HAL_TIM_SET_PRESCALER(&TIM_Handle, Prescaler);
}