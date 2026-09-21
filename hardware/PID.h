#ifndef __PID_H
#define __PID_H

typedef struct {
    float Error0;     
    float Error1;       
    float ErrorInt;     
} PID_TypeDefVar;

void PID_Fun(float Actual, float Target, float *Output, 
             float Kp, float Ki, float Kd,
             float Max, float Polarity, float Offset, PID_TypeDefVar *PIDStructure);
#endif