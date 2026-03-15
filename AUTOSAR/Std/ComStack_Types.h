/**
 * @file        ComStack_Types.h
 * @brief       通信栈标准类型定义
 * @details     定义PDU相关信息类型，用于CAN、LIN、FlexRay等通信
 * 
 * @author      [小昭debug]
 * @date        2026-03-15
 */

#ifndef COMSTACK_TYPES_H
#define COMSTACK_TYPES_H

#include "Std_Types.h"

/*==================================================================================================
 *                                      PDU相关类型
==================================================================================================*/
typedef uint16 PduIdType;       /**< PDU标识符类型 */
typedef uint16 PduLengthType;   /**< PDU长度类型 */

/*==================================================================================================
 *                                      PDU信息类型
==================================================================================================*/
typedef struct {
    uint8*          SduDataPtr;     /**< 指向SDU数据的指针 */
    uint8*          MetaDataPtr;    /**< 指向元数据的指针 */
    PduLengthType   SduLength;      /**< SDU数据长度 */
} PduInfoType;

/*==================================================================================================
 *                                      缓冲区请求返回类型
==================================================================================================*/
typedef uint8 BufReq_ReturnType;
#define BUFREQ_OK           (BufReq_ReturnType)0x00  /**< 缓冲区请求成功 */
#define BUFREQ_E_NOT_OK     (BufReq_ReturnType)0x01  /**< 缓冲区请求失败 */
#define BUFREQ_E_BUSY       (BufReq_ReturnType)0x02  /**< 缓冲区忙 */
#define BUFREQ_E_OVFL       (BufReq_ReturnType)0x03  /**< 缓冲区溢出 */

/*==================================================================================================
 *                                      缓冲区请求类型
==================================================================================================*/
typedef struct {
    PduLengthType   SduLength;      /**< 请求的数据长度 */
} BufReq_Type;

/*==================================================================================================
 *                                      重试信息类型
==================================================================================================*/
typedef struct {
    bool            TpDataState;    /**< TP数据状态 */
    PduLengthType   TxTpDataCnt;    /**< 待传输TP数据计数 */
} RetryInfoType;

/*==================================================================================================
 *                                      通知结果类型
==================================================================================================*/
typedef uint8 NotifResultType;
#define NTFRSLT_OK              (NotifResultType)0x00   /**< 通知成功 */
#define NTFRSLT_E_NOT_OK        (NotifResultType)0x01   /**< 通知失败 */
#define NTFRSLT_E_TIMEOUT_A     (NotifResultType)0x02   /**< 超时A (N_Ar/N_As) */
#define NTFRSLT_E_TIMEOUT_Bs    (NotifResultType)0x03   /**< 超时Bs */
#define NTFRSLT_E_TIMEOUT_Cr    (NotifResultType)0x04   /**< 超时Cr */
#define NTFRSLT_E_WRONG_SN      (NotifResultType)0x05   /**< 错误的序列号 */
#define NTFRSLT_E_INVALID_FS    (NotifResultType)0x06   /**< 无效的流控状态 */
#define NTFRSLT_E_UNEXP_PDU     (NotifResultType)0x07   /**< 意外的PDU */
#define NTFRSLT_E_WFT_OVRN      (NotifResultType)0x08   /**< WaitFrame溢出 */
#define NTFRSLT_E_NO_BUFFER     (NotifResultType)0x09   /**< 无缓冲区 */
#define NTFRSLT_E_CANCELATION_OK    (NotifResultType)0x0A   /**< 取消成功 */
#define NTFRSLT_E_CANCELATION_NOT_OK    (NotifResultType)0x0B   /**< 取消失败 */

/*==================================================================================================
 *                                      TP参数类型
==================================================================================================*/
typedef uint8 TPParameterType;
#define TP_STMIN        (TPParameterType)0x00   /**< 最小分离时间 */
#define TP_BS           (TPParameterType)0x01   /**< 块大小 */
#define TP_BC           (TPParameterType)0x02   /**< 带宽控制 */

/*==================================================================================================
 *                                      I-PDU组向量类型
==================================================================================================*/
typedef uint8 IcomConfigIdType;
typedef uint8 IcomSwitch_StateType;
#define ICOM_SWITCH_E_OK        (IcomSwitch_StateType)0x00
#define ICOM_SWITCH_E_FAILED    (IcomSwitch_StateType)0x01

/*==================================================================================================
 *                                      物理通道类型
==================================================================================================*/
typedef uint8 PhysicalChannelType;

/*==================================================================================================
 *                                      网络句柄类型
==================================================================================================*/
typedef uint8 NetworkHandleType;
typedef uint8 NetworkHandleSubtype;

/*==================================================================================================
 *                                      总线类型定义
==================================================================================================*/
typedef uint8 BusType;
#define BUS_TYPE_CAN    (BusType)0x00   /**< CAN总线 */
#define BUS_TYPE_LIN    (BusType)0x01   /**< LIN总线 */
#define BUS_TYPE_FR     (BusType)0x02   /**< FlexRay总线 */
#define BUS_TYPE_ETH    (BusType)0x03   /**< 以太网 */

/*==================================================================================================
 *                                      协议类型
==================================================================================================*/
typedef uint8 ProtocolType;
#define PROTOCOL_TYPE_TP    (ProtocolType)0x00   /**< 传输协议 */
#define PROTOCOL_TYPE_IF    (ProtocolType)0x01   /**< 接口协议 */

/*==================================================================================================
 *                                      寻址模式
==================================================================================================*/
typedef uint8 CanTp_AddressingFormatType;
#define CANTP_STANDARD          (CanTp_AddressingFormatType)0x00   /**< 标准寻址 */
#define CANTP_EXTENDED          (CanTp_AddressingFormatType)0x01   /**< 扩展寻址 */
#define CANTP_MIXED             (CanTp_AddressingFormatType)0x02   /**< 混合寻址 */
#define CANTP_MIXED29BIT        (CanTp_AddressingFormatType)0x03   /**< 混合29位寻址 */
#define CANTP_NORMALFIXED       (CanTp_AddressingFormatType)0x04   /**< 正常固定寻址 */

/*==================================================================================================
 *                                      通信方向
==================================================================================================*/
typedef uint8 Com_DirectionType;
#define COM_SEND    (Com_DirectionType)0x00   /**< 发送方向 */
#define COM_RECEIVE (Com_DirectionType)0x01   /**< 接收方向 */

/*==================================================================================================
 *                                      信号类型
==================================================================================================*/
typedef uint8 Com_SignalType;
#define COM_UINT8       (Com_SignalType)0x00
#define COM_UINT16      (Com_SignalType)0x01
#define COM_UINT32      (Com_SignalType)0x02
#define COM_UINT64      (Com_SignalType)0x03
#define COM_SINT8       (Com_SignalType)0x04
#define COM_SINT16      (Com_SignalType)0x05
#define COM_SINT32      (Com_SignalType)0x06
#define COM_SINT64      (Com_SignalType)0x07
#define COM_FLOAT32     (Com_SignalType)0x08
#define COM_FLOAT64     (Com_SignalType)0x09
#define COM_BOOLEAN     (Com_SignalType)0x0A

#endif /* COMSTACK_TYPES_H */
