/**
 * @file        Dcm.h
 * @brief       诊断通信管理模块头文件
 * @details     提供UDS诊断服务支持
 *              简化版AUTOSAR Dcm实现
 * 
 * @author      [小昭debug]
 * @date        2026-03-15
 */

#ifndef DCM_H
#define DCM_H

#include "Std_Types.h"
#include "ComStack_Types.h"

/*==================================================================================================
 *                                      版本信息
==================================================================================================*/
#define DCM_VENDOR_ID               0x00
#define DCM_MODULE_ID               0x21
#define DCM_INSTANCE_ID             0x00

#define DCM_SW_MAJOR_VERSION        1
#define DCM_SW_MINOR_VERSION        0
#define DCM_SW_PATCH_VERSION        0

/*==================================================================================================
 *                                      UDS服务ID
==================================================================================================*/
#define DCM_SSID_DIAGNOSTIC_SESSION_CONTROL     0x10
#define DCM_SSID_ECU_RESET                      0x11
#define DCM_SSID_SECURITY_ACCESS                0x27
#define DCM_SSID_COMMUNICATION_CONTROL          0x28
#define DCM_SSID_TESTER_PRESENT                 0x3E
#define DCM_SSID_READ_DATA_BY_IDENTIFIER        0x22
#define DCM_SSID_READ_MEMORY_BY_ADDRESS         0x23
#define DCM_SSID_READ_SCALING_DATA_BY_IDENTIFIER    0x24
#define DCM_SSID_WRITE_DATA_BY_IDENTIFIER       0x2E
#define DCM_SSID_WRITE_MEMORY_BY_ADDRESS        0x3D
#define DCM_SSID_CLEAR_DIAGNOSTIC_INFORMATION   0x14
#define DCM_SSID_READ_DTC_INFORMATION           0x19
#define DCM_SSID_ROUTINE_CONTROL                0x31
#define DCM_SSID_REQUEST_DOWNLOAD               0x34
#define DCM_SSID_REQUEST_UPLOAD                 0x35
#define DCM_SSID_TRANSFER_DATA                  0x36
#define DCM_SSID_REQUEST_TRANSFER_EXIT          0x37
#define DCM_SSID_CONTROL_DTC_SETTING            0x85

/*==================================================================================================
 *                                      否定响应代码
==================================================================================================*/
#define DCM_E_GENERAL_REJECT                    0x10
#define DCM_E_SERVICE_NOT_SUPPORTED             0x11
#define DCM_E_SUBFUNCTION_NOT_SUPPORTED         0x12
#define DCM_E_INCORRECT_MESSAGE_LENGTH          0x13
#define DCM_E_CONDITIONS_NOT_CORRECT            0x22
#define DCM_E_REQUEST_SEQUENCE_ERROR            0x24
#define DCM_E_REQUEST_OUT_OF_RANGE              0x31
#define DCM_E_SECURITY_ACCESS_DENIED            0x33
#define DCM_E_INVALID_KEY                       0x35
#define DCM_E_EXCEED_NUMBER_OF_ATTEMPTS         0x36
#define DCM_E_REQUIRED_TIME_DELAY               0x37
#define DCM_E_UPLOAD_DOWNLOAD_NOT_ACCEPTED      0x70
#define DCM_E_TRANSFER_DATA_SUSPENDED           0x71
#define DCM_E_GENERAL_PROGRAMMING_FAILURE       0x72
#define DCM_E_WRONG_BLOCK_SEQUENCE_COUNTER      0x73
#define DCM_E_RESPONSE_TOO_LONG                 0x78

/*==================================================================================================
 *                                      诊断会话类型
==================================================================================================*/
typedef uint8 Dcm_SessionType;
#define DCM_DEFAULT_SESSION                     (Dcm_SessionType)0x01
#define DCM_PROGRAMMING_SESSION                 (Dcm_SessionType)0x02
#define DCM_EXTENDED_DIAGNOSTIC_SESSION         (Dcm_SessionType)0x03
#define DCM_SAFETY_SYSTEM_DIAGNOSTIC_SESSION    (Dcm_SessionType)0x04

