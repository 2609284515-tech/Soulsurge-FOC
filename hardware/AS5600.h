#ifndef __AS5600_H
#define __AS5600_H

#include <stdint.h>
#include "stm32g4xx_hal_def.h"

// 公共函数声明
HAL_StatusTypeDef AS5600_WriteReg(uint8_t RegAddress, uint8_t Data);
uint8_t AS5600_ReadReg(uint8_t RegAddress);

void AS5600_Init(void);
uint16_t AS5600_GetAngle(void);
uint16_t AS5600_GetRawAngle(void);
uint8_t AS5600_GetStatus(void);
uint8_t AS5600_GetAGC(void);
uint16_t AS5600_GetMagnitude(void);

// 烧录命令（谨慎使用）
HAL_StatusTypeDef AS5600_BurnAngle(void);
HAL_StatusTypeDef AS5600_BurnSetting(void);

void Check_AS5600_Online(void);
void AS5600_Test(void);

#endif