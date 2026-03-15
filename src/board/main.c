/**
 * @file    main.c
 * @brief   STM32F407 AUTOSAR集成主程序
 * @details 实现AUTOSAR通信栈和诊断服务
 *          集成CanIf, PduR, Com, Dcm, EcuM模块
 * 
 *          Target: STM32F407ZGT6
 *          - HSE: 8 MHz
 *          - SYSCLK: 168 MHz
 *          - CAN1: PA11 (RX), PA12 (TX), 500 Kbps
 *          - AUTOSAR通信诊断功能
 * 
 * @author  [小昭debug]
 * @date    2026-03-15
 */

#include <stdint.h>
#include "AUTOSAR_Cfg.h"
#include "DebugLog.h"
#include "CanDriver.h"
#include "DbcConfig.h"

/* 函数前置声明 */
#ifdef ENABLE_AUTOSAR
void AUTOSAR_1msTask(void);
#endif

/* 寄存器定义 */
#define RCC_BASE        0x40023800
#define RCC_AHB1ENR     (*(volatile uint32_t *)(RCC_BASE + 0x30))
#define RCC_APB2ENR     (*(volatile uint32_t *)(RCC_BASE + 0x44))

#define GPIOA_BASE      0x40020000
#define GPIOA_MODER     (*(volatile uint32_t *)(GPIOA_BASE + 0x00))
#define GPIOA_AFRH      (*(volatile uint32_t *)(GPIOA_BASE + 0x24))

#define GPIOE_BASE      0x40021000
#define GPIOE_MODER     (*(volatile uint32_t *)(GPIOE_BASE + 0x00))
#define GPIOE_ODR       (*(volatile uint32_t *)(GPIOE_BASE + 0x14))
#define GPIOE_BSRR      (*(volatile uint32_t *)(GPIOE_BASE + 0x18))

#define GPIOG_BASE      0x40021800
#define GPIOG_MODER     (*(volatile uint32_t *)(GPIOG_BASE + 0x00))
#define GPIOG_ODR       (*(volatile uint32_t *)(GPIOG_BASE + 0x14))
#define GPIOG_BSRR      (*(volatile uint32_t *)(GPIOG_BASE + 0x18))

/* USART1寄存器 */
#define USART1_BASE     0x40011000
#define USART1_SR       (*(volatile uint32_t *)(USART1_BASE + 0x00))
#define USART1_DR       (*(volatile uint32_t *)(USART1_BASE + 0x04))
#define USART1_BRR      (*(volatile uint32_t *)(USART1_BASE + 0x08))
#define USART1_CR1      (*(volatile uint32_t *)(USART1_BASE + 0x0C))
#define USART1_CR2      (*(volatile uint32_t *)(USART1_BASE + 0x10))
#define USART1_CR3      (*(volatile uint32_t *)(USART1_BASE + 0x14))

/* SysTick寄存器 */
#define SYSTICK_BASE    0xE000E010
#define SYSTICK_CTRL    (*(volatile uint32_t *)(SYSTICK_BASE + 0x00))
#define SYSTICK_LOAD    (*(volatile uint32_t *)(SYSTICK_BASE + 0x04))
#define SYSTICK_VAL     (*(volatile uint32_t *)(SYSTICK_BASE + 0x08))

/* 全局变量 */
static volatile uint32_t gTickCount = 0;

/*==================================================================================================
 *                                      底层驱动接口
==================================================================================================*/

/**
 * @brief  简单延时函数
 */
static void Delay(volatile uint32_t count)
{
    while (count--) {
        __asm__ volatile("nop");
    }
}

/**
 * @brief  GPIO初始化 (LED: PE3, PE4, PG9 - 低电平点亮)
 */
static void GpioInit(void)
{
    /* 使能GPIOE和GPIOG时钟 */
    RCC_AHB1ENR |= (1U << 4);  /* GPIOE */
    RCC_AHB1ENR |= (1U << 6);  /* GPIOG */
    
    /* PE3, PE4配置为输出模式 */
    GPIOE_MODER &= ~((3U << 6) | (3U << 8));
    GPIOE_MODER |= ((1U << 6) | (1U << 8));
    
    /* PG9配置为输出模式 */
    GPIOG_MODER &= ~(3U << 18);
    GPIOG_MODER |= (1U << 18);
    
    /* 关闭所有LED (输出高电平) */
    GPIOE_BSRR = ((1U << 3) | (1U << 4));
    GPIOG_BSRR = (1U << 9);
}

