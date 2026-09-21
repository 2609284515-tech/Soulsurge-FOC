#ifndef __PWM_H
#define __PWM_H

void PWM_Init(void);
void PWM_SetCompare1(uint16_t Compare);
void PWM_SetAllCompare(float Compare1, float Compare2, float Compare3);
void PWM_SetPrescaler(uint16_t Prescaler);

#endif