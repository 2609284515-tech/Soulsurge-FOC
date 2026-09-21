#include "main.h"
#include "Serial.h"
#include "AD.h"
#include "OLED.h"
#include "cmsis_os2.h"
#include <string.h>

extern volatile float Angle;
extern volatile float Current[4];
extern  uint8_t Serial_Tail[4];
extern volatile float ElecAngle;
extern volatile float Actual_i_d, Actual_i_q;

#pragma pack(push, 1)
typedef struct {
    float Angle;
    float Current[sizeof(Current)/sizeof(float)];  
    float ElecAngle;
    float Target[2];
    float Kp[3];
    float Ki[3];
    float Kd[3];
    float Actual_i_d;
    float Actual_i_q;
    uint8_t Serial_Tail[sizeof(Serial_Tail)];
} Serial_TxStruct;
#pragma pack(pop)

volatile float Target[2], 
               Kp[3] = {0, 0, 0},
               Ki[3] = {0, 0, 0},
               Kd[3] = {0, 0, 0},
               Offset[3] = {0, 0, 0};
volatile float *PID_Para[5] = {Target, Kp, Ki, Kd, Offset};

void StartSerialTask(void *argument)
{
    Serial_TxStruct *Serial_TxStructure= (Serial_TxStruct *)Serial_TxPacket;    // 将发送缓冲区直接映射为结构体指针
    memcpy(Serial_TxStructure->Serial_Tail, Serial_Tail, sizeof(Serial_Tail));

    for (;;)
    {
        osDelay(1);

        Serial_TxStructure->Angle = Angle;
        memcpy(Serial_TxStructure->Current, Current, sizeof(Current));
        Serial_TxStructure->ElecAngle = ElecAngle;
        memcpy(Serial_TxStructure->Target, Target, sizeof(Target));
        memcpy(Serial_TxStructure->Kp, Kp, sizeof(Kp));
        memcpy(Serial_TxStructure->Ki, Ki, sizeof(Ki));
        memcpy(Serial_TxStructure->Kd, Kd, sizeof(Kd));
        Serial_TxStructure->Actual_i_d = Actual_i_d;
        Serial_TxStructure->Actual_i_q = Actual_i_q;

        Serial_DMA_Init(sizeof(Serial_TxStruct));
        
        if (Serial_GetRxFlag() == 1)
        {
            /*步长调整*/
            double k;
            switch (Serial_RxPacket[0])
            {
                case 0:
                    k = 0.1;
                    break;
                default:
                    k = 0.1;
                    break;
            }

            /*4位分别表示：要修改的数，PID环号码，变化大小，变化方向*/
            if (Serial_RxPacket[3] == 0)
            {
                PID_Para[Serial_RxPacket[0]][Serial_RxPacket[1]] += Serial_RxPacket[2] * k;
            }
            else if (Serial_RxPacket[3] == 1)
            {
                PID_Para[Serial_RxPacket[0]][Serial_RxPacket[1]] -= Serial_RxPacket[2] * k;
            }
        }
    }
}