/**
 * @brief  设置LED状态 (低电平点亮)
 * @param  led LED编号: 0=PE3, 1=PE4, 2=PG9
 * @param  on  1=点亮, 0=熄灭
 */
static void LedSet(uint8_t led, uint8_t on)
{
    switch (led) {
        case 0: /* PE3 */
            if (on) {
                GPIOE_BSRR = (1U << (3 + 16));  /* 复位 = 低电平 = 点亮 */
            } else {
                GPIOE_BSRR = (1U << 3);          /* 置位 = 高电平 = 熄灭 */
            }
            break;
        case 1: /* PE4 */
            if (on) {
                GPIOE_BSRR = (1U << (4 + 16));
            } else {
                GPIOE_BSRR = (1U << 4);
            }
            break;
        case 2: /* PG9 */
            if (on) {
                GPIOG_BSRR = (1U << (9 + 16));
            } else {
                GPIOG_BSRR = (1U << 9);
            }
            break;
        default:
            break;
    }
}

/**
 * @brief  翻转LED
 * @param  led LED编号: 0=PE3, 1=PE4, 2=PG9
 */
static void LedToggle(uint8_t led)
{
    static uint8_t state[3] = {0, 0, 0};
    
    if (led > 2) return;
    
    state[led] = state[led] ? 0 : 1;
    LedSet(led, state[led]);
}

/*==================================================================================================
 *                                      USART1串口驱动
==================================================================================================*/

/**
 * @brief  初始化USART1 (PA9-TX, PA10-RX, 115200bps)
 * @note   用于日志输出调试
 */
static void Usart1_Init(void)
{
    /* 1. 使能GPIOA和USART1时钟 */
    RCC_AHB1ENR |= (1U << 0);   /* GPIOA */
    RCC_APB2ENR |= (1U << 4);   /* USART1 */
    
    /* 2. 配置PA9(TX)和PA10(RX)为复用功能 */
    /* PA9 - TX */
    GPIOA_MODER &= ~(3U << 18);
    GPIOA_MODER |= (2U << 18);
    /* PA10 - RX */
    GPIOA_MODER &= ~(3U << 20);
    GPIOA_MODER |= (2U << 20);
    
    /* 设置复用功能 AF7 (USART1) */
    GPIOA_AFRH &= ~((0xFU << 4) | (0xFU << 8));
    GPIOA_AFRH |= ((7U << 4) | (7U << 8));
    
    /* 3. 配置波特率 115200 (APB2=84MHz) */
    /* BRR = 84MHz / 115200 = 729.17 = 0x2D9 */
    /* 实际计算: 84MHz / 16 = 5.25MHz, 5.25MHz / 115200 = 45.57 */
    /* 整数部分: 45 = 0x2D, 小数部分: 0.57 * 16 = 9 = 0x9 */
    USART1_BRR = 0x02D9;
    
    /* 4. 使能发送和接收，使能USART */
    USART1_CR1 = (1U << 3) | (1U << 2) | (1U << 13);
}

/**
 * @brief  发送单个字符
 * @param  c 要发送的字符
 */
static void Usart1_PutChar(char c)
{
    /* 等待发送缓冲区为空 */
    while (!(USART1_SR & (1U << 7))) {
        /* 等待 TXE=1 */
    }
    USART1_DR = c;
}

/**
 * @brief  发送字符串
 * @param  str 要发送的字符串
 */
static void Usart1_PutString(const char *str)
{
    if (str == 0) return;
    
    while (*str != '\0') {
        Usart1_PutChar(*str);
        str++;
    }
}

/**
 * @brief  发送十六进制字节
 * @param  data 要发送的字节
 */
static void Usart1_PutHexByte(uint8_t data)
{
    const char hex[] = "0123456789ABCDEF";
    Usart1_PutChar(hex[(data >> 4) & 0x0F]);
    Usart1_PutChar(hex[data & 0x0F]);
}