/*==================================================================================================
 *                                      安全级别类型
==================================================================================================*/
typedef uint8 Dcm_SecLevelType;
#define DCM_SEC_LOCKED                          (Dcm_SecLevelType)0x00
#define DCM_SEC_L1                              (Dcm_SecLevelType)0x01
#define DCM_SEC_L2                              (Dcm_SecLevelType)0x02

/*==================================================================================================
 *                                      协议类型
==================================================================================================*/
typedef uint8 Dcm_ProtocolType;
#define DCM_OBD_ON_CAN                          (Dcm_ProtocolType)0x00
#define DCM_UDS_ON_CAN                          (Dcm_ProtocolType)0x01
#define DCM_UDS_ON_FLEXRAY                      (Dcm_ProtocolType)0x02
#define DCM_OBD_ON_FLEXRAY                      (Dcm_ProtocolType)0x03
#define DCM_OBD_ON_IP                           (Dcm_ProtocolType)0x04
#define DCM_UDS_ON_IP                           (Dcm_ProtocolType)0x05

/*==================================================================================================
 *                                      DTC格式类型
==================================================================================================*/
typedef uint8 Dcm_DTCFormatType;
#define DCM_DTC_FORMAT_OBD                      (Dcm_DTCFormatType)0x00
#define DCM_DTC_FORMAT_UDS                      (Dcm_DTCFormatType)0x01
#define DCM_DTC_FORMAT_SAE_J1939                (Dcm_DTCFormatType)0x02
#define DCM_DTC_FORMAT_ISO_11992_4              (Dcm_DTCFormatType)0x03

/*==================================================================================================
 *                                      DID数据结构
==================================================================================================*/
typedef uint16 Dcm_DidType;     /**< Data Identifier Type */
typedef uint32 Dcm_DtcType;     /**< DTC Type */

typedef struct {
    Dcm_DidType     Did;            /**< DID */
    uint8*          Data;           /**< 数据指针 */
    uint16          DataLength;     /**< 数据长度 */
    boolean         IsReadOnly;     /**< 是否只读 */
} Dcm_DidInfoType;

/*==================================================================================================
 *                                      DTC数据结构
==================================================================================================*/
typedef struct {
    Dcm_DtcType     Dtc;            /**< DTC */
    volatile uint8  Status;         /**< 状态 */
    uint8           Snapshot[16];   /**< 冻结帧 */
    uint8           ExtData[8];     /**< 扩展数据 */
} Dcm_DtcInfoType;

/*==================================================================================================
 *                                      诊断消息类型
==================================================================================================*/
typedef struct {
    uint8*          ReqData;        /**< 请求数据 */
    uint32          ReqDataLen;     /**< 请求数据长度 */
    uint8*          ResData;        /**< 响应数据 */
    uint32          ResDataLen;     /**< 响应数据长度 */
    PduIdType       RxPduId;        /**< 接收PDU ID */
} Dcm_MsgContextType;

/*==================================================================================================
 *                                      回调函数类型
==================================================================================================*/
typedef Std_ReturnType (*Dcm_ReadDidFuncType)(Dcm_DidType Did, uint8* Data, uint16* DataLen);
typedef Std_ReturnType (*Dcm_WriteDidFuncType)(Dcm_DidType Did, const uint8* Data, uint16 DataLen);
typedef Std_ReturnType (*Dcm_RoutineControlFuncType)(uint16 Rid, uint8 Action, const uint8* ReqData, uint16 ReqLen, uint8* ResData, uint16* ResLen);

