/**
 * @file        CanIf.c
 * @brief       CAN接口模块实现
 * @details     简化版AUTOSAR CanIf实现
 * 
 * @author      [小昭debug]
 * @date        2026-03-15
 */

#include "CanIf.h"
#include "CanDriver.h"
#include "AUTOSAR_Cfg.h"
#include "DebugLog.h"

/*==================================================================================================
 *                                      宏定义
==================================================================================================*/
#define CANIF_MAX_CONTROLLERS       1       /**< 最大控制器数量 */
#define CANIF_MAX_TX_PDUS           16      /**< 最大发送PDU数量 */
#define CANIF_MAX_RX_PDUS           16      /**< 最大接收PDU数量 */

/*==================================================================================================
 *                                      内部类型
==================================================================================================*/
typedef struct {
    CanIf_ControllerModeType    Mode;           /**< 控制器模式 */
    CanIf_PduModeType           PduMode;        /**< PDU模式 */
    uint8                       BusOffCounter;  /**< BusOff计数器 */
} CanIf_ControllerStateType;

typedef struct {
    bool                        Initialized;                              /**< 初始化标志 */
    CanIf_ControllerStateType   ControllerState[CANIF_MAX_CONTROLLERS];   /**< 控制器状态 */
    const CanIf_ConfigType*     ConfigPtr;                                /**< 配置指针 */
} CanIf_GlobalType;

/*==================================================================================================
 *                                      全局变量
==================================================================================================*/
static CanIf_GlobalType CanIf_Global = {
    .Initialized = FALSE,
    .ControllerState = {{CANIF_CS_UNINIT, CANIF_OFFLINE, 0}},
    .ConfigPtr = NULL_PTR
};

/* 前向声明PduR函数 */
extern void PduR_RxIndication(PduIdType RxPduId, const PduInfoType* PduInfoPtr);
extern void PduR_TxConfirmation(PduIdType TxPduId, Std_ReturnType result);

/* CanIf回调函数实现 */
void CanIf_Callback_RxIndication(PduIdType RxPduId, const PduInfoType* PduInfoPtr)
{
    /* 输出调试信息 */
    DebugLog_String("[CANIF] RX PDU ID=");
    DebugLog_Dec(RxPduId);
    DebugLog_String(" Len=");
    DebugLog_Dec(PduInfoPtr->SduLength);
    DebugLog_String(" Data=");
    DebugLog_Hex(PduInfoPtr->SduDataPtr, PduInfoPtr->SduLength);
    DebugLog_NewLine();
    
    PduR_RxIndication(RxPduId, PduInfoPtr);
}

void CanIf_Callback_TxConfirmation(PduIdType TxPduId)
{
    DebugLog_String("[CANIF] TX Confirm PDU ID=");
    DebugLog_Dec(TxPduId);
    DebugLog_NewLine();
    
    PduR_TxConfirmation(TxPduId, E_OK);
}

/* 默认回调配置 */
const CanIf_CallbackType CanIf_Callbacks = {
    .RxIndication = CanIf_Callback_RxIndication,       /* 注册PduR接收指示函数 */
    .TxConfirmation = CanIf_Callback_TxConfirmation,   /* 注册PduR发送确认函数 */
    .ControllerBusOff = NULL_PTR,
    .ControllerModeIndication = NULL_PTR
};

/*==================================================================================================
 *                                      私有函数
==================================================================================================*/

/**
 * @brief  查找接收PDU配置
 */
static const CanIf_RxPduConfigType* CanIf_FindRxPduConfig(uint32 CanId, bool IsExtId)
{
    uint16 i;
    const CanIf_RxPduConfigType* rxPduConfig;
    
    if (CanIf_Global.ConfigPtr == NULL_PTR) {
        return NULL_PTR;
    }
    
    for (i = 0; i < CanIf_Global.ConfigPtr->NumRxPdus; i++) {
        rxPduConfig = &CanIf_Global.ConfigPtr->RxPduConfig[i];
        
        /* 检查ID类型匹配 */
        if (rxPduConfig->CanIdType != IsExtId) {
            continue;
        }
        
        /* 检查ID匹配（考虑掩码） */
        if ((CanId & rxPduConfig->CanIdMask) == (rxPduConfig->CanId & rxPduConfig->CanIdMask)) {
            return rxPduConfig;
        }
    }
    
    return NULL_PTR;
}

/**
 * @brief  查找发送PDU配置
 */
static const CanIf_TxPduConfigType* CanIf_FindTxPduConfig(PduIdType TxPduId)
{
    uint16 i;
    
    if (CanIf_Global.ConfigPtr == NULL_PTR) {
        return NULL_PTR;
    }
    
    for (i = 0; i < CanIf_Global.ConfigPtr->NumTxPdus; i++) {
        if (CanIf_Global.ConfigPtr->TxPduConfig[i].CanIfTxPduId == TxPduId) {
            return &CanIf_Global.ConfigPtr->TxPduConfig[i];
        }
    }
    
    return NULL_PTR;
}

