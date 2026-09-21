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

void PWM_SetAllCompare(float Compare1, float Compare2, float Compare3)
{
	__HAL_TIM_SET_COMPARE(&TIM_Handle, TIM_CHANNEL_1, (uint16_t)(4250 * Compare1));
	__HAL_TIM_SET_COMPARE(&TIM_Handle, TIM_CHANNEL_2, (uint16_t)(4250 * Compare2));
	__HAL_TIM_SET_COMPARE(&TIM_Handle, TIM_CHANNEL_3, (uint16_t)(4250 * Compare3));
}

//改PSC
void PWM_SetPrescaler(uint16_t Prescaler)
{
	__HAL_TIM_SET_PRESCALER(&TIM_Handle, Prescaler);
}

void PWM_Test(void)
{
	for (int i = 0; i < 100; i++)
	{
		PWM_SetAllCompare(50, 0, 0);
		HAL_Delay(500);
		PWM_SetAllCompare(0, 0, 0);
		HAL_Delay(500);
	}
}