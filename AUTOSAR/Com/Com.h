/**
 * @file        Com.h
 * @brief       通信模块头文件
 * @details     提供信号到PDU的转换和通信服务
 *              简化版AUTOSAR Com实现
 * 
 * @author      [小昭debug]
 * @date        2026-03-15
 */

#ifndef COM_H
#define COM_H

#include "Std_Types.h"
#include "ComStack_Types.h"

/*==================================================================================================
 *                                      版本信息
==================================================================================================*/
#define COM_VENDOR_ID               0x00
#define COM_MODULE_ID               0x19
#define COM_INSTANCE_ID             0x00

#define COM_SW_MAJOR_VERSION        1
#define COM_SW_MINOR_VERSION        0
#define COM_SW_PATCH_VERSION        0

/*==================================================================================================
 *                                      开发错误代码
==================================================================================================*/
#define COM_E_PARAM_POINTER         0x01    /**< 参数指针错误 */
#define COM_E_PARAM                 0x02    /**< 参数错误 */
#define COM_E_PARAM_POINTER_DET_1   0x03    /**< 参数指针错误1 */
#define COM_E_PARAM_POINTER_DET_2   0x04    /**< 参数指针错误2 */
#define COM_E_UNINIT                0x05    /**< 未初始化 */
#define COM_E_PARAM_SIGNALID        0x06    /**< 信号ID错误 */
#define COM_E_PARAM_SIGNALID_DET    0x07    /**< 信号ID错误DET */

/*==================================================================================================
 *                                      运行时错误代码
==================================================================================================*/
#define COM_E_SKIPPING_COPY_CHANGED_RECEIVED    0x01
#define COM_E_SKIPPING_COPY_CHANGED_SIGNAL      0x02

/*==================================================================================================
 *                                      瞬态故障代码
==================================================================================================*/
#define COM_E_TOO_MANY_IPDU_COUNTERS    0x01
#define COM_E_TOO_MANY_SIGNAL_COUNTERS  0x02
#define COM_E_TOO_MANY_GROUP_COUNTERS   0x03

/*==================================================================================================
 *                                      服务ID
==================================================================================================*/
#define COM_SID_INIT                        0x01
#define COM_SID_DEINIT                      0x02
#define COM_SID_IPDUGROUPCONTROL            0x03
#define COM_SID_RECEPTIONDMCONTROL          0x06
#define COM_SID_GETVERSIONINFO              0x09
#define COM_SID_SENDSIGNAL                  0x0A
#define COM_SID_RECEIVESIGNAL               0x0B
#define COM_SID_SENDSIGNALGROUP             0x0D
#define COM_SID_RECEIVESIGNALGROUP          0x0E
#define COM_SID_SENDIPDU                    0x49
#define COM_SID_SENDDYNSIGNAL               0x21
#define COM_SID_RECEIVEDYNSIGNAL            0x22
#define COM_SID_SWITCHIPDUTXMODE            0x27
#define COM_SID_MAINFUNCTIONRX              0x01
#define COM_SID_MAINFUNCTIONTX              0x02
#define COM_SID_MAINFUNCTIONROUTESIGNALS    0x06

/*==================================================================================================
 *                                      信号类型
==================================================================================================*/
typedef uint16 Com_SignalIdType;        /**< 信号ID类型 */
typedef uint16 Com_SignalGroupIdType;   /**< 信号组ID类型 */
typedef uint16 Com_IpduGroupIdType;     /**< I-PDU组ID类型 */

/*==================================================================================================
 *                                      信号属性
==================================================================================================*/
typedef uint8 Com_TransferPropertyType;
#define COM_PENDING                     (Com_TransferPropertyType)0x00
#define COM_TRIGGERED                   (Com_TransferPropertyType)0x01
#define COM_TRIGGERED_WITHOUT_REPETITION    (Com_TransferPropertyType)0x02
#define COM_TRIGGERED_ON_CHANGE         (Com_TransferPropertyType)0x03
#define COM_TRIGGERED_ON_CHANGE_WITHOUT_REPETITION  (Com_TransferPropertyType)0x04

/*==================================================================================================
 *                                      字节序类型
==================================================================================================*/
typedef uint8 Com_SignalEndiannessType;
#define COM_BIG_ENDIAN          (Com_SignalEndiannessType)0x00
#define COM_LITTLE_ENDIAN       (Com_SignalEndiannessType)0x01
#define COM_OPAQUE              (Com_SignalEndiannessType)0x02

/*==================================================================================================
 *                                      信号数据类型
==================================================================================================*/
typedef uint8 Com_SignalTypeType;
#define COM_BOOLEAN_TYPE        (Com_SignalTypeType)0x00
#define COM_UINT8_TYPE          (Com_SignalTypeType)0x01
#define COM_UINT16_TYPE         (Com_SignalTypeType)0x02
#define COM_UINT32_TYPE         (Com_SignalTypeType)0x03
#define COM_UINT64_TYPE         (Com_SignalTypeType)0x04
#define COM_SINT8_TYPE          (Com_SignalTypeType)0x05
#define COM_SINT16_TYPE         (Com_SignalTypeType)0x06
#define COM_SINT32_TYPE         (Com_SignalTypeType)0x07
#define COM_SINT64_TYPE         (Com_SignalTypeType)0x08
#define COM_FLOAT32_TYPE        (Com_SignalTypeType)0x09
#define COM_FLOAT64_TYPE        (Com_SignalTypeType)0x0A
#define COM_UINT8_N_TYPE        (Com_SignalTypeType)0x0B
#define COM_UINT8_DYN_TYPE      (Com_SignalTypeType)0x0C
#define COM_ZOMBIE_TYPE         (Com_SignalTypeType)0x0D

