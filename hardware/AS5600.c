#include "main.h"
#include "AS5600_Data.h"
#include "OLED.h"

extern I2C_HandleTypeDef hi2c1;

#define I2C_Handle  hi2c1

// 写一个字节到指定寄存器
HAL_StatusTypeDef AS5600_WriteReg(uint8_t RegAddress, uint8_t Data)
{
    return HAL_I2C_Mem_Write(&I2C_Handle, AS5600_ADDRESS, RegAddress,
                             I2C_MEMADD_SIZE_8BIT, &Data, 1, I2C_TIMEOUT);
}

// 从指定寄存器读一个字节
uint8_t AS5600_ReadReg(uint8_t RegAddress)
{
    uint8_t Data = 0;
    HAL_I2C_Mem_Read(&I2C_Handle, AS5600_ADDRESS, RegAddress,
                     I2C_MEMADD_SIZE_8BIT, &Data, 1, I2C_TIMEOUT);
    return Data;
}

// 初始化 AS5600（配置为模拟输出，慢速滤波，关闭低功耗，关闭迟滞）
void AS5600_Init(void)
{
    // 配置 CONF_L: SF=00 (16x 慢速滤波), FTH=000 (仅慢速滤波)
    AS5600_WriteReg(AS5600_CONF_L, 0x00);
    // 配置 CONF_H: OUTS=00 (模拟全范围), PWMF=00 (115Hz), HYST=00 (关闭), PM=00 (正常模式)
    AS5600_WriteReg(AS5600_CONF_H, 0x00);
    // 如果需要其他配置，可在此修改
}

// 读取最终角度（12位，已缩放）
uint16_t AS5600_GetAngle(void)
{
    uint8_t H = AS5600_ReadReg(AS5600_ANGLE_H);
    uint8_t L = AS5600_ReadReg(AS5600_ANGLE_L);
    return (uint16_t)((H << 8) | L);
}

// 读取原始角度（12位，未缩放）
uint16_t AS5600_GetRawAngle(void)
{
    uint8_t H = AS5600_ReadReg(AS5600_RAW_ANGLE_H);
    uint8_t L = AS5600_ReadReg(AS5600_RAW_ANGLE_L);
    return (uint16_t)((H << 8) | L);
}

// 读取状态寄存器
uint8_t AS5600_GetStatus(void)
{
    return AS5600_ReadReg(AS5600_STATUS);
}

// 读取 AGC 值
uint8_t AS5600_GetAGC(void)
{
    return AS5600_ReadReg(AS5600_AGC);
}

// 读取磁场幅度（12位）
uint16_t AS5600_GetMagnitude(void)
{
    uint8_t H = AS5600_ReadReg(AS5600_MAGNITUDE_H);
    uint8_t L = AS5600_ReadReg(AS5600_MAGNITUDE_L);
    return (uint16_t)((H << 8) | L);
}

// 执行 BURN_ANGLE 命令（烧录 ZPOS 和 MPOS，最多可执行 3 次）
HAL_StatusTypeDef AS5600_BurnAngle(void)
{
    uint8_t cmd = 0x80;
    return HAL_I2C_Mem_Write(&I2C_Handle, AS5600_ADDRESS, AS5600_BURN,
                             I2C_MEMADD_SIZE_8BIT, &cmd, 1, I2C_TIMEOUT);
}

// 执行 BURN_SETTING 命令（烧录 MANG 和 CONF，仅可执行 1 次）
HAL_StatusTypeDef AS5600_BurnSetting(void)
{
    uint8_t cmd = 0x40;
    return HAL_I2C_Mem_Write(&I2C_Handle, AS5600_ADDRESS, AS5600_BURN,
                             I2C_MEMADD_SIZE_8BIT, &cmd, 1, I2C_TIMEOUT);
}

void Check_AS5600_Online(void)
{
    // AS5600_ADDRESS 定义为 0x6C (8位写地址)
    // HAL_I2C_IsDeviceReady 会发送 START + 地址，等待应答
    HAL_StatusTypeDef ret = HAL_I2C_IsDeviceReady(&I2C_Handle, AS5600_ADDRESS, 3, 100);
    
    if (ret == HAL_OK) {
        OLED_ShowString(0, 0, "AS5600 OK", OLED_6X8);
    } else {
        OLED_ShowString(0, 0, "AS5600 Error", OLED_6X8);
    }
}

void AS5600_Test(void)
{ 
    // 读取角度
    uint16_t Angle = AS5600_GetAngle();
    // 转换为角度值（0~360°）
    float degree = (Angle * 360.0f) / 4096.0f;

    uint8_t status = AS5600_ReadReg(AS5600_STATUS); // 地址 0x0B
    uint8_t agc = AS5600_ReadReg(AS5600_AGC);       // 地址 0x1A

    Check_AS5600_Online();
    
    OLED_ShowNum(0, 8, Angle, 5, OLED_6X8);
    OLED_ShowNum(0, 16, status, 5, OLED_6X8);
    OLED_ShowNum(0, 24, agc, 5, OLED_6X8);
}