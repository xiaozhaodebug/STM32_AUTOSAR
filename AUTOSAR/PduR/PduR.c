/**
 * @file        PduR.c
 * @brief       PDU路由器模块实现
 * @details     简化版AUTOSAR PduR实现
 * 
 * @author      [小昭debug]
 * @date        2026-03-15
 */

#include "PduR.h"
#include "CanIf.h"
#include "Dcm.h"

/*==================================================================================================
 *                                      宏定义
==================================================================================================*/
#define PDUR_MAX_ROUTING_PATHS      32

/*==================================================================================================
 *                                      内部类型
==================================================================================================*/
typedef struct {
    bool                        Initialized;
    const PduR_ConfigType*      ConfigPtr;
} PduR_GlobalType;

/*==================================================================================================
 *                                      全局变量
==================================================================================================*/
static PduR_GlobalType PduR_Global = {
    .Initialized = FALSE,
    .ConfigPtr = NULL_PTR
};

/* 外部模块引用声明 */
extern void Com_RxIndication(PduIdType RxPduId, const PduInfoType* PduInfoPtr);
extern void Com_TxConfirmation(PduIdType TxPduId, Std_ReturnType result);

/*==================================================================================================
 *                                      私有函数
==================================================================================================*/

/**
 * @brief  查找路由路径
 */
static const PduR_RoutingPathConfigType* PduR_FindRoutingPath(PduIdType SrcPduId)
{
    uint16 i;
    
    if (PduR_Global.ConfigPtr == NULL_PTR) {
        return NULL_PTR;
    }
    
    for (i = 0; i < PduR_Global.ConfigPtr->NumRoutingPaths; i++) {
        if (PduR_Global.ConfigPtr->RoutingPaths[i].SrcPduId == SrcPduId) {
            return &PduR_Global.ConfigPtr->RoutingPaths[i];
        }
    }
    
    return NULL_PTR;
}

/**
 * @brief  根据目标模块发送
 */
static Std_ReturnType PduR_SendToModule(const PduR_RoutingPathConfigType* routingPath, const PduInfoType* PduInfoPtr)
{
    if (routingPath == NULL_PTR) {
        return E_NOT_OK;
    }
    
    switch (routingPath->DestModule) {
        case PDUR_DEST_MODULE_CANIF:
            return CanIf_Transmit(routingPath->DestPduId, PduInfoPtr);
            
        case PDUR_DEST_MODULE_COM:
            /* COM作为上层，不直接调用CanIf */
            return E_NOT_OK;
            
        case PDUR_DEST_MODULE_DCM:
            /* DCM处理诊断帧 */
            return E_NOT_OK;
            
        default:
            return E_NOT_OK;
    }
}

/**
 * @brief  从模块接收
 */
static void PduR_ReceiveFromModule(const PduR_RoutingPathConfigType* routingPath, const PduInfoType* PduInfoPtr)
{
    if (routingPath == NULL_PTR) {
        return;
    }
    
    switch (routingPath->DestModule) {
        case PDUR_DEST_MODULE_COM:
            Com_RxIndication(routingPath->DestPduId, PduInfoPtr);
            break;
            
        case PDUR_DEST_MODULE_DCM:
            /* DCM接收处理 */
            Dcm_RxIndication(routingPath->DestPduId, PduInfoPtr);
            break;
            
        default:
            break;
    }
}

/*==================================================================================================
 *                                      公共函数
==================================================================================================*/

void PduR_Init(const PduR_ConfigType* ConfigPtr)
{
    if (ConfigPtr == NULL_PTR) {
        return;
    }
    
    if (PduR_Global.Initialized) {
        return;
    }
    
    PduR_Global.ConfigPtr = ConfigPtr;
    PduR_Global.Initialized = TRUE;
}

void PduR_DeInit(void)
{
    if (!PduR_Global.Initialized) {
        return;
    }
    
    PduR_Global.Initialized = FALSE;
    PduR_Global.ConfigPtr = NULL_PTR;
}