/*==================================================================================================
 *                                      发送模式
==================================================================================================*/
typedef uint8 Com_TxModeModeType;
#define COM_DIRECT              (Com_TxModeModeType)0x00
#define COM_MIXED               (Com_TxModeModeType)0x01
#define COM_NONE                (Com_TxModeModeType)0x02
#define COM_PERIODIC            (Com_TxModeModeType)0x03

/*==================================================================================================
 *                                      I-PDU控制类型
==================================================================================================*/
typedef uint8 Com_IpduGroupVector[32];  /**< I-PDU组向量 */

/*==================================================================================================
 *                                      信号配置类型
==================================================================================================*/
typedef struct {
    Com_SignalIdType            SignalId;           /**< 信号ID */
    Com_SignalTypeType          SignalType;         /**< 信号类型 */
    Com_SignalEndiannessType    Endianness;         /**< 字节序 */
    uint32                      BitPosition;        /**< 起始位位置 */
    uint32                      BitSize;            /**< 位大小 */
    uint32                      InitialValue;       /**< 初始值 */
    PduIdType                   IpduId;             /**< 所属I-PDU ID */
    Com_TransferPropertyType    TransferProperty;   /**< 传输属性 */
} Com_SignalConfigType;

/*==================================================================================================
 *                                      I-PDU配置类型
==================================================================================================*/
typedef struct {
    PduIdType       PduId;          /**< PDU ID */
    uint8           PduLength;      /**< PDU长度 */
    uint16          Period;         /**< 周期(ms)，0表示非周期 */
    bool            IsTx;           /**< 发送/接收 */
} Com_IpduConfigType;

/*==================================================================================================
 *                                      COM配置类型
==================================================================================================*/
typedef struct {
    uint16                      NumSignals;         /**< 信号数量 */
    const Com_SignalConfigType* SignalConfig;       /**< 信号配置 */
    uint16                      NumIpdu;            /**< I-PDU数量 */
    const Com_IpduConfigType*   IpduConfig;         /**< I-PDU配置 */
} Com_ConfigType;

/*==================================================================================================
 *                                      回调函数类型
==================================================================================================*/
typedef void (*Com_CallbackRxIndicationType)(void);
typedef void (*Com_CallbackTxConfirmationType)(void);

/*==================================================================================================
 *                                      函数声明
==================================================================================================*/

/**
 * @brief       初始化Com模块
 * @param       config  指向配置结构的指针
 */
void Com_Init(const Com_ConfigType* config);

/**
 * @brief       反初始化Com模块
 */
void Com_DeInit(void);

/**
 * @brief       获取版本信息
 * @param       versioninfo 指向版本信息结构的指针
 */
void Com_GetVersionInfo(Std_VersionInfoType* versioninfo);

/**
 * @brief       发送信号
 * @param       SignalId    信号ID
 * @param       SignalDataPtr   指向信号数据的指针
 * @return      E_OK: 成功, E_NOT_OK: 失败
 */
uint8 Com_SendSignal(Com_SignalIdType SignalId, const void* SignalDataPtr);

/**
 * @brief       接收信号
 * @param       SignalId    信号ID
 * @param       SignalDataPtr   指向存储信号数据的指针
 * @return      E_OK: 成功, E_NOT_OK: 失败
 */
uint8 Com_ReceiveSignal(Com_SignalIdType SignalId, void* SignalDataPtr);

/**
 * @brief       I-PDU组控制
 * @param       ipduGroupVector     I-PDU组向量
 * @param       bitval              位值
 */
void Com_IpduGroupControl(Com_IpduGroupIdType ipduGroupVector, boolean bitval);

/**
 * @brief       切换I-PDU发送模式
 * @param       PduId   PDU ID
 * @param       Mode    发送模式
 */
void Com_SwitchIpduTxMode(PduIdType PduId, Com_TxModeModeType Mode);

/**
 * @brief       接收处理主函数
 */
void Com_MainFunctionRx(void);

/**
 * @brief       发送处理主函数
 */
void Com_MainFunctionTx(void);

/**
 * @brief       路由信号主函数
 */
void Com_MainFunctionRouteSignals(void);

/**
 * @brief       接收指示回调（由PduR调用）
 * @param       RxPduId     接收PDU ID
 * @param       PduInfoPtr  指向PDU信息的指针
 */
void Com_RxIndication(PduIdType RxPduId, const PduInfoType* PduInfoPtr);

/**
 * @brief       发送确认回调（由PduR调用）
 * @param       TxPduId     发送PDU ID
 * @param       result      发送结果
 */
void Com_TxConfirmation(PduIdType TxPduId, Std_ReturnType result);

#endif /* COM_H */