/*==================================================================================================
 *                                      公共函数
==================================================================================================*/

void CanIf_Init(const CanIf_ConfigType* ConfigPtr)
{
    uint8 i;
    
    /* 参数检查 */
    if (ConfigPtr == NULL_PTR) {
        return;
    }
    
    /* 检查是否已初始化 */
    if (CanIf_Global.Initialized) {
        return;
    }
    
    /* 保存配置指针 */
    CanIf_Global.ConfigPtr = ConfigPtr;
    
    /* 初始化控制器状态 */
    for (i = 0; i < CANIF_MAX_CONTROLLERS && i < ConfigPtr->NumControllers; i++) {
        CanIf_Global.ControllerState[i].Mode = CANIF_CS_STOPPED;
        CanIf_Global.ControllerState[i].PduMode = CANIF_OFFLINE;
        CanIf_Global.ControllerState[i].BusOffCounter = 0;
    }
    
    CanIf_Global.Initialized = TRUE;
}

void CanIf_DeInit(void)
{
    uint8 i;
    
    if (!CanIf_Global.Initialized) {
        return;
    }
    
    /* 停止所有控制器 */
    for (i = 0; i < CANIF_MAX_CONTROLLERS; i++) {
        CanIf_SetControllerMode(i, CANIF_CS_STOPPED);
        CanIf_Global.ControllerState[i].Mode = CANIF_CS_UNINIT;
    }
    
    CanIf_Global.Initialized = FALSE;
    CanIf_Global.ConfigPtr = NULL_PTR;
}

Std_ReturnType CanIf_SetControllerMode(uint8 Controller, CanIf_ControllerModeType ControllerMode)
{
    if (!CanIf_Global.Initialized) {
        return E_NOT_OK;
    }
    
    if (Controller >= CANIF_MAX_CONTROLLERS) {
        return E_NOT_OK;
    }
    
    switch (ControllerMode) {
        case CANIF_CS_STARTED:
            /* 启动CAN控制器 */
            if (CanDriver_Init() == CAN_STATUS_OK) {
                CanIf_Global.ControllerState[Controller].Mode = CANIF_CS_STARTED;
            } else {
                return E_NOT_OK;
            }
            break;
            
        case CANIF_CS_STOPPED:
            /* 停止CAN控制器 */
            CanIf_Global.ControllerState[Controller].Mode = CANIF_CS_STOPPED;
            break;
            
        case CANIF_CS_SLEEP:
            /* 进入睡眠模式 */
            CanIf_Global.ControllerState[Controller].Mode = CANIF_CS_SLEEP;
            break;
            
        default:
            return E_NOT_OK;
    }
    
    /* 通知上层模式改变 */
    if (CanIf_Callbacks.ControllerModeIndication != NULL_PTR) {
        CanIf_Callbacks.ControllerModeIndication(Controller, ControllerMode);
    }
    
    return E_OK;
}

Std_ReturnType CanIf_GetControllerMode(uint8 Controller, CanIf_ControllerModeType* ControllerModePtr)
{
    if (!CanIf_Global.Initialized) {
        return E_NOT_OK;
    }
    
    if (Controller >= CANIF_MAX_CONTROLLERS) {
        return E_NOT_OK;
    }
    
    if (ControllerModePtr == NULL_PTR) {
        return E_NOT_OK;
    }
    
    *ControllerModePtr = CanIf_Global.ControllerState[Controller].Mode;
    
    return E_OK;
}

Std_ReturnType CanIf_Transmit(PduIdType TxPduId, const PduInfoType* PduInfoPtr)
{
    const CanIf_TxPduConfigType* txPduConfig;
    CanMessage canMsg;
    CanStatus status;
    
    if (!CanIf_Global.Initialized) {
        return E_NOT_OK;
    }
    
    if (PduInfoPtr == NULL_PTR) {
        return E_NOT_OK;
    }
    
    /* 查找PDU配置 */
    txPduConfig = CanIf_FindTxPduConfig(TxPduId);
    if (txPduConfig == NULL_PTR) {
        return E_NOT_OK;
    }
    
    /* 检查控制器模式 */
    if (CanIf_Global.ControllerState[txPduConfig->ControllerId].Mode != CANIF_CS_STARTED) {
        return E_NOT_OK;
    }
    
    /* 检查PDU模式 */
    if (CanIf_Global.ControllerState[txPduConfig->ControllerId].PduMode == CANIF_OFFLINE ||
        CanIf_Global.ControllerState[txPduConfig->ControllerId].PduMode == CANIF_TX_OFFLINE) {
        return E_NOT_OK;
    }
    
    /* 构造CAN消息 */
    canMsg.Id = txPduConfig->CanId;
    canMsg.IsExtId = txPduConfig->CanIdType;
    canMsg.IsRemote = FALSE;
    canMsg.Dlc = (PduInfoPtr->SduLength > 8) ? 8 : (uint8)PduInfoPtr->SduLength;
    
    /* 复制数据 */
    if (PduInfoPtr->SduDataPtr != NULL_PTR) {
        uint8 i;
        for (i = 0; i < canMsg.Dlc; i++) {
            canMsg.Data[i] = PduInfoPtr->SduDataPtr[i];
        }
    }
    
    /* 发送消息 */
    status = CanDriver_Send(&canMsg, 100);
    
    if (status != CAN_STATUS_OK) {
        return E_NOT_OK;
    }
    
    /* 发送确认回调 */
    if (CanIf_Callbacks.TxConfirmation != NULL_PTR) {
        CanIf_Callbacks.TxConfirmation(TxPduId);
    }
    
    return E_OK;
}

