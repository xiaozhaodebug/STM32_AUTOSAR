/**
 * @file        PduR.h
 * @brief       PDU路由器模块头文件
 * @details     提供PDU路由服务，连接不同通信层模块
 *              简化版AUTOSAR PduR实现
 * 
 * @author      [小昭debug]
 * @date        2026-03-15
 */

#ifndef PDUR_H
#define PDUR_H

#include "Std_Types.h"
#include "ComStack_Types.h"

/*==================================================================================================
 *                                      版本信息
==================================================================================================*/
#define PDUR_VENDOR_ID              0x00
#define PDUR_MODULE_ID              0x69
#define PDUR_INSTANCE_ID            0x00

#define PDUR_SW_MAJOR_VERSION       1
#define PDUR_SW_MINOR_VERSION       0
#define PDUR_SW_PATCH_VERSION       0

/*==================================================================================================
 *                                      开发错误代码
==================================================================================================*/
#define PDUR_E_INVALID_REQUEST      0x01    /**< 无效请求 */
#define PDUR_E_PDUID_INVALID        0x02    /**< 无效的PDU ID */
#define PDUR_E_BUFFER_REQUEST       0x03    /**< 缓冲区请求错误 */
#define PDUR_E_CONFIG_PTR_INVALID   0x04    /**< 配置指针无效 */
#define PDUR_E_INVALID_PARAMETER    0x05    /**< 无效参数 */
#define PDUR_E_LOIF_TX_CONFIRMATION 0x06    /**< 下层发送确认错误 */
#define PDUR_E_PDU_ID_INVALID       0x07    /**< PDU ID无效 */
#define PDUR_E_DUPLICATE_CONFIGURED 0x08    /**< 重复配置 */
#define PDUR_E_ROUTING_PATH_GROUP_ID_INVALID    0x09    /**< 路由路径组ID无效 */

/*==================================================================================================
 *                                      运行时错误代码
==================================================================================================*/
#define PDUR_E_PDU_INSTANCES_LOST   0x01    /**< PDU实例丢失 */

/*==================================================================================================
 *                                      瞬态故障代码
==================================================================================================*/
#define PDUR_E_LOIF_TX_FAILED       0x01    /**< 下层发送失败 */

/*==================================================================================================
 *                                      服务ID
==================================================================================================*/
#define PDUR_SID_INIT                           0x00
#define PDUR_SID_DEINIT                         0x01
#define PDUR_SID_GETVERSIONINFO                 0x02
#define PDUR_SID_TRANSMIT                       0x03
#define PDUR_SID_CANCELTRANSMIT                 0x04
#define PDUR_SID_CANCELRECEIVE                  0x05
#define PDUR_SID_CHANGEPARAMETERRQST            0x06
#define PDUR_SID_IFRXINDICATION                 0x20
#define PDURSID_IFTXCONFIRMATION                0x21
#define PDUR_SID_IFTRIGGERTRANSMIT              0x22
#define PDUR_SID_TPCOPYTXDATA                   0x23
#define PDUR_SID_TPTXCONFIRMATION               0x24
#define PDUR_SID_TPRXINDICATION                 0x25
#define PDUR_SID_TPCOPYRXDATA                   0x26
#define PDUR_SID_TPRXINDICATION_TM              0x27
#define PDUR_SID_TPCANCELTRANSMIT               0x28
#define PDUR_SID_TPCANCELRECEIVE                0x29
#define PDUR_SID_ENABLEROUTING                  0x2A
#define PDUR_SID_DISABLEROUTING                 0x2B
#define PDUR_SID_IFTRIGGERCANCELTRANSMIT        0x2C
#define PDUR_SID_BUFFERDET                      0x2D
#define PDUR_SID_MULTICASTTXTOLO                0x2E

/*==================================================================================================
 *                                      路由路径配置类型
==================================================================================================*/
typedef uint8 PduR_RoutingPathType;
#define PDUR_ROUTINGPATH_IF     (PduR_RoutingPathType)0x00   /**< IF路由 */
#define PDUR_ROUTINGPATH_TP     (PduR_RoutingPathType)0x01   /**< TP路由 */

/*==================================================================================================
 *                                      路由目标模块类型
==================================================================================================*/
typedef uint8 PduR_DestModuleType;
#define PDUR_DEST_MODULE_CANIF  (PduR_DestModuleType)0x00
#define PDUR_DEST_MODULE_CANTP  (PduR_DestModuleType)0x01
#define PDUR_DEST_MODULE_COM    (PduR_DestModuleType)0x02
#define PDUR_DEST_MODULE_DCM    (PduR_DestModuleType)0x03

