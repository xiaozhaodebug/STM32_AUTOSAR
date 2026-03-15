/**
 * @file        Dcm.c
 * @brief       诊断通信管理模块实现
 * @details     简化版AUTOSAR Dcm实现，支持基本UDS服务
 * 
 * @author      [小昭debug]
 * @date        2026-03-15
 */

#include "Dcm.h"
#include "PduR.h"
#include <string.h>

/*==================================================================================================
 *                                      宏定义
==================================================================================================*/
#define DCM_MAX_BUFFER_SIZE     256     /**< 最大缓冲区大小 */
#define DCM_S3_TIMEOUT          5000    /**< S3定时器超时(ms) */
#define DCM_P2_TIMEOUT          500     /**< P2定时器超时(ms) */
#define DCM_P2STAR_TIMEOUT      5000    /**< P2*定时器超时(ms) */

/*==================================================================================================
 *                                      内部类型
==================================================================================================*/
typedef enum {
    DCM_IDLE,                   /**< 空闲状态 */
    DCM_RECEIVING,              /**< 接收中 */
    DCM_PROCESSING,             /**< 处理中 */
    DCM_SENDING                 /**< 发送中 */
} Dcm_StateType;

typedef struct {
    bool                        Initialized;          /**< 初始化标志 */
    const Dcm_ConfigType*       ConfigPtr;            /**< 配置指针 */
    Dcm_StateType               State;                /**< 状态机状态 */
    Dcm_SessionType             Session;              /**< 当前会话 */
    Dcm_SecLevelType            SecurityLevel;        /**< 当前安全级别 */
    uint32                      S3Timer;              /**< S3定时器 */
    uint32                      P2Timer;              /**< P2定时器 */
    uint8                       RxBuffer[DCM_MAX_BUFFER_SIZE];    /**< 接收缓冲区 */
    uint16                      RxBufferLen;                        /**< 接收长度 */
    uint8                       TxBuffer[DCM_MAX_BUFFER_SIZE];    /**< 发送缓冲区 */
    uint16                      TxBufferLen;                        /**< 发送长度 */
    boolean                     TxPending;              /**< 发送挂起 */
    PduIdType                   ActiveRxPduId;          /**< 活动接收PDU ID */
} Dcm_GlobalType;

/*==================================================================================================
 *                                      全局变量
==================================================================================================*/
static Dcm_GlobalType Dcm_Global = {
    .Initialized = FALSE,
    .ConfigPtr = NULL_PTR,
    .State = DCM_IDLE,
    .Session = DCM_DEFAULT_SESSION,
    .SecurityLevel = DCM_SEC_LOCKED,
    .S3Timer = 0,
    .P2Timer = 0,
    .RxBuffer = {0},
    .RxBufferLen = 0,
    .TxBuffer = {0},
    .TxBufferLen = 0,
    .TxPending = FALSE,
    .ActiveRxPduId = 0
};

/*==================================================================================================
 *                                      私有函数
==================================================================================================*/

/**
 * @brief  处理DiagnosticSessionControl (0x10)
 */
static Std_ReturnType Dcm_ProcessSessionControl(const uint8* reqData, uint16 reqLen, uint8* resData, uint16* resLen)
{
    uint8 subfunction;
    
    if (reqLen < 2) {
        return E_NOT_OK;
    }
    
    subfunction = reqData[1] & 0x7F;
    
    switch (subfunction) {
        case 0x01:  /* 默认会话 */
            Dcm_Global.Session = DCM_DEFAULT_SESSION;
            Dcm_Global.SecurityLevel = DCM_SEC_LOCKED;
            break;
        case 0x02:  /* 编程会话 */
            Dcm_Global.Session = DCM_PROGRAMMING_SESSION;
            break;
        case 0x03:  /* 扩展诊断会话 */
            Dcm_Global.Session = DCM_EXTENDED_DIAGNOSTIC_SESSION;
            break;
        default:
            return E_NOT_OK;
    }
    
    /* 构造响应 */
    resData[0] = DCM_SSID_DIAGNOSTIC_SESSION_CONTROL + 0x40;
    resData[1] = subfunction;
    resData[2] = (DCM_P2_TIMEOUT >> 8) & 0xFF;      /* P2高位 */
    resData[3] = DCM_P2_TIMEOUT & 0xFF;              /* P2低位 */
    resData[4] = (DCM_P2STAR_TIMEOUT >> 8) & 0xFF;  /* P2*高位 */
    resData[5] = DCM_P2STAR_TIMEOUT & 0xFF;          /* P2*低位 */
    *resLen = 6;
    
    return E_OK;
}