Std_ReturnType CanIf_SetPduMode(uint8 Controller, CanIf_PduModeType PduModeRequest)
{
    if (!CanIf_Global.Initialized) {
        return E_NOT_OK;
    }
    
    if (Controller >= CANIF_MAX_CONTROLLERS) {
        return E_NOT_OK;
    }
    
    CanIf_Global.ControllerState[Controller].PduMode = PduModeRequest;
    
    return E_OK;
}

Std_ReturnType CanIf_GetPduMode(uint8 Controller, CanIf_PduModeType* PduModePtr)
{
    if (!CanIf_Global.Initialized) {
        return E_NOT_OK;
    }
    
    if (Controller >= CANIF_MAX_CONTROLLERS) {
        return E_NOT_OK;
    }
    
    if (PduModePtr == NULL_PTR) {
        return E_NOT_OK;
    }
    
    *PduModePtr = CanIf_Global.ControllerState[Controller].PduMode;
    
    return E_OK;
}

void CanIf_GetVersionInfo(Std_VersionInfoType* VersionInfo)
{
    if (VersionInfo == NULL_PTR) {
        return;
    }
    
    VersionInfo->vendorID = CANIF_VENDOR_ID;
    VersionInfo->moduleID = CANIF_MODULE_ID;
    VersionInfo->sw_major_version = CANIF_SW_MAJOR_VERSION;
    VersionInfo->sw_minor_version = CANIF_SW_MINOR_VERSION;
    VersionInfo->sw_patch_version = CANIF_SW_PATCH_VERSION;
}

void CanIf_RxIndication(uint8 Hrh, uint32 CanId, uint8 CanDlc, const uint8* CanSduPtr)
{
    const CanIf_RxPduConfigType* rxPduConfig;
    PduInfoType pduInfo;
    uint8 sduData[8];
    bool isExtId;
    
    (void)Hrh;  /* 未使用参数 */
    
    if (!CanIf_Global.Initialized) {
        return;
    }
    
    if (CanSduPtr == NULL_PTR) {
        return;
    }
    
    /* 判断ID类型 */
    isExtId = (CanId > 0x7FF) ? TRUE : FALSE;
    
    /* 查找PDU配置 */
    rxPduConfig = CanIf_FindRxPduConfig(CanId, isExtId);
    if (rxPduConfig == NULL_PTR) {
        return;
    }
    
    /* 检查PDU模式 */
    if (CanIf_Global.ControllerState[rxPduConfig->ControllerId].PduMode == CANIF_OFFLINE) {
        return;
    }
    
    /* 构造PDU信息 */
    pduInfo.SduLength = (CanDlc > 8) ? 8 : CanDlc;
    pduInfo.SduDataPtr = sduData;
    pduInfo.MetaDataPtr = NULL_PTR;
    
    /* 复制数据 */
    {
        uint8 i;
        for (i = 0; i < pduInfo.SduLength; i++) {
            sduData[i] = CanSduPtr[i];
        }
    }
    
    /* 调用上层接收指示回调 */
    if (CanIf_Callbacks.RxIndication != NULL_PTR) {
        CanIf_Callbacks.RxIndication(rxPduConfig->UpperPduId, &pduInfo);
    }
}

void CanIf_TxConfirmation(uint8 Hth)
{
    (void)Hth;  /* 未使用参数 */
    /* 已在Transmit函数中调用回调，此处可选扩展 */
}

void CanIf_ControllerBusOff(uint8 Controller)
{
    if (Controller >= CANIF_MAX_CONTROLLERS) {
        return;
    }
    
    CanIf_Global.ControllerState[Controller].Mode = CANIF_CS_STOPPED;
    CanIf_Global.ControllerState[Controller].BusOffCounter++;
    
    /* 调用上层回调 */
    if (CanIf_Callbacks.ControllerBusOff != NULL_PTR) {
        CanIf_Callbacks.ControllerBusOff(Controller);
    }
}

Std_ReturnType CanIf_CheckWakeup(uint32 WakeupSource)
{
    (void)WakeupSource;  /* 未使用参数 */
    /* 简化实现，直接返回E_OK */
    return E_OK;
}
