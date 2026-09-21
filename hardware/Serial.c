#include "main.h"
#include "OLED.h"
#include "stm32g4xx_hal_uart.h"
#include <stdio.h>
#include <stdarg.h>
#include <string.h>

#define Serial_Handle huart1
#define Serial_Name USART1

extern UART_HandleTypeDef Serial_Handle;

uint8_t Serial_RxData;
uint8_t Serial_RxFlag;
uint8_t Serial_RxPacket[4];
uint8_t Serial_TxPacket[100];
uint8_t Serial_Tail[4] = {0x00, 0x00, 0x80, 0x7f};

void Serial_Init(void)
{
    HAL_UART_Receive_IT(&Serial_Handle, &Serial_RxData, 1);
}

void Serial_DMA_Init(uint16_t Length)
{
	HAL_UART_Transmit_DMA(&Serial_Handle, Serial_TxPacket, Length);	
}

void Serial_SendByte(uint8_t Byte)
{
    HAL_UART_Transmit(&Serial_Handle, &Byte, 1, HAL_MAX_DELAY);
}
void Serial_SendArray(uint8_t *Array, uint16_t Length)
{
	uint16_t i;
	for(i = 0; i < Length; i ++)
	{
		Serial_SendByte(Array[i]);
	}
}

void Serial_SendString(char *String)
{
	uint8_t i;
	for (i = 0; String[i] != '\0'; i ++)
	{
		Serial_SendByte(String[i]);
	}
}

uint32_t Serial_Pow(uint32_t X, uint32_t Y)
{
	uint32_t Result = 1;
	while (Y --)
	{
		Result *= X;
	}
	return Result;
}

void Serial_SendNumber(uint32_t Number, uint8_t Length)
{
	uint8_t i;
	for (i = 0; i < Length; i ++)
	{
		Serial_SendByte(Number / Serial_Pow(10, Length - 1 - i) % 10 + '0');
	}
}

void Serial_SendSignedNumber(int32_t Number, uint8_t Length)
{
	if (Number < 0)
	{
		Serial_SendByte('-');
		Serial_SendNumber(-Number, Length);
	}
	else
	{
		Serial_SendNumber(Number, Length);
	}
}


void Serial_Printf(char *format, ...)
{
	char String[100];
	va_list arg;
	va_start(arg, format);
	vsprintf(String, format, arg);
	va_end(arg);
	Serial_SendString(String);
}

#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
    #define HOST_IS_LITTLE_ENDIAN 1
    #define HOST_IS_BIG_ENDIAN    0
#elif __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
    #define HOST_IS_LITTLE_ENDIAN 0
    #define HOST_IS_BIG_ENDIAN    1
#else
    #error "Unknown byte order!"
#endif

void float_to_bytes(float val, unsigned char *dst) {
    uint32_t bits;
    memcpy(&bits, &val, 4);
    
    #if HOST_IS_BIG_ENDIAN
        // 主机是大端，需要交换字节转为小端
        bits = (bits >> 24) | ((bits & 0x00FF0000) >> 8) | 
               ((bits & 0x0000FF00) << 8) | (bits << 24);
    #endif
    memcpy(dst, &bits, 4);
}

void Serial_SendFloatNumber(float Number)
{
	unsigned char Bytes[4]; 
	float_to_bytes(Number, Bytes);
	Serial_SendArray(Bytes, 4);
}

void Serial_SendTail(void)
{
	Serial_SendByte(0x00);
	Serial_SendByte(0x00);
	Serial_SendByte(0x80);
	Serial_SendByte(0x7f);
}

uint8_t Serial_GetRxFlag(void)
{
	if (Serial_RxFlag == 1)
	{
		Serial_RxFlag = 0;
		return 1;
	}
	return 0;
}

void Serial_SendPacket(void)
{
	Serial_SendByte(0xFF);
	Serial_SendArray(Serial_TxPacket, 4);
	Serial_SendByte(0xFE);
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	static uint8_t RxState = 0;
	static uint8_t pRxPacket = 0;
    if (huart->Instance == Serial_Name)
    {
        if (RxState == 0)
		{
			if (Serial_RxData == 0xFF && Serial_RxFlag == 0)
			{
				RxState = 1;
				pRxPacket = 0;
			}
		}
		else if (RxState == 1)
		{
			Serial_RxPacket[pRxPacket] = Serial_RxData;
			pRxPacket ++;
			if (pRxPacket >= 4)
			{
				RxState =2;
			}
		}
		else if (RxState == 2)
		{
			if (Serial_RxData == 0xFE)
			{
				RxState = 0;
				Serial_RxFlag = 1;
			}
		}
        
        HAL_UART_Receive_IT(&Serial_Handle, &Serial_RxData, 1);
    }
}

void Serial_Test(void)
{
	if (Serial_GetRxFlag() == 1)
	{
		OLED_ShowHexNum(0, 8, Serial_RxPacket[0], 2, OLED_6X8);
		OLED_ShowHexNum(18, 8, Serial_RxPacket[1], 2, OLED_6X8);
		OLED_ShowHexNum(36, 8, Serial_RxPacket[2], 2, OLED_6X8);
		OLED_ShowHexNum(54, 8, Serial_RxPacket[3], 2, OLED_6X8);
	}

	static uint16_t Flag = 0;
	if (Flag)
	{
	}
	else
	{
		OLED_ShowString(0, 0, "RXData", OLED_6X8);
		Serial_Printf("sjdlfkj%d", 123);

		Serial_DMA_Init(4);
		Serial_TxPacket[0] = 0x01;
		Serial_TxPacket[1] = 0x02;
		Serial_TxPacket[2] = 0x03;
		Serial_TxPacket[3] = 0x04;

		Flag = 1;
	}
}
