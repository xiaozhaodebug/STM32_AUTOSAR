/**
 * @file        example_can_rx.c
 * @brief       CAN 接收示例
 * @details     演示如何接收和处理 CAN 消息
 * 
 * @author      [你的名字]
 * @date        2026-03-15
 */

#include "CanDriver.h"
#include "DebugLog.h"

/**
 * @brief  轮询方式接收 CAN 消息
 * @note   在主循环中调用
 */
void Example_CanPollReceive(void)
{
    CanMessage msg;
    CanStatus status;
    
    /* 检查是否有消息 */
    if (CanDriver_IsMessagePending()) {
        /* 接收消息 */
        status = CanDriver_Receive(&msg);
        
        if (status == CAN_STATUS_OK) {
            /* 输出接收信息 */
            DebugLog_String("[Example] CAN received: ID=");
            DebugLog_HexNum(msg.Id, msg.IsExtId ? 8 : 3);
            DebugLog_String(" DLC=");
            DebugLog_Dec(msg.Dlc);
            DebugLog_String(" Data=");
            DebugLog_Hex(msg.Data, msg.Dlc);
            DebugLog_NewLine();
            
            /* 处理消息 */
            if (msg.Id == 0x123) {
                /* 处理测试帧 */
                DebugLog_String("[Example] Test frame received\r\n");
            } else if (msg.Id == 0x7E0) {
                /* 处理 UDS 请求 */
                DebugLog_String("[Example] UDS request received\r\n");
            }
        }
    }
}

/**
 * @brief  回显接收到的消息
 * @note   收到消息后，发送 ID+0x100 的回显
 */
void Example_CanEcho(void)
{
    CanMessage rxMsg;
    CanMessage txMsg;
    
    if (CanDriver_IsMessagePending()) {
        if (CanDriver_Receive(&rxMsg) == CAN_STATUS_OK) {
            /* 构造回显消息 */
            txMsg.Id = rxMsg.Id + 0x100;  /* ID + 0x100 */
            txMsg.IsExtId = rxMsg.IsExtId;
            txMsg.IsRemote = 0;
            txMsg.Dlc = rxMsg.Dlc;
            
            /* 数据加 1 */
            for (int i = 0; i < rxMsg.Dlc; i++) {
                txMsg.Data[i] = rxMsg.Data[i] + 1;
            }
            
            /* 发送回显 */
            CanDriver_Send(&txMsg, 10);
            
            DebugLog_String("[Example] Echo sent\r\n");
        }
    }
}

/**
 * @brief  处理特定 ID 的消息
 * @param  msg 接收到的消息
 */
void Example_ProcessMessage(const CanMessage* msg)
{
    switch (msg->Id) {
        case 0x123:
            /* 测试帧 */
            DebugLog_String("[Example] Processing test frame\r\n");
            break;
            
        case 0x7E0:
            /* UDS 诊断请求 */
            DebugLog_String("[Example] Processing UDS request\r\n");
            break;
            
        case 0x7DF:
            /* OBD 功能请求 */
            DebugLog_String("[Example] Processing OBD request\r\n");
            break;
            
        default:
            DebugLog_String("[Example] Unknown ID\r\n");
            break;
    }
}
