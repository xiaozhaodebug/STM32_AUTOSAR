/**
 * @file        AUTOSAR_Integration.c
 * @brief       AUTOSAR模块集成实现
 * @details     STM32F407工程的AUTOSAR模块集成和主循环
 * 
 * @author      [小昭debug]
 * @date        2026-03-15
 */

#include "AUTOSAR_Cfg.h"

/*==================================================================================================
 *                                      全局变量
==================================================================================================*/
static volatile uint32 Autosar_TickCounter = 0;

/*==================================================================================================
 *                                      ECUM回调实现
==================================================================================================*/

/**
 * @brief  ECUM驱动初始化0
 * @note   在OS启动前调用，初始化MCAL等底层驱动
 */
void EcuM_DriverInitZero(void)
{
    /* 初始化系统时钟等 */
}

/**
 * @brief  ECUM驱动初始化1
 * @note   初始化BSW基础模块
 */
void EcuM_AL_DriverInitOne(void)
{
    /* 初始化DET（开发和错误追踪） */
    /* 初始化DEM（诊断事件管理） */
}

/**
 * @brief  ECUM驱动初始化2
 * @note   OS启动后初始化通信栈
 */
void EcuM_AL_DriverInitTwo(void)
{
    /* 初始化CANIF */
    CanIf_Init(&CanIf_Config);
    
    /* 启动CAN控制器 */
    CanIf_SetControllerMode(0, CANIF_CS_STARTED);
    
    /* 设置PDU模式为在线 */
    CanIf_SetPduMode(0, CANIF_ONLINE);
    
    /* 初始化PDUR */
    PduR_Init(&PduR_Config);
    
    /* 初始化COM */
    Com_Init(&Com_Config);
    
    /* 初始化DCM */
    Dcm_Init(&Dcm_Config);
    
    /* 请求ECUM RUN状态 */
    EcuM_RequestRUN(ECUM_USER_ECUM);
}

/**
 * @brief  ECUM驱动初始化3
 * @note   初始化SWC和RTE
 */
void EcuM_AL_DriverInitThree(void)
{
    /* 应用层初始化 */
}

/**
 * @brief  ECUM驱动重启
 */
void EcuM_AL_DriverRestart(void)
{
    /* 重新初始化驱动 */
}

/**
 * @brief  ECUM关闭
 */
void EcuM_AL_SwitchOff(void)
{
    /* 反初始化模块 */
    Com_DeInit();
    PduR_DeInit();
    CanIf_DeInit();
    Dcm_DeInit();
    
    EcuM_ReleaseRUN(ECUM_USER_ECUM);
}

/*==================================================================================================
 *                                      回调函数实现
==================================================================================================*/

/**
 * @brief  CanIf接收指示回调
 * @note   由CanIf调用，将接收到的PDU路由到上层
 */
void CanIf_Callback_RxIndication(PduIdType RxPduId, const PduInfoType* PduInfoPtr)
{
    /* 调用PduR接收指示 */
    PduR_RxIndication(RxPduId, PduInfoPtr);
}

/**
 * @brief  CanIf发送确认回调
 */
void CanIf_Callback_TxConfirmation(PduIdType TxPduId)
{
    /* 调用PduR发送确认 */
    PduR_TxConfirmation(TxPduId, E_OK);
}

/*==================================================================================================
 *                                      定时任务
==================================================================================================*/

/**
 * @brief  1ms定时任务
 * @note   由SysTick中断调用
 */
void AUTOSAR_1msTask(void)
{
    Autosar_TickCounter++;
    
    /* DCM主函数 - 处理诊断请求 */
    Dcm_MainFunction();
}

/**
 * @brief  5ms定时任务
 */
void AUTOSAR_5msTask(void)
{
    /* ECUM主函数 */
    EcuM_MainFunction();
}

/**
 * @brief  10ms定时任务
 */
void AUTOSAR_10msTask(void)
{
    /* COM接收主函数 */
    Com_MainFunctionRx();
    
    /* COM发送主函数 */
    Com_MainFunctionTx();
}

/**
 * @brief  100ms定时任务
 */
void AUTOSAR_100msTask(void)
{
    /* COM路由信号主函数 */
    Com_MainFunctionRouteSignals();
}