/*==================================================================================================
 *                                      配置类型
==================================================================================================*/
typedef struct {
    uint16                  NumDids;            /**< DID数量 */
    const Dcm_DidInfoType*  DidTable;           /**< DID表 */
    uint16                  NumDtcs;            /**< DTC数量 */
    const Dcm_DtcInfoType*  DtcTable;           /**< DTC表 */
    Dcm_ReadDidFuncType     ReadDidFunc;        /**< 读DID回调 */
    Dcm_WriteDidFuncType    WriteDidFunc;       /**< 写DID回调 */
    Dcm_RoutineControlFuncType  RoutineFunc;    /**< 例程控制回调 */
    PduIdType               RxPduId;            /**< 接收PDU ID */
    PduIdType               TxPduId;            /**< 发送PDU ID */
} Dcm_ConfigType;

/*==================================================================================================
 *                                      函数声明
==================================================================================================*/

/**
 * @brief       初始化Dcm模块
 * @param       ConfigPtr   指向配置结构的指针
 */
void Dcm_Init(const Dcm_ConfigType* ConfigPtr);

/**
 * @brief       反初始化Dcm模块
 */
void Dcm_DeInit(void);

/**
 * @brief       获取版本信息
 * @param       versioninfo 指向版本信息结构的指针
 */
void Dcm_GetVersionInfo(Std_VersionInfoType* versioninfo);

/**
 * @brief       诊断处理主函数
 */
void Dcm_MainFunction(void);

/**
 * @brief       接收指示回调（由PduR调用）
 * @param       RxPduId     接收PDU ID
 * @param       PduInfoPtr  指向PDU信息的指针
 */
void Dcm_RxIndication(PduIdType RxPduId, const PduInfoType* PduInfoPtr);

/**
 * @brief       发送确认回调（由PduR调用）
 * @param       TxPduId     发送PDU ID
 * @param       result      发送结果
 */
void Dcm_TxConfirmation(PduIdType TxPduId, Std_ReturnType result);

/**
 * @brief       处理诊断请求
 * @param       msgContext  消息上下文
 * @return      E_OK: 成功, E_NOT_OK: 失败
 */
Std_ReturnType Dcm_ProcessDiagnosticRequest(Dcm_MsgContextType* msgContext);

/**
 * @brief       发送否定响应
 * @param       ServiceId       服务ID
 * @param       ErrorCode       错误代码
 */
void Dcm_SendNegativeResponse(uint8 ServiceId, uint8 ErrorCode);

/**
 * @brief       发送肯定响应
 * @param       ServiceId       服务ID
 * @param       Data            响应数据
 * @param       DataLen         数据长度
 */
void Dcm_SendPositiveResponse(uint8 ServiceId, const uint8* Data, uint16 DataLen);

/**
 * @brief       获取当前会话
 * @return      当前会话类型
 */
Dcm_SessionType Dcm_GetSession(void);

/**
 * @brief       设置会话
 * @param       session     目标会话
 */
void Dcm_SetSession(Dcm_SessionType session);

/**
 * @brief       获取当前安全级别
 * @return      当前安全级别
 */
Dcm_SecLevelType Dcm_GetSecurityLevel(void);

/**
 * @brief       设置安全级别
 * @param       secLevel    目标安全级别
 */
void Dcm_SetSecurityLevel(Dcm_SecLevelType secLevel);

/**
 * @brief       重置S3定时器
 */
void Dcm_ResetS3Timer(void);

/**
 * @brief       获取DTC状态
 * @param       Dtc     DTC代码
 * @return      DTC状态
 */
uint8 Dcm_GetDtcStatus(Dcm_DtcType Dtc);

/**
 * @brief       设置DTC状态
 * @param       Dtc         DTC代码
 * @param       Status      状态
 */
void Dcm_SetDtcStatus(Dcm_DtcType Dtc, uint8 Status);

/**
 * @brief       清除DTC
 * @param       Dtc         DTC代码，0xFFFFFF清除所有
 * @return      E_OK: 成功, E_NOT_OK: 失败
 */
Std_ReturnType Dcm_ClearDtc(Dcm_DtcType Dtc);

#endif /* DCM_H */
