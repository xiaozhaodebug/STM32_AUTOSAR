/**
 * @file        CanDriver.h
 * @brief       CAN1 驱动头文件
 * @details     提供 STM32F407 CAN1 外设的初始化、数据发送和接收功能
 *              - 波特率: 500 Kbps
 *              - 引脚: PA11 (RX), PA12 (TX)
 *              - 工作模式: 正常模式，使能接收中断
 * 
 * @author      [小昭debug]
 * @date        2026-03-15
 */

#ifndef CAN_DRIVER_H
#define CAN_DRIVER_H

#include <stdint.h>

/**
 * @brief CAN 操作状态返回值
 */
typedef enum {
    CAN_STATUS_OK = 0,      /**< 操作成功 */
    CAN_STATUS_ERROR,       /**< 操作失败 */
    CAN_STATUS_BUSY,        /**< CAN 忙 */
    CAN_STATUS_TX_FULL,     /**< 发送邮箱满 */
    CAN_STATUS_RX_EMPTY,    /**< 接收 FIFO 空 */
    CAN_STATUS_NOT_INIT     /**< 未初始化 */
} CanStatus;

/**
 * @brief CAN 消息结构体
 */
typedef struct {
    uint32_t Id;            /**< 消息 ID (标准 ID: 11位，扩展 ID: 29位) */
    uint8_t  IsExtId;       /**< 是否为扩展 ID: 0=标准ID, 1=扩展ID */
    uint8_t  IsRemote;      /**< 是否为远程帧: 0=数据帧, 1=远程帧 */
    uint8_t  Dlc;           /**< 数据长度码 (0-8) */
    uint8_t  Data[8];       /**< 数据字节 */
} CanMessage;

/**
 * @brief       CAN1 驱动初始化
 * @details     初始化 CAN1 外设，配置 GPIO (PA11-RX, PA12-TX)，
 *              设置波特率 500 Kbps，使能接收中断
 * @param       None
 * @return      CAN_STATUS_OK     - 初始化成功
 * @return      CAN_STATUS_ERROR  - 初始化失败
 * @note        必须在调用其他 CAN 函数之前调用此函数
 * @warning     此函数会配置 NVIC 中断，确保中断向量表正确
 * @author      [小昭debug]
 * @date        2026-03-15
 */
CanStatus CanDriver_Init(void);

/**
 * @brief       发送 CAN 消息
 * @details     将消息发送到 CAN 总线，使用三个发送邮箱之一
 * @param[in]   msg     指向要发送的消息结构体
 * @param[in]   timeout 超时时间 (毫秒)，0 表示非阻塞
 * @return      CAN_STATUS_OK      - 发送成功
 * @return      CAN_STATUS_ERROR   - 发送失败
 * @return      CAN_STATUS_BUSY    - 发送超时
 * @return      CAN_STATUS_TX_FULL - 所有发送邮箱满
 * @note        超时功能为简化实现，实际可能略有偏差
 * @warning     确保 CAN 已初始化后再调用
 * @author      [小昭debug]
 * @date        2026-03-15
 */
CanStatus CanDriver_Send(const CanMessage *msg, uint32_t timeout);

/**
 * @brief       接收 CAN 消息
 * @details     从接收 FIFO 中读取消息，非阻塞方式
 * @param[out]  msg     指向消息结构体，用于存储接收到的数据
 * @return      CAN_STATUS_OK       - 接收成功
 * @return      CAN_STATUS_RX_EMPTY - 接收 FIFO 空
 * @return      CAN_STATUS_ERROR    - 接收失败
 * @note        此函数为查询方式接收，中断接收需在中断回调中处理
 * @warning     确保 CAN 已初始化后再调用
 * @author      [小昭debug]
 * @date        2026-03-15
 */
CanStatus CanDriver_Receive(CanMessage *msg);

/**
 * @brief       获取 CAN 接收中断标志
 * @details     查询是否有新消息到达（FIFO 挂起标志）
 * @param       None
 * @return      0 - 无新消息
 * @return      1 - 有新消息
 * @note        可用于轮询方式检查接收状态
 * @author      [小昭debug]
 * @date        2026-03-15
 */
uint8_t CanDriver_IsMessagePending(void);

/**
 * @brief       设置接收过滤器
 * @details     配置 CAN 过滤器，支持标准 ID 和扩展 ID 过滤
 * @param[in]   filterId    过滤器 ID
 * @param[in]   mask        过滤器掩码
 * @param[in]   isExtId     是否为扩展 ID
 * @return      CAN_STATUS_OK     - 配置成功
 * @return      CAN_STATUS_ERROR  - 配置失败
 * @note        过滤器 0 用于接收所有标准 ID (掩码 0)
 * @warning     必须在 CanDriver_Init 之后调用
 * @author      [小昭debug]
 * @date        2026-03-15
 */
CanStatus CanDriver_SetFilter(uint32_t filterId, uint32_t mask, uint8_t isExtId);

#endif /* CAN_DRIVER_H */