/**
 * @brief  处理EcuReset (0x11)
 */
static Std_ReturnType Dcm_ProcessEcuReset(const uint8* reqData, uint16 reqLen, uint8* resData, uint16* resLen)
{
    uint8 subfunction;
    
    if (reqLen < 2) {
        return E_NOT_OK;
    }
    
    subfunction = reqData[1] & 0x7F;
    
    /* 构造响应 */
    resData[0] = DCM_SSID_ECU_RESET + 0x40;
    resData[1] = subfunction;
    *resLen = 2;
    
    /* 发送响应后执行复位 */
    /* 实际复位在发送确认后执行 */
    
    return E_OK;
}

/**
 * @brief  处理ReadDataByIdentifier (0x22)
 */
static Std_ReturnType Dcm_ProcessReadDid(const uint8* reqData, uint16 reqLen, uint8* resData, uint16* resLen)
{
    Dcm_DidType did;
    uint16 i;
    Std_ReturnType result = E_NOT_OK;
    
    if (reqLen < 3) {
        return E_NOT_OK;
    }
    
    /* 提取DID */
    did = ((uint16)reqData[1] << 8) | reqData[2];
    
    /* 查找DID */
    for (i = 0; i < Dcm_Global.ConfigPtr->NumDids; i++) {
        if (Dcm_Global.ConfigPtr->DidTable[i].Did == did) {
            /* 构造响应 */
            resData[0] = DCM_SSID_READ_DATA_BY_IDENTIFIER + 0x40;
            resData[1] = (did >> 8) & 0xFF;
            resData[2] = did & 0xFF;
            
            /* 复制数据 */
            memcpy(&resData[3], Dcm_Global.ConfigPtr->DidTable[i].Data, 
                   Dcm_Global.ConfigPtr->DidTable[i].DataLength);
            *resLen = 3 + Dcm_Global.ConfigPtr->DidTable[i].DataLength;
            
            result = E_OK;
            break;
        }
    }
    
    /* 如果配置了回调函数，也尝试调用 */
    if (result != E_OK && Dcm_Global.ConfigPtr->ReadDidFunc != NULL_PTR) {
        uint16 dataLen = DCM_MAX_BUFFER_SIZE - 3;
        if (Dcm_Global.ConfigPtr->ReadDidFunc(did, &resData[3], &dataLen) == E_OK) {
            resData[0] = DCM_SSID_READ_DATA_BY_IDENTIFIER + 0x40;
            resData[1] = (did >> 8) & 0xFF;
            resData[2] = did & 0xFF;
            *resLen = 3 + dataLen;
            result = E_OK;
        }
    }
    
    return result;
}

/**
 * @brief  处理WriteDataByIdentifier (0x2E)
 */
