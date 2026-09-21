#include "main.h"
#include "cmsis_os2.h"
#include "PID.h"
#include "arm_math.h"
#include "PWM.h"
#include "AS5600.h"
#include <stdbool.h>
#include <stdint.h>

#define VDC 24.0f
#define SQRT3 1.7320508f

extern volatile uint16_t adcValue_1[];
extern volatile float Angle;
extern volatile float *PID_Para[];

volatile float Current[4], ZeroAngle, ElecAngle, Actual_i_d, Actual_i_q;;
volatile uint8_t AS5600_Online_Flag = 0;
const uint8_t V_Vector[6][3] = {
    {1, 0, 0}, {1, 1, 0}, 
    {0, 1, 0}, {0, 1, 1}, 
    {0, 0, 1}, {1, 0, 1}};

void StartPWMTask(void *argument)
{
    

    /* 零点校准 */
    PWM_SetAllCompare(0.1, 0, 0);
    osDelay(500);
    ZeroAngle = (-AS5600_GetAngle() + 4096) * 2.0f * 3.14159265f / 4096.0f;
    PWM_SetAllCompare(0, 0, 0);   
    osDelay(1000);

    AS5600_Online_Flag = 1;

    // 伪六步
    // for (int i = 0; i < 5; i++)
    // { 
    //     PWM_SetAllCompare(0.1, 0, 0);
    //     osDelay(25);
    //     PWM_SetAllCompare(0.1, 0.1, 0);
    //     osDelay(25);
    //     PWM_SetAllCompare(0, 0.1, 0);
    //     osDelay(25);
    //     PWM_SetAllCompare(0, 0.1, 0.1);
    //     osDelay(25);
    //     PWM_SetAllCompare(0, 0, 0.1);
    //     osDelay(25);
    //     PWM_SetAllCompare(0.1, 0, 0.1);
    //     osDelay(25);
    //     PWM_SetAllCompare(0, 0, 0);   
    // }

      for(;;)
    {
        osDelay(100);

        AS5600_Online_Flag = AS5600_GetStatus();
    }
}

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef *hadc)
{
    static PID_TypeDefVar PID_TypedefVarStucture_1,
                          PID_TypedefVarStucture_2,
                          PID_TypedefVarStucture_3;

    if (hadc->Instance == ADC1 && AS5600_Online_Flag == 1) 
    {
        /*读电流*/
        // Current[0] = (adcValue_1[3] * 3.3 / 4096 - 1.65) / 3 / 0.33;
        Current[1] = (adcValue_1[4] * 3.3 / 4096 - 1.65) / 3 / 0.33;
        Current[2] = (adcValue_1[5] * 3.3 / 4096 - 1.65) / 3 / 0.33;

        Current[0] = -Current[1] - Current[2];

        /*计算电角度*/
        ElecAngle = fmodf((Angle - ZeroAngle) * 11.0f, 2 * 3.14159265f);
        if (ElecAngle < 0.0f) 
        {
            ElecAngle += 2 * 3.14159265f;
        }

        /*克拉克*/
        float Actual_i_alpha, Actual_i_beta;
        arm_clarke_f32(Current[0], Current[1], &Actual_i_alpha, &Actual_i_beta);

        /*帕克*/
        float sinE = arm_sin_f32(ElecAngle);
        float cosE = arm_cos_f32(ElecAngle);    //逆帕克也要用，所以缓存
        arm_park_f32(Actual_i_alpha, Actual_i_beta,
                     &Actual_i_d, &Actual_i_q,
                     sinE, cosE);

        /*电流环*/
        float V_d, V_q;
        PID_Fun(Actual_i_d, 0, &V_d, 
                PID_Para[1][0], PID_Para[2][0], PID_Para[3][0],    
                999, 1, 0, &PID_TypedefVarStucture_1);
        PID_Fun(Actual_i_q, PID_Para[0][0], &V_q, 
                PID_Para[1][0], PID_Para[2][0], PID_Para[3][0],    
                999, 1, 0, &PID_TypedefVarStucture_2);

        /*逆帕克*/
        float Target_i_alpha, Target_i_beta;
        arm_inv_park_f32(V_d * SQRT3 / VDC, V_q * SQRT3 / VDC,   //归一化
                         &Target_i_alpha, &Target_i_beta,
                         sinE, cosE);

        /*扇区判断*/
        bool A = (Target_i_beta > 0.0f);
        bool B = (SQRT3 * Target_i_alpha - Target_i_beta > 0.0f);
        bool C = (-SQRT3 * Target_i_alpha - Target_i_beta > 0.0f);
        uint8_t N = A + 2 * B + 4 * C;

        static const uint8_t sector_table[8] = {0, 2, 6, 1,
                                                4, 3, 5, 0};    //除非电压矢量为0，否则不会出现0
        uint8_t sector = sector_table[N];

        if (sector == 0)
        {
            // PWM_SetAllCompare(0.5f, 0.5f, 0.5f);
            return;
        }

        /*计算矢量作用时间*/
        float T1 = arm_sin_f32(sector * 2 * 3.14159265f / 6) * Target_i_alpha 
                         - arm_cos_f32(sector * 2 * 3.14159265f / 6) * Target_i_beta;
        float T2 = Target_i_beta * arm_cos_f32((sector - 1) * 2 * 3.14159265f / 6) 
                         - Target_i_alpha * arm_sin_f32((sector - 1) * 2 * 3.14159265f / 6);
        float T3 = 1 - T1 - T2;

         /*过调处理*/
        float Tsum = T1 + T2;
        if (Tsum > 1.0f)
        {
            T1 /= Tsum;
            T2 /= Tsum;
            T3 = 0.0f;
        }

        /*分配三相占空比*/
        // 第一个有效矢量对 x 相的作用时间 + 第二个有效矢量对 x 相的作用时间 + 零矢量平分到 000 和 111
        float Compare1 = T1 * V_Vector[sector - 1][0] + T2 * V_Vector[sector % 6][0] + T3 / 2;
        float Compare2 = T1 * V_Vector[sector - 1][1] + T2 * V_Vector[sector % 6][1] + T3 / 2;
        float Compare3 = T1 * V_Vector[sector - 1][2] + T2 * V_Vector[sector % 6][2] + T3 / 2;

        // 占空比最终限幅
        Compare1 = fmaxf(0.0f, fminf(Compare1, 1.0f));
        Compare2 = fmaxf(0.0f, fminf(Compare2, 1.0f));
        Compare3 = fmaxf(0.0f, fminf(Compare3, 1.0f));

        PWM_SetAllCompare(Compare1, Compare2, Compare3);
    }
    else if (AS5600_Online_Flag == 0)
    {
        PWM_SetAllCompare(0, 0, 0);  
    }
}