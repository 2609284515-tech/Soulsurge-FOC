#include "main.h"
#include "OLED.h"

#define FDCAN_Handle hfdcan1

extern FDCAN_HandleTypeDef FDCAN_Handle;   

FDCAN_RxHeaderTypeDef RxMsg;
uint8_t RxMsgData[4];

void MyCAN_Init(void)
{
    FDCAN_FilterTypeDef CAN_FilterInitStructure;

    // 配置过滤器：接收所有标准帧（ID和掩码均为0）
    CAN_FilterInitStructure.IdType = FDCAN_STANDARD_ID;       // 标准帧
    CAN_FilterInitStructure.FilterIndex = 0;                  // 使用过滤器0
    CAN_FilterInitStructure.FilterType = FDCAN_FILTER_MASK;   // 掩码模式
    CAN_FilterInitStructure.FilterConfig = FDCAN_FILTER_TO_RXFIFO0; // 存入Rx FIFO0
    CAN_FilterInitStructure.FilterID1 = 0x0000;               // ID全0（任意ID）
    CAN_FilterInitStructure.FilterID2 = 0x0000;               // 掩码全0（不关心任何位）
    HAL_FDCAN_ConfigFilter(&FDCAN_Handle, &CAN_FilterInitStructure);

    HAL_FDCAN_Start(&FDCAN_Handle);                                 // 启动FDCAN外设

    //中断用
    // HAL_FDCAN_ActivateNotification(&FDCAN_Handle, FDCAN_IT_RX_FIFO0_NEW_MESSAGE, 0);
}

void MyCAN_Transmit(FDCAN_TxHeaderTypeDef *TxMessage, uint8_t *Data)
{
    HAL_StatusTypeDef status;
    uint32_t TxBuffer;

    // 设置一个标记以便区分事件（例如用递增的计数器）
    static uint8_t marker = 0;
    TxMessage->MessageMarker = marker++;

    // 添加到Tx FIFO
    status = HAL_FDCAN_AddMessageToTxFifoQ(&FDCAN_Handle, TxMessage, Data);
    if (status != HAL_OK) return;

    // 获取缓冲区索引（仍然可用，但主要用于等待队列空闲）
    TxBuffer = HAL_FDCAN_GetLatestTxFifoQRequestBuffer(&FDCAN_Handle);

    // 轮询事件FIFO，等待匹配的事件
    uint32_t timeout = 100000;
    FDCAN_TxEventFifoTypeDef txEvent;

    while (timeout--)
    {
        if (HAL_FDCAN_GetTxEvent(&FDCAN_Handle, &txEvent) == HAL_OK)
        {
            // 检查是否是我们发送的消息（通过 MessageMarker 匹配）
            if (txEvent.MessageMarker == TxMessage->MessageMarker)
            {
                // 检查事件类型，判断是否成功
                if (txEvent.EventType == FDCAN_TX_EVENT)
                {
                    // 发送成功，退出
                    break;
                }
                else
                {
                    // 发送失败（如仲裁丢失、错误等），可做错误处理
                    // 例如：OLED_Printf(... "Tx Error");
                    break;
                }
            }
        }
        // 简单延时（可选）
    }

    // 若超时，说明事件未出现（可做超时处理）
}

uint8_t MyCAN_ReceiveFlag(void)
{
    // 检查Rx FIFO0中是否有待读消息
    if (HAL_FDCAN_GetRxFifoFillLevel(&FDCAN_Handle, FDCAN_RX_FIFO0) > 0)
        return 1;
    return 0;
}

void MyCAN_Receive(FDCAN_RxHeaderTypeDef *RxMessage, uint8_t *Data)
{
    // 从Rx FIFO0读取消息
    HAL_FDCAN_GetRxMessage(&FDCAN_Handle, FDCAN_RX_FIFO0, RxMessage, Data);
}

// void HAL_FDCAN_RxFifo0Callback(FDCAN_HandleTypeDef *hfdcan, uint32_t RxFifo0ITs)
// {
//     MyCAN_Receive(&RxMsg, RxMsgData);
// }

