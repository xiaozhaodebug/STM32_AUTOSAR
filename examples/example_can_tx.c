/**
 * @file        example_can_tx.c
 * @brief       CAN 发送示例
 * @details     演示如何使用 CAN 驱动发送消息
 * 
 * @author      [你的名字]
 * @date        2026-03-15
 */

#include "CanDriver.h"
#include "DebugLog.h"

/**
 * @brief  发送 CAN 消息示例
 * @note   发送标准帧，ID=0x123，8字节数据
 */
void Example_CanSend(void)
{
    CanMessage msg;
    CanStatus status;
    
    /* 构造 CAN 消息 */
    msg.Id = 0x123;         /* CAN ID */
    msg.IsExtId = 0;        /* 标准帧 (11位 ID) */
    msg.IsRemote = 0;       /* 数据帧 */
    msg.Dlc = 8;            /* 数据长度 8 字节 */
    
    /* 填充数据 */
    msg.Data[0] = 0x11;
    msg.Data[1] = 0x22;
    msg.Data[2] = 0x33;
    msg.Data[3] = 0x44;
    msg.Data[4] = 0x55;
    msg.Data[5] = 0x66;
    msg.Data[6] = 0x77;
    msg.Data[7] = 0x88;
    
    /* 发送消息，超时 100ms */
    status = CanDriver_Send(&msg, 100);
    
    if (status == CAN_STATUS_OK) {
        DebugLog_String("[Example] CAN message sent successfully\r\n");
    } else {
        DebugLog_String("[Example] CAN message send failed\r\n");
    }
}

/**
 * @brief  发送扩展帧示例
 * @note   发送扩展帧，ID=0x18DAF100
 */
void Example_CanSendExt(void)
{
    CanMessage msg;
    
    /* 构造扩展帧消息 */
    msg.Id = 0x18DAF100;    /* 29位扩展 ID */
    msg.IsExtId = 1;        /* 扩展帧 */
    msg.IsRemote = 0;
    msg.Dlc = 8;
    
    /* 填充 UDS 响应数据 */
    msg.Data[0] = 0x06;     /* 长度 */
    msg.Data[1] = 0x50;     /* 正响应，会话控制 */
    msg.Data[2] = 0x01;     /* 默认会话 */
    msg.Data[3] = 0x00;     /* P2 高字节 */
    msg.Data[4] = 0x32;     /* P2 低字节 (50ms) */
    msg.Data[5] = 0x01;     /* P2* 高字节 */
    msg.Data[6] = 0xF4;     /* P2* 低字节 (5s) */
    msg.Data[7] = 0x00;
    
    CanDriver_Send(&msg, 100);
}

/**
 * @brief  周期性发送示例
 * @note   在主循环中调用，每 500ms 发送一次
 */
void Example_CanPeriodicSend(void)
{
    static uint32_t lastSend = 0;
    static uint32_t tick = 0;
    
    tick++;
    
    /* 每 500ms 发送一次 */
    if ((tick - lastSend) >= 500) {
        lastSend = tick;
        
        CanMessage msg = {0};
        msg.Id = 0x123;
        msg.Dlc = 8;
        
        /* 使用 tick 作为数据 */
        msg.Data[0] = (uint8_t)(tick & 0xFF);
        msg.Data[1] = (uint8_t)((tick >> 8) & 0xFF);
        msg.Data[2] = (uint8_t)((tick >> 16) & 0xFF);
        msg.Data[3] = (uint8_t)((tick >> 24) & 0xFF);
        
        CanDriver_Send(&msg, 10);
    }
}