static Std_ReturnType Dcm_ProcessWriteDid(const uint8* reqData, uint16 reqLen, uint8* resData, uint16* resLen)
{
    Dcm_DidType did;
    uint16 i;
    Std_ReturnType result = E_NOT_OK;
    
    if (reqLen < 3) {
        return E_NOT_OK;
    }
    
    /* 提取DID */
    did = ((uint16)reqData[1] << 8) | reqData[2];
    
    /* 检查安全级别 */
    if (Dcm_Global.SecurityLevel == DCM_SEC_LOCKED) {
        return E_NOT_OK;  /* 需要解锁 */
    }
    
    /* 查找DID */
    for (i = 0; i < Dcm_Global.ConfigPtr->NumDids; i++) {
        if (Dcm_Global.ConfigPtr->DidTable[i].Did == did) {
            /* 检查是否只读 */
            if (Dcm_Global.ConfigPtr->DidTable[i].IsReadOnly) {
                return E_NOT_OK;
            }
            
            /* 写入数据 */
            uint16 dataLen = reqLen - 3;
            if (dataLen > Dcm_Global.ConfigPtr->DidTable[i].DataLength) {
                dataLen = Dcm_Global.ConfigPtr->DidTable[i].DataLength;
            }
            memcpy(Dcm_Global.ConfigPtr->DidTable[i].Data, &reqData[3], dataLen);
            
            /* 构造响应 */
            resData[0] = DCM_SSID_WRITE_DATA_BY_IDENTIFIER + 0x40;
            resData[1] = (did >> 8) & 0xFF;
            resData[2] = did & 0xFF;
            *resLen = 3;
            
            result = E_OK;
            break;
        }
    }
    
    /* 如果配置了回调函数 */
    if (result != E_OK && Dcm_Global.ConfigPtr->WriteDidFunc != NULL_PTR) {
        if (Dcm_Global.ConfigPtr->WriteDidFunc(did, &reqData[3], reqLen - 3) == E_OK) {
            resData[0] = DCM_SSID_WRITE_DATA_BY_IDENTIFIER + 0x40;
            resData[1] = (did >> 8) & 0xFF;
            resData[2] = did & 0xFF;
            *resLen = 3;
            result = E_OK;
        }
    }
    
    return result;
}

/**
 * @brief  处理TesterPresent (0x3E)
 */
static Std_ReturnType Dcm_ProcessTesterPresent(const uint8* reqData, uint16 reqLen, uint8* resData, uint16* resLen)
{
    uint8 subfunction;
    
    if (reqLen < 2) {
        return E_NOT_OK;
    }
    
    subfunction = reqData[1] & 0x7F;
    
    /* 重置S3定时器 */
    Dcm_Global.S3Timer = 0;
    
    /* 构造响应 */
    resData[0] = DCM_SSID_TESTER_PRESENT + 0x40;
    resData[1] = subfunction;
    *resLen = 2;
    
    return E_OK;
}

/**
 * @brief  处理SecurityAccess (0x27)
 */
static Std_ReturnType Dcm_ProcessSecurityAccess(const uint8* reqData, uint16 reqLen, uint8* resData, uint16* resLen)
{
    uint8 subfunction;
    
    if (reqLen < 2) {
        return E_NOT_OK;
    }
    
    subfunction = reqData[1];
    
    /* 请求种子 */
    if ((subfunction & 0x01) == 0x01) {
        uint8 seedLevel = subfunction;
        
        /* 构造响应 - 简化实现，返回固定种子 */
        resData[0] = DCM_SSID_SECURITY_ACCESS + 0x40;
        resData[1] = seedLevel;
        resData[2] = 0x12;  /* 种子字节1 */
        resData[3] = 0x34;  /* 种子字节2 */
        resData[4] = 0x56;  /* 种子字节3 */
        resData[5] = 0x78;  /* 种子字节4 */
        *resLen = 6;
        
        return E_OK;
    }
    /* 发送密钥 */
    else if (subfunction == 0x02) {
        /* 简化实现，接受任何密钥 */
        Dcm_Global.SecurityLevel = DCM_SEC_L1;
        
        resData[0] = DCM_SSID_SECURITY_ACCESS + 0x40;
        resData[1] = subfunction;
        *resLen = 2;
        
        return E_OK;
    }
    
    return E_NOT_OK;
}

/**
 * @brief  处理诊断服务
 */
