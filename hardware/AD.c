#include "main.h"
#include <stdint.h>
#include "OLED.h"
#include "AD.h"

extern ADC_HandleTypeDef hadc1, hadc2;

uint16_t adcValue_1[ADC_CHANNEL_COUNT_1];
uint16_t adcValue_2[ADC_CHANNEL_COUNT_2];

void AD_Init(void)
{
    HAL_ADCEx_Calibration_Start(&hadc1, ADC_SINGLE_ENDED);
    HAL_ADC_Start_DMA(&hadc1, (uint32_t *)adcValue_1, ADC_CHANNEL_COUNT_1);

    HAL_ADCEx_Calibration_Start(&hadc2, ADC_SINGLE_ENDED);
    HAL_ADC_Start_DMA(&hadc2, (uint32_t *)adcValue_2, ADC_CHANNEL_COUNT_2);
}

void AD_Test(void)
{
    OLED_Printf(0, 0, OLED_6X8, "%d %d %d  ", adcValue_1[0], adcValue_1[1], adcValue_1[2]);
    OLED_Printf(0, 8, OLED_6X8, "%d %d %d  ", adcValue_1[3], adcValue_1[4], adcValue_1[5]);
    OLED_Printf(0, 16, OLED_6X8, "%d  ", adcValue_1[6]);

    OLED_Printf(0, 24, OLED_6X8, "%d %d %d  ", adcValue_2[0], adcValue_2[1], adcValue_2[2]);
    OLED_Printf(0, 32, OLED_6X8, "%d %d  ", adcValue_2[3], adcValue_2[4]);
}