void PduR_GetVersionInfo(Std_VersionInfoType* VersionInfo)
{
    if (VersionInfo == NULL_PTR) {
        return;
    }
    
    VersionInfo->vendorID = PDUR_VENDOR_ID;
    VersionInfo->moduleID = PDUR_MODULE_ID;
    VersionInfo->sw_major_version = PDUR_SW_MAJOR_VERSION;
    VersionInfo->sw_minor_version = PDUR_SW_MINOR_VERSION;
    VersionInfo->sw_patch_version = PDUR_SW_PATCH_VERSION;
}

Std_ReturnType PduR_Transmit(PduIdType TxPduId, const PduInfoType* PduInfoPtr)
{
    const PduR_RoutingPathConfigType* routingPath;
    
    if (!PduR_Global.Initialized) {
        return E_NOT_OK;
    }
    
    if (PduInfoPtr == NULL_PTR) {
        return E_NOT_OK;
    }
    
    /* 查找路由路径 */
    routingPath = PduR_FindRoutingPath(TxPduId);
    if (routingPath == NULL_PTR) {
        return E_NOT_OK;
    }
    
    /* 发送到目标模块 */
    return PduR_SendToModule(routingPath, PduInfoPtr);
}

Std_ReturnType PduR_CancelTransmit(PduIdType TxPduId)
{
    (void)TxPduId;
    /* 简化实现，暂不支持 */
    return E_NOT_OK;
}

Std_ReturnType PduR_CancelReceive(PduIdType RxPduId)
{
    (void)RxPduId;
    /* 简化实现，暂不支持 */
    return E_NOT_OK;
}

void PduR_RxIndication(PduIdType RxPduId, const PduInfoType* PduInfoPtr)
{
    const PduR_RoutingPathConfigType* routingPath;
    
    if (!PduR_Global.Initialized) {
        return;
    }
    
    if (PduInfoPtr == NULL_PTR) {
        return;
    }
    
    /* 查找路由路径 */
    routingPath = PduR_FindRoutingPath(RxPduId);
    if (routingPath == NULL_PTR) {
        return;
    }
    
    /* 路由到目标模块 */
    PduR_ReceiveFromModule(routingPath, PduInfoPtr);
}

void PduR_TxConfirmation(PduIdType TxPduId, Std_ReturnType result)
{
    const PduR_RoutingPathConfigType* routingPath;
    
    if (!PduR_Global.Initialized) {
        return;
    }
    
    /* 查找路由路径 */
    routingPath = PduR_FindRoutingPath(TxPduId);
    if (routingPath == NULL_PTR) {
        return;
    }
    
    /* 通知上层 */
    switch (routingPath->DestModule) {
        case PDUR_DEST_MODULE_COM:
            Com_TxConfirmation(routingPath->DestPduId, result);
            break;
            
        default:
            break;
    }
}

void PduR_TpRxIndication(PduIdType id, Std_ReturnType result, PduLengthType TpSduLength)
{
    (void)id;
    (void)result;
    (void)TpSduLength;
    /* TP功能简化实现 */
}

void PduR_TpTxConfirmation(PduIdType id, Std_ReturnType result)
{
    (void)id;
    (void)result;
    /* TP功能简化实现 */
}

BufReq_ReturnType PduR_TpCopyRxData(PduIdType id, const PduInfoType* info, PduLengthType* bufferSizePtr)
{
    (void)id;
    (void)info;
    (void)bufferSizePtr;
    return BUFREQ_E_NOT_OK;
}

BufReq_ReturnType PduR_TpCopyTxData(PduIdType id, const PduInfoType* info, const RetryInfoType* retry, PduLengthType* availableDataPtr)
{
    (void)id;
    (void)info;
    (void)retry;
    (void)availableDataPtr;
    return BUFREQ_E_NOT_OK;
}

BufReq_ReturnType PduR_StartOfReception(PduIdType id, const PduInfoType* info, PduLengthType TpSduLength, PduLengthType* bufferSizePtr)
{
    (void)id;
    (void)info;
    (void)TpSduLength;
    (void)bufferSizePtr;
    return BUFREQ_E_NOT_OK;
}