/**
 * @brief  发送换行
 */
static void Usart1_NewLine(void)
{
    Usart1_PutString("\r\n");
}

/*==================================================================================================
 *                                      SysTick定时器
==================================================================================================

/**
 * @brief  SysTick初始化 (1ms中断)
 */
static void SysTick_Init(void)
{
    /* 配置SysTick为1ms周期 (168MHz / 1000 = 168000) */
    SYSTICK_LOAD = 168000 - 1;
    SYSTICK_VAL = 0;
    SYSTICK_CTRL = 0x07;  /* 使能中断，使用处理器时钟 */
}

/**
 * @brief  SysTick中断处理
 */
void SysTick_Handler(void)
{
    gTickCount++;
    
#ifdef ENABLE_AUTOSAR
    /* AUTOSAR 1ms任务 */
    AUTOSAR_1msTask();
#endif
}

/**
 * @brief  获取系统tick
 */
uint32_t GetTick(void)
{
    return gTickCount;
}

/*==================================================================================================
 *                                      AUTOSAR任务调度
==================================================================================================*/

#ifdef ENABLE_AUTOSAR

/**
 * @brief  AUTOSAR 1ms任务
 */
void AUTOSAR_1msTask(void)
{
    /* DCM主函数 - 处理诊断请求 */
    Dcm_MainFunction();
}

/**
 * @brief  AUTOSAR周期任务
 */
static void AUTOSAR_PeriodicTasks(void)
{
    static uint32_t last5ms = 0;
    static uint32_t last10ms = 0;
    static uint32_t last100ms = 0;
    
    uint32_t currentTick = gTickCount;
    
    /* 5ms任务 - ECUM */
    if ((currentTick - last5ms) >= 5) {
        last5ms = currentTick;
        EcuM_MainFunction();
    }
    
    /* 10ms任务 - COM */
    if ((currentTick - last10ms) >= 10) {
        last10ms = currentTick;
        Com_MainFunctionRx();
        Com_MainFunctionTx();
    }
    
    /* 100ms任务 - 信号路由 */
    if ((currentTick - last100ms) >= 100) {
        last100ms = currentTick;
        Com_MainFunctionRouteSignals();
        
        /* 更新测试信号 */
        static uint8_t counter = 0;
        counter++;
        Com_SendSignal(0, &counter);
    }
}

/**
 * @brief  AUTOSAR初始化
 */
static void AUTOSAR_Init(void)
{
    /* 初始化ECUM */
    EcuM_Init(&EcuM_Config);
    
    /* 启动第二步 */
    EcuM_StartupTwo();
}

#endif /* ENABLE_AUTOSAR */

/*==================================================================================================
 *                                      ECUM回调实现
==================================================================================================*/

#ifdef ENABLE_AUTOSAR

void EcuM_DriverInitZero(void)
{
    /* 底层驱动初始化 */
}

void EcuM_AL_DriverInitOne(void)
{
    /* BSW基础模块初始化 */
}

void EcuM_AL_DriverInitTwo(void)
{
    DebugLog_String("[ECUM] DriverInitTwo start...\r\n");
    
    /* 初始化CANIF */
    DebugLog_String("[ECUM] Init CanIf...\r\n");
    CanIf_Init(&CanIf_Config);
    CanIf_SetControllerMode(0, CANIF_CS_STARTED);
    CanIf_SetPduMode(0, CANIF_ONLINE);
    DebugLog_String("[ECUM] CanIf init OK\r\n");
    
    /* 初始化PDUR */
    DebugLog_String("[ECUM] Init PduR...\r\n");
    PduR_Init(&PduR_Config);
    DebugLog_String("[ECUM] PduR init OK\r\n");
    
    /* 初始化COM */
    DebugLog_String("[ECUM] Init Com...\r\n");
    Com_Init(&Com_Config);
    DebugLog_String("[ECUM] Com init OK\r\n");
    
    /* 初始化DCM */
    DebugLog_String("[ECUM] Init Dcm...\r\n");
    Dcm_Init(&Dcm_Config);
    DebugLog_String("[ECUM] Dcm init OK\r\n");
    
    /* 请求ECUM RUN状态 */
    EcuM_RequestRUN(ECUM_USER_ECUM);
    
    /* 指示LED - 点亮LED0 */
    LedSet(0, 1);
    DebugLog_String("[ECUM] LED0 ON\r\n");
    
    DebugLog_String("[ECUM] DriverInitTwo complete!\r\n");
}

