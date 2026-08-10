#include "AS5600.h"

// 写一个字节到指定寄存器
HAL_StatusTypeDef AS5600_WriteReg(uint8_t RegAddress, uint8_t Data)
{
    return HAL_I2C_Mem_Write(&hi2c3, AS5600_ADDRESS, RegAddress,
                             I2C_MEMADD_SIZE_8BIT, &Data, 1, I2C_TIMEOUT);
}

// 从指定寄存器读一个字节
uint8_t AS5600_ReadReg(uint8_t RegAddress)
{
    uint8_t Data = 0;
    HAL_I2C_Mem_Read(&hi2c3, AS5600_ADDRESS, RegAddress,
                     I2C_MEMADD_SIZE_8BIT, &Data, 1, I2C_TIMEOUT);
    return Data;
}

// 初始化 AS5600
void AS5600_Init(void)
{
    // FOC 专用配置（一行写完，清晰无比）
    AS5600_WriteReg(AS5600_CONF_L, AS5600_SF_2x | AS5600_FTH_18LSB | AS5600_WD_OFF);
    AS5600_WriteReg(AS5600_CONF_H, AS5600_PM_NOM | AS5600_HYST_OFF | AS5600_OUTS_ANALOG_FULL | AS5600_PWMF_115HZ);
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
    return HAL_I2C_Mem_Write(&hi2c3, AS5600_ADDRESS, AS5600_BURN,
                             I2C_MEMADD_SIZE_8BIT, &cmd, 1, I2C_TIMEOUT);
}

// 执行 BURN_SETTING 命令（烧录 MANG 和 CONF，仅可执行 1 次）
HAL_StatusTypeDef AS5600_BurnSetting(void)
{
    uint8_t cmd = 0x40;
    return HAL_I2C_Mem_Write(&hi2c3, AS5600_ADDRESS, AS5600_BURN,
                             I2C_MEMADD_SIZE_8BIT, &cmd, 1, I2C_TIMEOUT);
}

void Check_AS5600_Online(void)
{
    // AS5600_ADDRESS 定义为 0x6C (8位写地址)
    // HAL_I2C_IsDeviceReady 会发送 START + 地址，等待应答
    HAL_StatusTypeDef ret = HAL_I2C_IsDeviceReady(&hi2c3, AS5600_ADDRESS, 3, 100);
    
    if (ret == HAL_OK) {
        OLED_ShowString(1, 1, "AS5600 OK");
    } else {
        OLED_ShowString(1, 1, "AS5600 Error");
    }
}