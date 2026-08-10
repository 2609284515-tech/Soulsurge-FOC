#ifndef __AS5600_H
#define __AS5600_H

#include "main.h"
#include "OLED.h"
#include "stm32f4xx_hal_i2c.h"

extern I2C_HandleTypeDef hi2c3;

// AS5600 7-bit 地址 0x36，左移一位为 8-bit 写地址
#define AS5600_ADDRESS      0x6C
#define I2C_TIMEOUT         100

// 寄存器地址定义
#define AS5600_ZMCO         0x00
#define AS5600_ZPOS_H       0x01
#define AS5600_ZPOS_L       0x02
#define AS5600_MPOS_H       0x03
#define AS5600_MPOS_L       0x04
#define AS5600_MANG_H       0x05
#define AS5600_MANG_L       0x06
#define AS5600_CONF_L       0x07   // 包含 FTH, SF
#define AS5600_CONF_H       0x08   // 包含 PWMF, OUTS, HYST, PM
#define AS5600_STATUS       0x0B
#define AS5600_RAW_ANGLE_H  0x0C
#define AS5600_RAW_ANGLE_L  0x0D
#define AS5600_ANGLE_H      0x0E
#define AS5600_ANGLE_L      0x0F
#define AS5600_AGC          0x1A
#define AS5600_MAGNITUDE_H  0x1B
#define AS5600_MAGNITUDE_L  0x1C
#define AS5600_BURN         0xFF

// =============================================
// 寄存器 0x07 (CONF_L) - 滤波与看门狗配置
// =============================================

// --- 慢速滤波器 SF (位 1:0) ---
#define AS5600_SF_16x       (0x00 << 0)  // 延迟 2.2ms,  噪声 0.015° (最平滑，FOC绝对禁用)
#define AS5600_SF_8x        (0x01 << 0)  // 延迟 1.1ms,  噪声 0.021°
#define AS5600_SF_4x        (0x02 << 0)  // 延迟 0.55ms, 噪声 0.030°
#define AS5600_SF_2x        (0x03 << 0)  // 延迟 0.286ms,噪声 0.043° (FOC/高速控制首选)

// --- 快速滤波器阈值 FTH (位 4:2) ---
#define AS5600_FTH_OFF      (0x00 << 2)  // 仅慢速滤波（快滤关闭）
#define AS5600_FTH_6LSB     (0x01 << 2)  // 阈值 0.53°
#define AS5600_FTH_7LSB     (0x02 << 2)  // 阈值 0.62°
#define AS5600_FTH_9LSB     (0x03 << 2)  // 阈值 0.79°
#define AS5600_FTH_18LSB    (0x04 << 2)  // 阈值 1.58° (FOC 黄金平衡点)
#define AS5600_FTH_21LSB    (0x05 << 2)  // 阈值 1.85°
#define AS5600_FTH_24LSB    (0x06 << 2)  // 阈值 2.11°
#define AS5600_FTH_10LSB    (0x07 << 2)  // 阈值 0.88° (特殊档位)

// --- 看门狗 WD (位 5) ---
#define AS5600_WD_OFF       (0x00 << 5)  // 关闭看门狗
#define AS5600_WD_ON        (0x01 << 5)  // 开启看门狗（1分钟静止自动休眠）

// =============================================
// 寄存器 0x08 (CONF_H) - 输出与功耗配置
// =============================================

// --- 功耗模式 PM (位 1:0) ---
#define AS5600_PM_NOM       (0x00 << 0)  // 常开 6.5mA (响应最快)
#define AS5600_PM_LPM1      (0x01 << 0)  // 3.4mA (轮询 5ms)
#define AS5600_PM_LPM2      (0x02 << 0)  // 1.8mA (轮询 20ms)
#define AS5600_PM_LPM3      (0x03 << 0)  // 1.5mA (轮询 100ms)

// --- 输出迟滞 HYST (位 3:2) ---
#define AS5600_HYST_OFF     (0x00 << 2)  // 无迟滞（FOC/高精度必选）
#define AS5600_HYST_1LSB    (0x01 << 2)  // 1 LSB 迟滞
#define AS5600_HYST_2LSB    (0x02 << 2)  // 2 LSB 迟滞（旋钮手感稳）
#define AS5600_HYST_3LSB    (0x03 << 2)  // 3 LSB 迟滞

// --- 输出模式 OUTS (位 5:4) ---
#define AS5600_OUTS_ANALOG_FULL   (0x00 << 4)  // 模拟 0% ~ 100% VDD
#define AS5600_OUTS_ANALOG_10_90  (0x01 << 4)  // 模拟 10% ~ 90% VDD
#define AS5600_OUTS_PWM           (0x02 << 4)  // PWM 数字输出

// --- PWM 频率 PWMF (位 7:6) ---
#define AS5600_PWMF_115HZ   (0x00 << 6)  // 115 Hz
#define AS5600_PWMF_230HZ   (0x01 << 6)  // 230 Hz
#define AS5600_PWMF_460HZ   (0x02 << 6)  // 460 Hz
#define AS5600_PWMF_920HZ   (0x03 << 6)  // 920 Hz (仅PWM模式有效)

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

#endif