void EcuM_AL_DriverInitThree(void)
{
    /* 应用层初始化 */
}

void EcuM_AL_DriverRestart(void)
{
    /* 重新初始化 */
}

void EcuM_AL_SwitchOff(void)
{
    /* 关闭模块 */
    Com_DeInit();
    PduR_DeInit();
    CanIf_DeInit();
    Dcm_DeInit();
    
    EcuM_ReleaseRUN(ECUM_USER_ECUM);
}

#endif /* ENABLE_AUTOSAR */

/*==================================================================================================
 *                                      主函数
==================================================================================================*/

int main(void)
{
    /* 初始化GPIO */
    GpioInit();
    
    /* 初始化调试串口 */
    DebugLog_Init();
    
    /* 打印启动信息 */
    DebugLog_String("\r\n================================\r\n");
    DebugLog_String("STM32F407 AUTOSAR CAN Demo\r\n");
    DebugLog_String("Hardware: CAN1(PA11/PA12), USART1(PA9/PA10)\r\n");
    DebugLog_String("LED: PE3, PE4, PG9 (Low Active)\r\n");
    DebugLog_String("================================\r\n");
    
    /* 初始化SysTick */
    SysTick_Init();
    
#ifdef ENABLE_AUTOSAR
    /* AUTOSAR初始化 */
    AUTOSAR_Init();
#endif
    
    /* DBC 信号状态 */
    XZ_A_Led_State_t ledState = {0};
    XZ_A_KEY_State_t keyState = {0};
    XZ_A_Power_State_t powerState = {0};
    XZ_B_Ctrl_State_t ctrlState = {0};
    uint8_t canTxData[8];
    
    /* 初始化 LED 状态 */
    ledState.Led1_State = 1;  /* 常亮 */
    ledState.Led2_State = 2;  /* 闪烁 */
    ledState.Led3_State = 0;  /* 关闭 */
    ledState.Led4_State = 0;  /* 关闭 */
    
    /* 打印DBC信息 */
    DebugLog_String("[DBC] XZ_CAN_V2 DBC Configured\r\n");
    DebugLog_String("      TX: 0x210(Led), 0x211(Key), 0x212(Power)\r\n");
    DebugLog_String("      RX: 0x123(Motor), 0x124(Ctrl)\r\n");
    
    /* 主循环 */
    while (1) {
        static uint32_t lastLedTx = 0;
        static uint32_t lastKeyTx = 0;
        static uint32_t lastPowerTx = 0;
        
        /*==== 发送 XZ_A_Led_State (0x210) 每 50ms ====*/
        if ((gTickCount - lastLedTx) >= 50) {
            lastLedTx = gTickCount;
            
            /* 更新 LED2 闪烁状态 */
            ledState.Led2_State = ((gTickCount / 500) % 2) ? 2 : 0;  /* 500ms闪烁 */
            
            /* 根据接收到的控制信号更新 LED3/4 */
            ledState.Led3_State = ctrlState.Ctrl_Led3_State ? 1 : 0;
            ledState.Led4_State = ctrlState.Ctrl_Led4_State ? 1 : 0;
            
            /* 打包并发送 */
            Dbc_Pack_XZ_A_Led_State(canTxData, &ledState);
            
            CanMessage txMsg;
            txMsg.Id = MSG_ID_XZ_A_LED_STATE;
            txMsg.IsExtId = 0;
            txMsg.IsRemote = 0;
            txMsg.Dlc = 8;
            uint8_t i;
            for (i = 0; i < 8; i++) {
                txMsg.Data[i] = canTxData[i];
            }
            CanDriver_Send(&txMsg, 10);
        }
        
        /*==== 发送 XZ_A_KEY_State (0x211) 每 20ms ====*/
        if ((gTickCount - lastKeyTx) >= 20) {
            lastKeyTx = gTickCount;
            
            /* 模拟按键状态 (循环变化) */
            uint32_t keyCycle = (gTickCount / 2000) % 4;
            keyState.Key1_State = (keyCycle == 0) ? 1 : 0;
            keyState.Key2_State = (keyCycle == 1) ? 1 : 0;
            keyState.Key3_State = (keyCycle == 2) ? 1 : 0;
            keyState.Key4_State = (keyCycle == 3) ? 1 : 0;
            
            Dbc_Pack_XZ_A_KEY_State(canTxData, &keyState);
            
            CanMessage txMsg;
            txMsg.Id = MSG_ID_XZ_A_KEY_STATE;
            txMsg.IsExtId = 0;
            txMsg.IsRemote = 0;
            txMsg.Dlc = 8;
            uint8_t i;
            for (i = 0; i < 8; i++) {
                txMsg.Data[i] = canTxData[i];
            }
            CanDriver_Send(&txMsg, 10);
        }
        
        /*==== 发送 XZ_A_Power_State (0x212) 每 100ms ====*/
        if ((gTickCount - lastPowerTx) >= 100) {
            lastPowerTx = gTickCount;
            
            /* KL15 电源状态 */
            powerState.KL15_State = 1;  /* ON */
            
            /* 模拟 ADC 值 (简单计数) */
            powerState.ADC_Value = (uint16_t)(gTickCount % 4096);
            
            Dbc_Pack_XZ_A_Power_State(canTxData, &powerState);
            
            CanMessage txMsg;
            txMsg.Id = MSG_ID_XZ_A_POWER_STATE;
            txMsg.IsExtId = 0;
            txMsg.IsRemote = 0;
            txMsg.Dlc = 8;
            uint8_t i;
            for (i = 0; i < 8; i++) {
                txMsg.Data[i] = canTxData[i];
            }
            CanDriver_Send(&txMsg, 10);
        }
        
        /*==== 接收处理 ====*/
        if (CanDriver_IsMessagePending()) {
            CanMessage rxMsg;
            if (CanDriver_Receive(&rxMsg) == CAN_STATUS_OK) {
                
                switch (rxMsg.Id) {
                    case MSG_ID_XZ_B_MOTORSTATE: {
                        XZ_B_MotorState_t motorState;
                        Dbc_Parse_XZ_B_MotorState(rxMsg.Data, &motorState);
                        
                        /* 根据车速调整 LED1 状态 */
                        if (motorState.CarSpeed > 0) {
                            ledState.Led1_State = 2;  /* 闪烁 */
                        } else {
                            ledState.Led1_State = motorState.OnOff ? 1 : 0;
                        }
                        break;
                    }
                    
                    case MSG_ID_XZ_B_CTRL_STATE: {
                        Dbc_Parse_XZ_B_Ctrl_State(rxMsg.Data, &ctrlState);
                        
                        /* 根据控制信号设置 LED */
                        if (ctrlState.Ctrl_Led3_State) {
                            LedSet(0, 1);  /* PE3 LED0 点亮 */
                        } else {
                            LedSet(0, 0);
                        }
                        
                        if (ctrlState.Ctrl_Led4_State) {
                            LedSet(1, 1);  /* PE4 LED1 点亮 */
                        } else {
                            LedSet(1, 0);
                        }
                        break;
                    }
                    
                    default:
                        break;
                }
            }
        }
        
        /*==== 调试输出 (每5秒) ====*/
        if ((gTickCount % 5000) == 0) {
            DebugLog_String("[DBC] Status: Led1=");
            DebugLog_Dec(ledState.Led1_State);
            DebugLog_String(" Led2=");
            DebugLog_Dec(ledState.Led2_State);
            DebugLog_String(" Led3=");
            DebugLog_Dec(ledState.Led3_State);
            DebugLog_String(" Led4=");
            DebugLog_Dec(ledState.Led4_State);
            DebugLog_String(" Ctrl_Led3=");
            DebugLog_Dec(ctrlState.Ctrl_Led3_State);
            DebugLog_String(" Ctrl_Led4=");
            DebugLog_Dec(ctrlState.Ctrl_Led4_State);
            DebugLog_NewLine();
        }
        
        /* 低功耗等待 */
        __asm__ volatile("wfi");
    }
    
    return 0;
}