/*==================================================================================================
 *                                      主循环
==================================================================================================*/

/**
 * @brief  AUTOSAR主循环初始化
 */
void AUTOSAR_Init(void)
{
    /* 初始化ECUM */
    EcuM_Init(&EcuM_Config);
    
    /* 启动第二步（OS启动后调用） */
    EcuM_StartupTwo();
}

/**
 * @brief  AUTOSAR主循环
 * @note   在main循环中调用
 */
void AUTOSAR_MainLoop(void)
{
    static uint32 last1ms = 0;
    static uint32 last5ms = 0;
    static uint32 last10ms = 0;
    static uint32 last100ms = 0;
    
    uint32 currentTick = Autosar_TickCounter;
    
    /* 1ms任务 */
    if ((currentTick - last1ms) >= 1) {
        last1ms = currentTick;
        AUTOSAR_1msTask();
    }
    
    /* 5ms任务 */
    if ((currentTick - last5ms) >= 5) {
        last5ms = currentTick;
        AUTOSAR_5msTask();
    }
    
    /* 10ms任务 */
    if ((currentTick - last10ms) >= 10) {
        last10ms = currentTick;
        AUTOSAR_10msTask();
    }
    
    /* 100ms任务 */
    if ((currentTick - last100ms) >= 100) {
        last100ms = currentTick;
        AUTOSAR_100msTask();
    }
}

/**
 * @brief  更新信号示例
 * @note   演示如何发送COM信号
 */
void AUTOSAR_UpdateSignals(void)
{
    uint8 signal1 = 0x12;
    uint16 signal2 = 0x3456;
    uint32 signal3 = 0x789ABCDE;
    
    /* 发送信号 */
    Com_SendSignal(0, &signal1);
    Com_SendSignal(1, &signal2);
    Com_SendSignal(2, &signal3);
}

/**
 * @brief  接收信号示例
 * @note   演示如何接收COM信号
 */
void AUTOSAR_ReceiveSignals(void)
{
    uint8 signal1;
    uint16 signal2;
    uint32 signal3;
    
    /* 接收信号 */
    Com_ReceiveSignal(0, &signal1);
    Com_ReceiveSignal(1, &signal2);
    Com_ReceiveSignal(2, &signal3);
}

/*==================================================================================================
 *                                      诊断服务回调
==================================================================================================*/

/**
 * @brief  读取DID回调
 */
Std_ReturnType AUTOSAR_ReadDidCallback(Dcm_DidType Did, uint8* Data, uint16* DataLen)
{
    /* 根据DID读取数据 */
    (void)Did;
    (void)Data;
    (void)DataLen;
    return E_NOT_OK;
}

/**
 * @brief  写入DID回调
 */
Std_ReturnType AUTOSAR_WriteDidCallback(Dcm_DidType Did, const uint8* Data, uint16 DataLen)
{
    /* 根据DID写入数据 */
    (void)Did;
    (void)Data;
    (void)DataLen;
    return E_NOT_OK;
}

/**
 * @brief  例程控制回调
 */
Std_ReturnType AUTOSAR_RoutineControlCallback(uint16 Rid, uint8 Action, 
                                               const uint8* ReqData, uint16 ReqLen,
                                               uint8* ResData, uint16* ResLen)
{
    (void)Rid;
    (void)Action;
    (void)ReqData;
    (void)ReqLen;
    (void)ResData;
    (void)ResLen;
    return E_NOT_OK;
}

/*==================================================================================================
 *                                      与底层CAN驱动接口
==================================================================================================*/

/**
 * @brief  CAN接收中断回调
 * @note   当CAN驱动接收到消息时调用
 */
void AUTOSAR_CanRxCallback(uint32 CanId, uint8 CanDlc, const uint8* CanData)
{
    /* 调用CanIf接收指示 */
    CanIf_RxIndication(0, CanId, CanDlc, CanData);
}

/**
 * @brief  CAN发送完成中断回调
 * @note   当CAN驱动发送完成时调用
 */
void AUTOSAR_CanTxCallback(void)
{
    /* 调用CanIf发送确认 */
    CanIf_TxConfirmation(0);
}
