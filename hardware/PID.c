#include "main.h"
#include "PID.h"
#include <math.h>

#define EPSILON 1e-6f 

void PID_Fun(float Actual, float Target, float *Output, 
             float Kp, float Ki, float Kd, 
             float Max, float Polarity, float Offset, PID_TypeDefVar *PIDStructure)
{

    PIDStructure->Error1 = PIDStructure->Error0;
    PIDStructure->Error0 = Target - Actual;
    
    /*是否开启积分项*/
    if (fabsf(Ki) > EPSILON)
    {
        PIDStructure->ErrorInt += PIDStructure->Error0;
    }
    else
    {
        PIDStructure->ErrorInt = 0;
    }

    /*积分限幅*/
    if (PIDStructure->ErrorInt * Ki > 100) {PIDStructure->ErrorInt = 100 / Ki;}
    if (PIDStructure->ErrorInt * Ki < -100) {PIDStructure->ErrorInt = -100 / Ki;}
    
    *Output = Polarity * (Kp * PIDStructure->Error0 + Ki * PIDStructure->ErrorInt + Kd * (PIDStructure->Error0 - PIDStructure->Error1));
    
    if (*Output > 0) { *Output += Offset; }
    if (*Output < 0) { *Output -= Offset; }
    
    if (*Output > Max) { *Output = Max; }
    if (*Output < -Max) { *Output = -Max; }
}

