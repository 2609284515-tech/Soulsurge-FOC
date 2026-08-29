#ifndef __MYCAN_H
#define __MYCAN_H

#include "main.h"   // 需包含FDCAN句柄及相关类型定义

void MyCAN_Init(void);
void MyCAN_Transmit(FDCAN_TxHeaderTypeDef *TxMessage, uint8_t *Data);
uint8_t MyCAN_ReceiveFlag(void);
void MyCAN_Receive(FDCAN_RxHeaderTypeDef *RxMessage, uint8_t *Data);
void MyCAN_Test(void);

#endif