static Std_ReturnType Dcm_ProcessService(uint8 serviceId, const uint8* reqData, uint16 reqLen, uint8* resData, uint16* resLen)
{
    Std_ReturnType result = E_NOT_OK;
    
    switch (serviceId) {
        case DCM_SSID_DIAGNOSTIC_SESSION_CONTROL:
            result = Dcm_ProcessSessionControl(reqData, reqLen, resData, resLen);
            break;
        case DCM_SSID_ECU_RESET:
            result = Dcm_ProcessEcuReset(reqData, reqLen, resData, resLen);
            break;
        case DCM_SSID_SECURITY_ACCESS:
            result = Dcm_ProcessSecurityAccess(reqData, reqLen, resData, resLen);
            break;
        case DCM_SSID_READ_DATA_BY_IDENTIFIER:
            result = Dcm_ProcessReadDid(reqData, reqLen, resData, resLen);
            break;
        case DCM_SSID_WRITE_DATA_BY_IDENTIFIER:
            result = Dcm_ProcessWriteDid(reqData, reqLen, resData, resLen);
            break;
        case DCM_SSID_TESTER_PRESENT:
            result = Dcm_ProcessTesterPresent(reqData, reqLen, resData, resLen);
            break;
        default:
            /* 不支持的服务 */
            break;
    }
    
    return result;
}

/*==================================================================================================
 *                                      公共函数
==================================================================================================*/

void Dcm_Init(const Dcm_ConfigType* ConfigPtr)
{
    if (ConfigPtr == NULL_PTR) {
        return;
    }
    
    if (Dcm_Global.Initialized) {
        return;
    }
    
    Dcm_Global.ConfigPtr = ConfigPtr;
    Dcm_Global.State = DCM_IDLE;
    Dcm_Global.Session = DCM_DEFAULT_SESSION;
    Dcm_Global.SecurityLevel = DCM_SEC_LOCKED;
    Dcm_Global.S3Timer = 0;
    Dcm_Global.P2Timer = 0;
    Dcm_Global.RxBufferLen = 0;
    Dcm_Global.TxBufferLen = 0;
    Dcm_Global.TxPending = FALSE;
    
    Dcm_Global.Initialized = TRUE;
}

void Dcm_DeInit(void)
{
    if (!Dcm_Global.Initialized) {
        return;
    }
    
    Dcm_Global.Initialized = FALSE;
    Dcm_Global.ConfigPtr = NULL_PTR;
    Dcm_Global.State = DCM_IDLE;
}

void Dcm_GetVersionInfo(Std_VersionInfoType* versioninfo)
{
    if (versioninfo == NULL_PTR) {
        return;
    }
    
    versioninfo->vendorID = DCM_VENDOR_ID;
    versioninfo->moduleID = DCM_MODULE_ID;
    versioninfo->sw_major_version = DCM_SW_MAJOR_VERSION;
    versioninfo->sw_minor_version = DCM_SW_MINOR_VERSION;
    versioninfo->sw_patch_version = DCM_SW_PATCH_VERSION;
}

