#include "main.h"
#include "cmsis_os2.h"
#include "AS5600.h"


volatile float Angle = 0.0f;
float LastAngle, LaLastAngle;
void StartAngleTask(void *argument)
{

    for (;;)
    {
        osDelay(1);

        if (AS5600_CheckOnline() == 0)
        {
            AS5600_Init();
        }

        LaLastAngle = LastAngle;
        LastAngle = Angle;
        Angle = (-AS5600_GetAngle() + 4096) * 2.0f * 3.14159265f / 4096.0f;    //极性反转
        Angle = (Angle + LastAngle + LaLastAngle) / 3.0f;    //滤波
    }
}