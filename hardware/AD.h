#ifndef __AD_H
#define __AD_H

#define ADC_CHANNEL_COUNT_1 7
#define ADC_CHANNEL_COUNT_2 5

extern uint16_t adcValue_1[ADC_CHANNEL_COUNT_1];
extern uint16_t adcValue_2[ADC_CHANNEL_COUNT_2];

void AD_Init(void);
void AD_Test(void);

#endif