void Dcm_MainFunction(void)
{
    if (!Dcm_Global.Initialized) {
        return;
    }
    
    /* 处理状态机 */
    switch (Dcm_Global.State) {
        case DCM_RECEIVING:
            /* 接收完成，开始处理 */
            Dcm_Global.State = DCM_PROCESSING;
            break;
            
        case DCM_PROCESSING:
            {
                uint8 pci = Dcm_Global.RxBuffer[0];  /* Protocol Control Information */
                uint8 frameType = pci & 0xF0;        /* 帧类型: 0x00=单帧, 0x10=首帧 */
                uint8 dataLen = pci & 0x0F;          /* 单帧数据长度 */
                uint8 serviceId;
                uint8* reqData;
                uint16 reqLen;
                Std_ReturnType result;
                
                /* 解析ISO-TP单帧格式 */
                if (frameType == 0x00) {
                    /* 单帧(SF): Byte0=[0|DLC], Byte1=SID, Byte2+...=数据 */
                    serviceId = Dcm_Global.RxBuffer[1];
                    reqData = &Dcm_Global.RxBuffer[1];  /* SID开始的地址 */
                    reqLen = dataLen;  /* 实际数据长度(不含PCI) */
                } else if (frameType == 0x10) {
                    /* 首帧(FF): 需要TP层支持, 简化处理 */
                    serviceId = Dcm_Global.RxBuffer[2];
                    reqData = &Dcm_Global.RxBuffer[2];
                    reqLen = Dcm_Global.RxBufferLen - 2;
                } else {
                    /* 其他帧类型不支持 */
                    Dcm_Global.State = DCM_IDLE;
                    break;
                }
                
                result = Dcm_ProcessService(serviceId, reqData, reqLen,
                                           Dcm_Global.TxBuffer, 
                                           &Dcm_Global.TxBufferLen);
                
                if (result == E_OK) {
                    /* 发送肯定响应 - 添加单帧PCI */
                    uint8 i;
                    /* 将响应数据后移一位，插入PCI字节 */
                    for (i = Dcm_Global.TxBufferLen; i > 0; i--) {
                        Dcm_Global.TxBuffer[i] = Dcm_Global.TxBuffer[i-1];
                    }
                    Dcm_Global.TxBuffer[0] = 0x00 | Dcm_Global.TxBufferLen;  /* 单帧PCI */
                    Dcm_Global.TxBufferLen += 1;
                    Dcm_Global.TxPending = TRUE;
                } else {
                    /* 发送否定响应 - 添加单帧PCI */
                    Dcm_Global.TxBuffer[3] = Dcm_Global.TxBuffer[2];  /* 错误码后移 */
                    Dcm_Global.TxBuffer[2] = serviceId;               /* SID */
                    Dcm_Global.TxBuffer[1] = 0x7F;                    /* 否定响应服务 */
                    Dcm_Global.TxBuffer[0] = 0x03;                    /* 单帧PCI, len=3 */
                    Dcm_Global.TxBufferLen = 4;
                    Dcm_Global.TxPending = TRUE;
                }
                
                Dcm_Global.State = DCM_SENDING;
            }
            break;
            
        case DCM_SENDING:
            if (Dcm_Global.TxPending) {
                PduInfoType pduInfo;
                pduInfo.SduDataPtr = Dcm_Global.TxBuffer;
                pduInfo.SduLength = Dcm_Global.TxBufferLen;
                pduInfo.MetaDataPtr = NULL_PTR;
                
                if (PduR_Transmit(Dcm_Global.ConfigPtr->TxPduId, &pduInfo) == E_OK) {
                    Dcm_Global.TxPending = FALSE;
                    Dcm_Global.State = DCM_IDLE;
                    Dcm_Global.RxBufferLen = 0;
                }
            }
            break;
            
        case DCM_IDLE:
        default:
            break;
    }
    
    /* S3定时器处理 */
    if (Dcm_Global.Session != DCM_DEFAULT_SESSION) {
        Dcm_Global.S3Timer++;
        if (Dcm_Global.S3Timer >= DCM_S3_TIMEOUT) {
            /* 超时，返回默认会话 */
            Dcm_Global.Session = DCM_DEFAULT_SESSION;
            Dcm_Global.SecurityLevel = DCM_SEC_LOCKED;
            Dcm_Global.S3Timer = 0;
        }
    }
}

void Dcm_RxIndication(PduIdType RxPduId, const PduInfoType* PduInfoPtr)
{
    if (!Dcm_Global.Initialized) {
        return;
    }
    
    if (PduInfoPtr == NULL_PTR || PduInfoPtr->SduDataPtr == NULL_PTR) {
        return;
    }
    
    if (Dcm_Global.State != DCM_IDLE) {
        return;  /* 忙，忽略新请求 */
    }
    
    if (PduInfoPtr->SduLength > DCM_MAX_BUFFER_SIZE) {
        return;
    }
    
    /* 只处理诊断相关的PDU (0x7DF=PDU10, 0x735=PDU11) */
    if (RxPduId != 10 && RxPduId != 11) {
        return;
    }
    
    /* 保存接收数据 */
    memcpy(Dcm_Global.RxBuffer, PduInfoPtr->SduDataPtr, PduInfoPtr->SduLength);
    Dcm_Global.RxBufferLen = PduInfoPtr->SduLength;
    Dcm_Global.ActiveRxPduId = RxPduId;
    
    /* 进入接收状态 */
    Dcm_Global.State = DCM_RECEIVING;
}

