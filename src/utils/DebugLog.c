/**
 * @file        DebugLog.c
 * @brief       调试日志模块实现
 * @details     通过USART1输出调试信息
 *              波特率: 115200
 *              引脚: PA9(TX), PA10(RX)
 * 
 * @author      [小昭debug]
 * @date        2026-03-15
 */

#include "DebugLog.h"

/* USART1寄存器定义 */
#define RCC_BASE        0x40023800
#define RCC_AHB1ENR     (*(volatile uint32_t *)(RCC_BASE + 0x30))
#define RCC_APB2ENR     (*(volatile uint32_t *)(RCC_BASE + 0x44))

#define GPIOA_BASE      0x40020000
#define GPIOA_MODER     (*(volatile uint32_t *)(GPIOA_BASE + 0x00))
#define GPIOA_AFRH      (*(volatile uint32_t *)(GPIOA_BASE + 0x24))

#define USART1_BASE     0x40011000
#define USART1_SR       (*(volatile uint32_t *)(USART1_BASE + 0x00))
#define USART1_DR       (*(volatile uint32_t *)(USART1_BASE + 0x04))
#define USART1_BRR      (*(volatile uint32_t *)(USART1_BASE + 0x08))
#define USART1_CR1      (*(volatile uint32_t *)(USART1_BASE + 0x0C))

/* 标志位定义 */
#define USART_SR_TXE    (1U << 7)   /* 发送缓冲区空 */

static uint8_t gDebugLogInitialized = 0;

/**
 * @brief  初始化USART1 (PA9-TX, PA10-RX, 115200bps)
 */
void DebugLog_Init(void)
{
    if (gDebugLogInitialized) {
        return;
    }
    
    /* 1. 使能GPIOA和USART1时钟 */
    RCC_AHB1ENR |= (1U << 0);   /* GPIOA */
    RCC_APB2ENR |= (1U << 4);   /* USART1 */
    
    /* 2. 配置PA9(TX)和PA10(RX)为复用功能 AF7 */
    GPIOA_MODER &= ~((3U << 18) | (3U << 20));
    GPIOA_MODER |= ((2U << 18) | (2U << 20));
    
    GPIOA_AFRH &= ~((0xFU << 4) | (0xFU << 8));
    GPIOA_AFRH |= ((7U << 4) | (7U << 8));
    
    /* 3. 配置波特率 115200 (APB2=84MHz) */
    /* 84MHz / 115200 = 729.17 = 0x2D9 */
    USART1_BRR = 0x02D9;
    
    /* 4. 使能发送和接收，使能USART */
    USART1_CR1 = (1U << 3) | (1U << 2) | (1U << 13);
    
    gDebugLogInitialized = 1;
}

/**
 * @brief  发送单个字符
 */
static void DebugLog_PutChar(char c)
{
    if (!gDebugLogInitialized) {
        return;
    }
    
    /* 等待发送缓冲区为空 */
    while (!(USART1_SR & USART_SR_TXE)) {
        /* 等待 */
    }
    USART1_DR = c;
}

/**
 * @brief  输出字符串
 */
void DebugLog_String(const char* str)
{
    if (str == 0) {
        return;
    }
    
    while (*str != '\0') {
        DebugLog_PutChar(*str);
        str++;
    }
}

/**
 * @brief  输出带前缀的字符串
 */
void DebugLog_Prefix(const char* prefix, const char* str)
{
    DebugLog_String(prefix);
    DebugLog_String(str);
    DebugLog_NewLine();
}

/**
 * @brief  输出换行
 */
void DebugLog_NewLine(void)
{
    DebugLog_String("\r\n");
}

/**
 * @brief  输出十六进制数字
 */
void DebugLog_HexNum(uint32_t num, uint8_t digits)
{
    const char hex[] = "0123456789ABCDEF";
    int i;
    
    for (i = digits - 1; i >= 0; i--) {
        DebugLog_PutChar(hex[(num >> (i * 4)) & 0x0F]);
    }
}

/**
 * @brief  输出十进制数字
 */
void DebugLog_Dec(uint32_t num)
{
    char buf[12];
    int i = 0;
    
    if (num == 0) {
        DebugLog_PutChar('0');
        return;
    }
    
    do {
        buf[i++] = '0' + (num % 10);
        num /= 10;
    } while (num > 0);
    
    while (i > 0) {
        DebugLog_PutChar(buf[--i]);
    }
}

/**
 * @brief  输出十六进制数据
 */
void DebugLog_Hex(const uint8_t* data, uint8_t len)
{
    uint8_t i;
    
    for (i = 0; i < len; i++) {
        if (i > 0) {
            DebugLog_PutChar(' ');
        }
        DebugLog_HexNum(data[i], 2);
    }
}

/**
 * @brief  输出CAN接收消息
 */
void DebugLog_CanRx(uint32_t canId, uint8_t isExtId, uint8_t dlc, const uint8_t* data)
{
    DebugLog_String("[CAN] RX ID=");
    
    if (isExtId) {
        DebugLog_HexNum(canId, 8);
    } else {
        DebugLog_HexNum(canId, 3);
    }
    
    DebugLog_String(" DLC=");
    DebugLog_Dec(dlc);
    DebugLog_String(" Data=");
    DebugLog_Hex(data, dlc);
    DebugLog_NewLine();
}

/**
 * @brief  输出CAN发送消息
 */
void DebugLog_CanTx(uint32_t canId, uint8_t isExtId, uint8_t dlc, const uint8_t* data)
{
    DebugLog_String("[CAN] TX ID=");
    
    if (isExtId) {
        DebugLog_HexNum(canId, 8);
    } else {
        DebugLog_HexNum(canId, 3);
    }
    
    DebugLog_String(" DLC=");
    DebugLog_Dec(dlc);
    DebugLog_String(" Data=");
    DebugLog_Hex(data, dlc);
    DebugLog_NewLine();
}