/*==================================================================================================
 *                                      路由路径配置
==================================================================================================*/
typedef struct {
    PduIdType               SrcPduId;       /**< 源PDU ID */
    PduIdType               DestPduId;      /**< 目标PDU ID */
    PduR_DestModuleType     DestModule;     /**< 目标模块 */
    PduR_RoutingPathType    PathType;       /**< 路由路径类型 */
} PduR_RoutingPathConfigType;

/*==================================================================================================
 *                                      PDU路由器配置
==================================================================================================*/
typedef struct {
    uint16                          NumRoutingPaths;    /**< 路由路径数量 */
    const PduR_RoutingPathConfigType*   RoutingPaths;       /**< 路由路径配置 */
} PduR_ConfigType;

/*==================================================================================================
 *                                      回调函数类型
==================================================================================================*/
typedef Std_ReturnType (*PduR_TransmitFuncType)(PduIdType TxPduId, const PduInfoType* PduInfoPtr);
typedef void (*PduR_RxIndicationFuncType)(PduIdType RxPduId, const PduInfoType* PduInfoPtr);
typedef void (*PduR_TxConfirmationFuncType)(PduIdType TxPduId, Std_ReturnType result);

/*==================================================================================================
 *                                      函数声明
==================================================================================================*/

/**
 * @brief       初始化PduR模块
 * @param       ConfigPtr   指向配置结构的指针
 */
void PduR_Init(const PduR_ConfigType* ConfigPtr);

/**
 * @brief       反初始化PduR模块
 */
void PduR_DeInit(void);

/**
 * @brief       获取版本信息
 * @param       VersionInfo 指向版本信息结构的指针
 */
void PduR_GetVersionInfo(Std_VersionInfoType* VersionInfo);

/**
 * @brief       发送PDU
 * @param       TxPduId     发送PDU ID
 * @param       PduInfoPtr  指向PDU信息的指针
 * @return      E_OK: 成功, E_NOT_OK: 失败
 */
Std_ReturnType PduR_Transmit(PduIdType TxPduId, const PduInfoType* PduInfoPtr);

/**
 * @brief       取消发送
 * @param       TxPduId     发送PDU ID
 * @return      E_OK: 成功, E_NOT_OK: 失败
 */
Std_ReturnType PduR_CancelTransmit(PduIdType TxPduId);

/**
 * @brief       取消接收
 * @param       RxPduId     接收PDU ID
 * @return      E_OK: 成功, E_NOT_OK: 失败
 */
Std_ReturnType PduR_CancelReceive(PduIdType RxPduId);

/**
 * @brief       IF层接收指示（由下层调用）
 * @param       RxPduId     接收PDU ID
 * @param       PduInfoPtr  指向PDU信息的指针
 */
void PduR_RxIndication(PduIdType RxPduId, const PduInfoType* PduInfoPtr);

/**
 * @brief       IF层发送确认（由下层调用）
 * @param       TxPduId     发送PDU ID
 * @param       result      发送结果
 */
void PduR_TxConfirmation(PduIdType TxPduId, Std_ReturnType result);

/**
 * @brief       TP层接收指示（由下层调用）
 * @param       id          PDU ID
 * @param       result      接收结果
 * @param       TpSduLength TP SDU长度
 */
void PduR_TpRxIndication(PduIdType id, Std_ReturnType result, PduLengthType TpSduLength);

/**
 * @brief       TP层发送确认（由下层调用）
 * @param       id          PDU ID
 * @param       result      发送结果
 */
void PduR_TpTxConfirmation(PduIdType id, Std_ReturnType result);

/**
 * @brief       TP层复制接收数据（由下层调用）
 * @param       id          PDU ID
 * @param       info        指向PDU信息的指针
 * @param       bufferSizePtr   指向缓冲区大小的指针
 * @return      缓冲区请求结果
 */
BufReq_ReturnType PduR_TpCopyRxData(PduIdType id, const PduInfoType* info, PduLengthType* bufferSizePtr);

/**
 * @brief       TP层复制发送数据（由下层调用）
 * @param       id          PDU ID
 * @param       info        指向PDU信息的指针
 * @param       retry       指向重试信息的指针
 * @param       availableDataPtr    指向可用数据大小的指针
 * @return      缓冲区请求结果
 */
BufReq_ReturnType PduR_TpCopyTxData(PduIdType id, const PduInfoType* info, const RetryInfoType* retry, PduLengthType* availableDataPtr);

/**
 * @brief       TP层开始接收（由下层调用）
 * @param       id          PDU ID
 * @param       info        指向PDU信息的指针
 * @param       TpSduLength TP SDU长度
 * @param       bufferSizePtr   指向缓冲区大小的指针
 * @return      缓冲区请求结果
 */
BufReq_ReturnType PduR_StartOfReception(PduIdType id, const PduInfoType* info, PduLengthType TpSduLength, PduLengthType* bufferSizePtr);

#endif /* PDUR_H */
