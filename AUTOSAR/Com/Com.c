/**
 * @file        Com.c
 * @brief       通信模块实现
 * @details     简化版AUTOSAR Com实现
 * 
 * @author      [小昭debug]
 * @date        2026-03-15
 */

#include "Com.h"
#include "PduR.h"
#include <string.h>

/*==================================================================================================
 *                                      宏定义
==================================================================================================*/
#define COM_MAX_SIGNALS         64      /**< 最大信号数量 */
#define COM_MAX_IPDU            16      /**< 最大I-PDU数量 */
#define COM_MAX_PDU_LENGTH      8       /**< 最大PDU长度 */

/*==================================================================================================
 *                                      内部类型
==================================================================================================*/
typedef struct {
    uint32              Value;          /**< 信号值 */
    bool                Updated;        /**< 更新标志 */
    bool                UpdatedEver;    /**< 曾经更新标志 */
} Com_SignalDataType;

typedef struct {
    uint8               Data[COM_MAX_PDU_LENGTH];   /**< PDU数据 */
    uint8               Length;                     /**< PDU长度 */
    bool                Pending;                    /**< 待发送标志 */
    uint16              Timer;                      /**< 周期定时器 */
} Com_IpduDataType;

typedef struct {
    bool                        Initialized;                        /**< 初始化标志 */
    const Com_ConfigType*       ConfigPtr;                          /**< 配置指针 */
    Com_SignalDataType          SignalData[COM_MAX_SIGNALS];        /**< 信号数据 */
    Com_IpduDataType            IpduData[COM_MAX_IPDU];             /**< I-PDU数据 */
} Com_GlobalType;

/*==================================================================================================
 *                                      全局变量
==================================================================================================*/
static Com_GlobalType Com_Global = {
    .Initialized = FALSE,
    .ConfigPtr = NULL_PTR,
    .SignalData = {{0, FALSE, FALSE}},
    .IpduData = {{{0}, 0, FALSE, 0}}
};

/*==================================================================================================
 *                                      私有函数
==================================================================================================*/

/**
 * @brief  查找信号配置
 */
static const Com_SignalConfigType* Com_FindSignalConfig(Com_SignalIdType SignalId)
{
    uint16 i;
    
    if (Com_Global.ConfigPtr == NULL_PTR) {
        return NULL_PTR;
    }
    
    for (i = 0; i < Com_Global.ConfigPtr->NumSignals; i++) {
        if (Com_Global.ConfigPtr->SignalConfig[i].SignalId == SignalId) {
            return &Com_Global.ConfigPtr->SignalConfig[i];
        }
    }
    
    return NULL_PTR;
}

/**
 * @brief  查找I-PDU配置
 */
static const Com_IpduConfigType* Com_FindIpduConfig(PduIdType PduId)
{
    uint16 i;
    
    if (Com_Global.ConfigPtr == NULL_PTR) {
        return NULL_PTR;
    }
    
    for (i = 0; i < Com_Global.ConfigPtr->NumIpdu; i++) {
        if (Com_Global.ConfigPtr->IpduConfig[i].PduId == PduId) {
            return &Com_Global.ConfigPtr->IpduConfig[i];
        }
    }
    
    return NULL_PTR;
}

/**
 * @brief  从PDU中提取信号值
 */
static uint64 Com_ExtractSignalValue(const uint8* pduData, uint32 bitPosition, uint32 bitSize, Com_SignalEndiannessType endianness)
{
    uint64 value = 0;
    uint32 i;
    uint32 byteStart = bitPosition / 8;
    uint32 bitOffset = bitPosition % 8;
    
    (void)endianness;  /* 简化实现，假设小端 */
    
    for (i = 0; i < ((bitSize + 7) / 8) && (byteStart + i) < COM_MAX_PDU_LENGTH; i++) {
        value |= ((uint64)pduData[byteStart + i]) << (i * 8);
    }
    
    /* 右移并掩码 */
    value >>= bitOffset;
    if (bitSize < 64) {
        value &= ((uint64)1 << bitSize) - 1;
    }
    
    return value;
}

/**
 * @brief  将信号值插入PDU
 */
static void Com_InsertSignalValue(uint8* pduData, uint64 value, uint32 bitPosition, uint32 bitSize, Com_SignalEndiannessType endianness)
{
    uint32 i;
    uint32 byteStart = bitPosition / 8;
    uint32 bitOffset = bitPosition % 8;
    uint64 mask;
    
    (void)endianness;  /* 简化实现，假设小端 */
    
    if (bitSize < 64) {
        mask = ((uint64)1 << bitSize) - 1;
        value &= mask;
    }
    
    value <<= bitOffset;
    mask = (((uint64)1 << bitSize) - 1) << bitOffset;
    
    for (i = 0; i < ((bitSize + bitOffset + 7) / 8) && (byteStart + i) < COM_MAX_PDU_LENGTH; i++) {
        pduData[byteStart + i] &= ~(uint8)(mask >> (i * 8));
        pduData[byteStart + i] |= (uint8)(value >> (i * 8));
    }
}

