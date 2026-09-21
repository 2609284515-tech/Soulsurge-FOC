#ifndef __SERIAL_H
#define __SERIAL_H

#include <stdint.h>
#include <stdio.h>

extern uint8_t Serial_RxPacket[4];
extern uint8_t Serial_TxPacket[100];

void Serial_SendByte(uint8_t Byte);
void Serial_Init(void);
void Serial_DMA_Init(uint16_t Length);
void Serial_SendArray(uint8_t *Array, uint16_t Length);
void Serial_SendString(char *String);
void Serial_SendNumber(uint32_t Number, uint8_t Length);
void Serial_SendSignedNumber(int32_t Number, uint8_t Length);
void Serial_Printf(char *format, ...);
void Serial_SendPacket(void);
void Serial_SendFloatNumber(float Number);
void Serial_SendTail(void);

uint8_t Serial_GetRxData(void);
uint8_t Serial_GetRxFlag(void);

void Serial_Test(void);


#endif
