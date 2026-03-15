/**
 * @file        DebugLog.h
 * @brief       调试日志模块头文件
 * @details     提供串口调试输出功能
 * 
 * @author      [小昭debug]
 * @date        2026-03-15
 */

#ifndef DEBUG_LOG_H
#define DEBUG_LOG_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief  初始化调试日志
 */
void DebugLog_Init(void);

/**
 * @brief  输出字符串
 * @param  str 字符串
 */
void DebugLog_String(const char* str);

/**
 * @brief  输出带前缀的字符串
 * @param  prefix 前缀
 * @param  str 字符串
 */
void DebugLog_Prefix(const char* prefix, const char* str);

/**
 * @brief  输出十六进制数据
 * @param  data 数据指针
 * @param  len 数据长度
 */
void DebugLog_Hex(const uint8_t* data, uint8_t len);

/**
 * @brief  输出CAN接收消息
 * @param  canId CAN ID
 * @param  isExtId 是否为扩展帧
 * @param  dlc 数据长度
 * @param  data 数据指针
 */
void DebugLog_CanRx(uint32_t canId, uint8_t isExtId, uint8_t dlc, const uint8_t* data);

/**
 * @brief  输出CAN发送消息
 * @param  canId CAN ID
 * @param  isExtId 是否为扩展帧
 * @param  dlc 数据长度
 * @param  data 数据指针
 */
void DebugLog_CanTx(uint32_t canId, uint8_t isExtId, uint8_t dlc, const uint8_t* data);

/**
 * @brief  输出数字
 * @param  num 数字
 */
void DebugLog_Dec(uint32_t num);

/**
 * @brief  输出十六进制数字
 * @param  num 数字
 * @param  digits 位数
 */
void DebugLog_HexNum(uint32_t num, uint8_t digits);

/**
 * @brief  换行
 */
void DebugLog_NewLine(void);

#ifdef __cplusplus
}
#endif

#endif /* DEBUG_LOG_H */