void MyCAN_Test(void)
{
    FDCAN_TxHeaderTypeDef TxMsgArray[] = {
        // 标准数据帧
        {
            .Identifier = 0x555,
            .IdType = FDCAN_STANDARD_ID,
            .TxFrameType = FDCAN_DATA_FRAME,
            .DataLength = 4,   // ← 修正点
            .ErrorStateIndicator = FDCAN_ESI_PASSIVE,
            .BitRateSwitch = FDCAN_BRS_OFF,
            .FDFormat = FDCAN_CLASSIC_CAN,
            .TxEventFifoControl = FDCAN_NO_TX_EVENTS,
            .MessageMarker = 0
        },
        // 扩展数据帧
        {
            .Identifier = 0x12345678,
            .IdType = FDCAN_EXTENDED_ID,
            .TxFrameType = FDCAN_DATA_FRAME,
            .DataLength = 4,   // ← 修正点
            .ErrorStateIndicator = FDCAN_ESI_PASSIVE,
            .BitRateSwitch = FDCAN_BRS_OFF,
            .FDFormat = FDCAN_CLASSIC_CAN,
            .TxEventFifoControl = FDCAN_NO_TX_EVENTS,
            .MessageMarker = 0
        },
        // 标准远程帧
        {
            .Identifier = 0x666,
            .IdType = FDCAN_STANDARD_ID,
            .TxFrameType = FDCAN_REMOTE_FRAME,
            .DataLength = 0,   // ← 修正点
            .ErrorStateIndicator = FDCAN_ESI_PASSIVE,
            .BitRateSwitch = FDCAN_BRS_OFF,
            .FDFormat = FDCAN_CLASSIC_CAN,
            .TxEventFifoControl = FDCAN_NO_TX_EVENTS,
            .MessageMarker = 0
        },
        // 扩展远程帧
        {
            .Identifier = 0x0789ABCD,
            .IdType = FDCAN_EXTENDED_ID,
            .TxFrameType = FDCAN_REMOTE_FRAME,
            .DataLength = 0,   // ← 修正点
            .ErrorStateIndicator = FDCAN_ESI_PASSIVE,
            .BitRateSwitch = FDCAN_BRS_OFF,
            .FDFormat = FDCAN_CLASSIC_CAN,
            .TxEventFifoControl = FDCAN_NO_TX_EVENTS,
            .MessageMarker = 0
        }
    };

    uint8_t TxMsgData[4][4] = {
        {0x11, 0x22, 0x33, 0x44},
        {0xAA, 0xBB, 0xCC, 0xDD},
        {0x00, 0x00, 0x00, 0x00},
        {0x00, 0x00, 0x00, 0x00},
    };

    
    static uint8_t pTxMsgArray = 0;    //test用
    

    MyCAN_Transmit(&TxMsgArray[pTxMsgArray], TxMsgData[pTxMsgArray]);
    
    pTxMsgArray ++;
    if (pTxMsgArray >= sizeof(TxMsgArray) / sizeof(FDCAN_TxHeaderTypeDef))
    {
        pTxMsgArray = 0;
    }

    // 接收处理（接收结构体同样没有DLC，而是DataLength）
    if (MyCAN_ReceiveFlag())
    {
        MyCAN_Receive(&RxMsg, RxMsgData);
        
        if (RxMsg.IdType == FDCAN_STANDARD_ID)
        {
            OLED_Printf(0, 0, OLED_6X8, "Std %X     ", RxMsg.Identifier);
        }
        else
        {
            OLED_Printf(0, 0, OLED_6X8, "Ext %X     ", RxMsg.Identifier);
        }
        
        if (RxMsg.RxFrameType == FDCAN_DATA_FRAME)
        {
            OLED_Printf(0, 8, OLED_6X8, "Data %x     ", RxMsg.DataLength);   // ← 修正点
            OLED_Printf(0, 16, OLED_6X8, "%x %x %x %x", 
                        RxMsgData[0], RxMsgData[1], RxMsgData[2], RxMsgData[3]);
        }
        else if (RxMsg.RxFrameType == FDCAN_REMOTE_FRAME)
        {
            OLED_Printf(0, 8, OLED_6X8, "Remote %x     ", RxMsg.DataLength); // ← 修正点
            OLED_Printf(0, 16, OLED_6X8, "00 00 00 00");
        }
    }
  
    HAL_Delay(1000); 
}