void Dcm_TxConfirmation(PduIdType TxPduId, Std_ReturnType result)
{
    (void)TxPduId;
    (void)result;
    /* 发送确认处理 */
}

Std_ReturnType Dcm_ProcessDiagnosticRequest(Dcm_MsgContextType* msgContext)
{
    Std_ReturnType result;
    uint16 resDataLen;
    
    if (msgContext == NULL_PTR) {
        return E_NOT_OK;
    }
    
    /* 使用临时uint16变量解决类型不匹配 */
    resDataLen = (uint16)msgContext->ResDataLen;
    
    result = Dcm_ProcessService(msgContext->ReqData[0], msgContext->ReqData,
                               (uint16)msgContext->ReqDataLen, msgContext->ResData,
                               &resDataLen);
    
    msgContext->ResDataLen = resDataLen;
    
    return result;
}

void Dcm_SendNegativeResponse(uint8 ServiceId, uint8 ErrorCode)
{
    Dcm_Global.TxBuffer[0] = 0x7F;
    Dcm_Global.TxBuffer[1] = ServiceId;
    Dcm_Global.TxBuffer[2] = ErrorCode;
    Dcm_Global.TxBufferLen = 3;
    Dcm_Global.TxPending = TRUE;
}

void Dcm_SendPositiveResponse(uint8 ServiceId, const uint8* Data, uint16 DataLen)
{
    uint16 i;
    
    Dcm_Global.TxBuffer[0] = ServiceId + 0x40;
    
    if (Data != NULL_PTR && DataLen > 0) {
        for (i = 0; i < DataLen && i < (DCM_MAX_BUFFER_SIZE - 1); i++) {
            Dcm_Global.TxBuffer[1 + i] = Data[i];
        }
        Dcm_Global.TxBufferLen = 1 + DataLen;
    } else {
        Dcm_Global.TxBufferLen = 1;
    }
    
    Dcm_Global.TxPending = TRUE;
}

Dcm_SessionType Dcm_GetSession(void)
{
    return Dcm_Global.Session;
}

void Dcm_SetSession(Dcm_SessionType session)
{
    Dcm_Global.Session = session;
}

Dcm_SecLevelType Dcm_GetSecurityLevel(void)
{
    return Dcm_Global.SecurityLevel;
}

void Dcm_SetSecurityLevel(Dcm_SecLevelType secLevel)
{
    Dcm_Global.SecurityLevel = secLevel;
}

void Dcm_ResetS3Timer(void)
{
    Dcm_Global.S3Timer = 0;
}

uint8 Dcm_GetDtcStatus(Dcm_DtcType Dtc)
{
    uint16 i;
    
    if (Dcm_Global.ConfigPtr == NULL_PTR) {
        return 0;
    }
    
    for (i = 0; i < Dcm_Global.ConfigPtr->NumDtcs; i++) {
        if (Dcm_Global.ConfigPtr->DtcTable[i].Dtc == Dtc) {
            return Dcm_Global.ConfigPtr->DtcTable[i].Status;
        }
    }
    
    return 0;
}

void Dcm_SetDtcStatus(Dcm_DtcType Dtc, uint8 Status)
{
    /* 简化实现，实际应该修改DtcTable，但由于是const需要其他方式 */
    (void)Dtc;
    (void)Status;
}

Std_ReturnType Dcm_ClearDtc(Dcm_DtcType Dtc)
{
    /* 简化实现 */
    (void)Dtc;
    return E_OK;
}