/*==================================================================================================
 *                                      公共函数
==================================================================================================*/

void Com_Init(const Com_ConfigType* config)
{
    uint16 i;
    
    if (config == NULL_PTR) {
        return;
    }
    
    if (Com_Global.Initialized) {
        return;
    }
    
    Com_Global.ConfigPtr = config;
    
    /* 初始化信号数据 */
    for (i = 0; i < COM_MAX_SIGNALS && i < config->NumSignals; i++) {
        Com_Global.SignalData[i].Value = config->SignalConfig[i].InitialValue;
        Com_Global.SignalData[i].Updated = FALSE;
        Com_Global.SignalData[i].UpdatedEver = FALSE;
    }
    
    /* 初始化I-PDU数据 */
    for (i = 0; i < COM_MAX_IPDU && i < config->NumIpdu; i++) {
        Com_Global.IpduData[i].Length = config->IpduConfig[i].PduLength;
        Com_Global.IpduData[i].Pending = FALSE;
        Com_Global.IpduData[i].Timer = 0;
        memset(Com_Global.IpduData[i].Data, 0, COM_MAX_PDU_LENGTH);
    }
    
    Com_Global.Initialized = TRUE;
}

void Com_DeInit(void)
{
    if (!Com_Global.Initialized) {
        return;
    }
    
    Com_Global.Initialized = FALSE;
    Com_Global.ConfigPtr = NULL_PTR;
}

void Com_GetVersionInfo(Std_VersionInfoType* versioninfo)
{
    if (versioninfo == NULL_PTR) {
        return;
    }
    
    versioninfo->vendorID = COM_VENDOR_ID;
    versioninfo->moduleID = COM_MODULE_ID;
    versioninfo->sw_major_version = COM_SW_MAJOR_VERSION;
    versioninfo->sw_minor_version = COM_SW_MINOR_VERSION;
    versioninfo->sw_patch_version = COM_SW_PATCH_VERSION;
}

uint8 Com_SendSignal(Com_SignalIdType SignalId, const void* SignalDataPtr)
{
    const Com_SignalConfigType* signalConfig;
    uint64 value = 0;
    Com_IpduDataType* ipduData;
    
    if (!Com_Global.Initialized) {
        return E_NOT_OK;
    }
    
    if (SignalDataPtr == NULL_PTR) {
        return E_NOT_OK;
    }
    
    signalConfig = Com_FindSignalConfig(SignalId);
    if (signalConfig == NULL_PTR) {
        return E_NOT_OK;
    }
    
    /* 获取信号值 */
    switch (signalConfig->SignalType) {
        case COM_UINT8_TYPE:
        case COM_BOOLEAN_TYPE:
            value = *(const uint8*)SignalDataPtr;
            break;
        case COM_UINT16_TYPE:
        case COM_SINT16_TYPE:
            value = *(const uint16*)SignalDataPtr;
            break;
        case COM_UINT32_TYPE:
        case COM_SINT32_TYPE:
        case COM_FLOAT32_TYPE:
            value = *(const uint32*)SignalDataPtr;
            break;
        case COM_UINT64_TYPE:
        case COM_SINT64_TYPE:
        case COM_FLOAT64_TYPE:
            value = *(const uint64*)SignalDataPtr;
            break;
        default:
            value = *(const uint8*)SignalDataPtr;
            break;
    }
    
    /* 保存信号值 */
    Com_Global.SignalData[signalConfig->SignalId].Value = value;
    Com_Global.SignalData[signalConfig->SignalId].Updated = TRUE;
    Com_Global.SignalData[signalConfig->SignalId].UpdatedEver = TRUE;
    
    /* 更新PDU */
    ipduData = &Com_Global.IpduData[signalConfig->IpduId];
    Com_InsertSignalValue(ipduData->Data, value, signalConfig->BitPosition, 
                          signalConfig->BitSize, signalConfig->Endianness);
    
    /* 标记待发送 */
    if (signalConfig->TransferProperty == COM_TRIGGERED ||
        signalConfig->TransferProperty == COM_TRIGGERED_WITHOUT_REPETITION) {
        ipduData->Pending = TRUE;
    }
    
    return E_OK;
}

