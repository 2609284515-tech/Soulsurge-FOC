#ifndef __AS5600_DATA_H
#define __AS5600_DATA_H

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

#endif