uint8 Com_ReceiveSignal(Com_SignalIdType SignalId, void* SignalDataPtr)
{
    const Com_SignalConfigType* signalConfig;
    uint64 value;
    Com_IpduDataType* ipduData;
    
    if (!Com_Global.Initialized) {
        return E_NOT_OK;
    }
    
    if (SignalDataPtr == NULL_PTR) {
        return E_NOT_OK;
    }
    
    signalConfig = Com_FindSignalConfig(SignalId);
    if (signalConfig == NULL_PTR) {
        return E_NOT_OK;
    }
    
    /* 从PDU提取信号值 */
    ipduData = &Com_Global.IpduData[signalConfig->IpduId];
    value = Com_ExtractSignalValue(ipduData->Data, signalConfig->BitPosition,
                                   signalConfig->BitSize, signalConfig->Endianness);
    
    /* 转换并存储 */
    switch (signalConfig->SignalType) {
        case COM_UINT8_TYPE:
        case COM_BOOLEAN_TYPE:
            *(uint8*)SignalDataPtr = (uint8)value;
            break;
        case COM_UINT16_TYPE:
            *(uint16*)SignalDataPtr = (uint16)value;
            break;
        case COM_UINT32_TYPE:
            *(uint32*)SignalDataPtr = (uint32)value;
            break;
        case COM_UINT64_TYPE:
            *(uint64*)SignalDataPtr = value;
            break;
        case COM_SINT8_TYPE:
            *(sint8*)SignalDataPtr = (sint8)value;
            break;
        case COM_SINT16_TYPE:
            *(sint16*)SignalDataPtr = (sint16)value;
            break;
        case COM_SINT32_TYPE:
            *(sint32*)SignalDataPtr = (sint32)value;
            break;
        case COM_SINT64_TYPE:
            *(sint64*)SignalDataPtr = (sint64)value;
            break;
        default:
            *(uint8*)SignalDataPtr = (uint8)value;
            break;
    }
    
    return E_OK;
}

void Com_IpduGroupControl(Com_IpduGroupIdType ipduGroupVector, boolean bitval)
{
    (void)ipduGroupVector;
    (void)bitval;
    /* 简化实现 */
}

void Com_SwitchIpduTxMode(PduIdType PduId, Com_TxModeModeType Mode)
{
    (void)PduId;
    (void)Mode;
    /* 简化实现 */
}

void Com_MainFunctionRx(void)
{
    /* 接收处理 - 实际接收在RxIndication回调中完成 */
}

void Com_MainFunctionTx(void)
{
    uint16 i;
    PduInfoType pduInfo;
    const Com_IpduConfigType* ipduConfig;
    
    if (!Com_Global.Initialized) {
        return;
    }
    
    for (i = 0; i < Com_Global.ConfigPtr->NumIpdu; i++) {
        ipduConfig = &Com_Global.ConfigPtr->IpduConfig[i];
        
        if (!ipduConfig->IsTx) {
            continue;
        }
        
        /* 检查周期发送 */
        if (ipduConfig->Period > 0) {
            Com_Global.IpduData[i].Timer++;
            if (Com_Global.IpduData[i].Timer >= ipduConfig->Period) {
                Com_Global.IpduData[i].Timer = 0;
                Com_Global.IpduData[i].Pending = TRUE;
            }
        }
        
        /* 发送待发送的PDU */
        if (Com_Global.IpduData[i].Pending) {
            pduInfo.SduDataPtr = Com_Global.IpduData[i].Data;
            pduInfo.SduLength = Com_Global.IpduData[i].Length;
            pduInfo.MetaDataPtr = NULL_PTR;
            
            if (PduR_Transmit(ipduConfig->PduId, &pduInfo) == E_OK) {
                Com_Global.IpduData[i].Pending = FALSE;
            }
        }
    }
}

void Com_MainFunctionRouteSignals(void)
{
    /* 路由功能 - 简化实现 */
}

void Com_RxIndication(PduIdType RxPduId, const PduInfoType* PduInfoPtr)
{
    const Com_IpduConfigType* ipduConfig;
    uint16 i;
    
    if (!Com_Global.Initialized) {
        return;
    }
    
    if (PduInfoPtr == NULL_PTR) {
        return;
    }
    
    /* 查找I-PDU配置 */
    ipduConfig = Com_FindIpduConfig(RxPduId);
    if (ipduConfig == NULL_PTR) {
        return;
    }
    
    /* 保存PDU数据 */
    if (PduInfoPtr->SduDataPtr != NULL_PTR) {
        memcpy(Com_Global.IpduData[ipduConfig->PduId].Data, 
               PduInfoPtr->SduDataPtr, 
               (PduInfoPtr->SduLength > COM_MAX_PDU_LENGTH) ? COM_MAX_PDU_LENGTH : PduInfoPtr->SduLength);
        Com_Global.IpduData[ipduConfig->PduId].Length = (PduInfoPtr->SduLength > COM_MAX_PDU_LENGTH) ? COM_MAX_PDU_LENGTH : PduInfoPtr->SduLength;
    }
    
    /* 更新信号 */
    for (i = 0; i < Com_Global.ConfigPtr->NumSignals; i++) {
        if (Com_Global.ConfigPtr->SignalConfig[i].IpduId == ipduConfig->PduId) {
            Com_Global.SignalData[i].Updated = TRUE;
        }
    }
}

void Com_TxConfirmation(PduIdType TxPduId, Std_ReturnType result)
{
    (void)TxPduId;
    (void)result;
    /* 发送确认处理 */
}
