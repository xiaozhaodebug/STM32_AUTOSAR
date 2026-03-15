/**
 * @file        CanIf.h
 * @brief       CAN接口模块头文件
 * @details     提供CAN硬件抽象接口，管理PDU收发和控制器模式
 *              简化版AUTOSAR CanIf实现
 * 
 * @author      [小昭debug]
 * @date        2026-03-15
 */

#ifndef CANIF_H
#define CANIF_H

#include "Std_Types.h"
#include "ComStack_Types.h"

/*==================================================================================================
 *                                      版本信息
==================================================================================================*/
#define CANIF_VENDOR_ID             0x00    /**< 供应商ID */
#define CANIF_MODULE_ID             0x3C    /**< 模块ID */
#define CANIF_INSTANCE_ID           0x00    /**< 实例ID */

#define CANIF_SW_MAJOR_VERSION      1       /**< 软件主版本 */
#define CANIF_SW_MINOR_VERSION      0       /**< 软件次版本 */
#define CANIF_SW_PATCH_VERSION      0       /**< 软件补丁版本 */

/*==================================================================================================
 *                                      开发错误代码
==================================================================================================*/
#define CANIF_E_PARAM_CANID         0x01    /**< 无效的CAN ID */
#define CANIF_E_PARAM_DLC           0x02    /**< 无效的DLC */
#define CANIF_E_PARAM_CONTROLLER    0x03    /**< 无效的控制器 */
#define CANIF_E_PARAM_WAKEUPSOURCE  0x04    /**< 无效的唤醒源 */
#define CANIF_E_PARAM_TRCV          0x05    /**< 无效的收发器 */
#define CANIF_E_PARAM_TRCVMODE      0x06    /**< 无效的收发器模式 */
#define CANIF_E_PARAM_TRCVWAKEUPMODE    0x07    /**< 无效的收发器唤醒模式 */
#define CANIF_E_PARAM_POINTER       0x08    /**< 空指针 */
#define CANIF_E_PARAM_CONTROLLERMODE    0x09    /**< 无效的控制器模式 */
#define CANIF_E_PARAM_PDUMODE       0x0A    /**< 无效的PDU模式 */
#define CANIF_E_UNINIT              0x10    /**< 未初始化 */
#define CANIF_E_ALREADY_INITIALIZED 0x11    /**< 已初始化 */
#define CANIF_E_INVALID_RXPDUID     0x12    /**< 无效的接收PDU ID */
#define CANIF_E_INVALID_TXPDUID     0x13    /**< 无效的发送PDU ID */

/*==================================================================================================
 *                                      运行时错误代码
==================================================================================================*/
#define CANIF_E_STOPPED             0x20    /**< CAN控制器已停止 */
#define CANIF_E_FULL_TX_BUFFER      0x21    /**< 发送缓冲区满 */

/*==================================================================================================
 *                                      瞬态故障代码
==================================================================================================*/
#define CANIF_E_TX_BUSY             0x30    /**< 发送忙 */

/*==================================================================================================
 *                                      服务ID
==================================================================================================*/
#define CANIF_SID_INIT                  0x01
#define CANIF_SID_DEINIT                0x02
#define CANIF_SID_SET_CONTROLLER_MODE   0x03
#define CANIF_SID_GET_CONTROLLER_MODE   0x04
#define CANIF_SID_TRANSMIT              0x05
#define CANIF_SID_READ_RX_PDU_DATA      0x06
#define CANIF_SID_READ_RX_NOTIF_STATUS  0x07
#define CANIF_SID_READ_TX_NOTIF_STATUS  0x08
#define CANIF_SID_SET_PDU_MODE          0x09
#define CANIF_SID_GET_PDU_MODE          0x0A
#define CANIF_SID_CONTROLLER_BUSOFF     0x0B
#define CANIF_SID_CHECK_WAKEUP          0x11

/*==================================================================================================
 *                                      控制器模式类型
==================================================================================================*/
typedef uint8 CanIf_ControllerModeType;
#define CANIF_CS_UNINIT         (CanIf_ControllerModeType)0x00   /**< 未初始化 */
#define CANIF_CS_SLEEP          (CanIf_ControllerModeType)0x01   /**< 睡眠模式 */
#define CANIF_CS_STARTED        (CanIf_ControllerModeType)0x02   /**< 已启动 */
#define CANIF_CS_STOPPED        (CanIf_ControllerModeType)0x03   /**< 已停止 */

/*==================================================================================================
 *                                      PDU模式类型
==================================================================================================*/
typedef uint8 CanIf_PduModeType;
#define CANIF_OFFLINE           (CanIf_PduModeType)0x00   /**< 离线 */
#define CANIF_TX_OFFLINE        (CanIf_PduModeType)0x01   /**< 发送离线 */
#define CANIF_TX_OFFLINE_ACTIVE (CanIf_PduModeType)0x02   /**< 发送离线主动 */
#define CANIF_ONLINE            (CanIf_PduModeType)0x03   /**< 在线 */

/*==================================================================================================
 *                                      通知状态类型
==================================================================================================*/
typedef uint8 CanIf_NotifStatusType;
#define CANIF_NO_NOTIFICATION   (CanIf_NotifStatusType)0x00   /**< 无通知 */
#define CANIF_TX_RX_NOTIFICATION    (CanIf_NotifStatusType)0x01   /**< 收发通知 */

/*==================================================================================================
 *                                      控制器配置类型
==================================================================================================*/
typedef struct {
    uint8   ControllerId;           /**< CAN控制器ID */
    uint32  BaudRate;               /**< 波特率 */
    uint8   PropSeg;                /**< 传播段 */
    uint8   PhaseSeg1;              /**< 相位段1 */
    uint8   PhaseSeg2;              /**< 相位段2 */
    uint8   SyncJumpWidth;          /**< 同步跳转宽度 */
} CanIf_ControllerConfigType;

/*==================================================================================================
 *                                      PDU配置类型
==================================================================================================*/
typedef struct {
    PduIdType       CanIfTxPduId;       /**< CANIF PDU ID */
    PduIdType       CanPduId;           /**< CAN PDU ID (硬件ID) */
    uint32          CanId;              /**< CAN ID */
    bool            CanIdType;          /**< 0=标准帧, 1=扩展帧 */
    uint8           Dlc;                /**< 数据长度 */
    uint8           ControllerId;       /**< 所属控制器ID */
} CanIf_TxPduConfigType;

typedef struct {
    PduIdType       CanIfRxPduId;       /**< CANIF PDU ID */
    uint32          CanId;              /**< CAN ID */
    uint32          CanIdMask;          /**< CAN ID掩码 */
    bool            CanIdType;          /**< 0=标准帧, 1=扩展帧 */
    uint8           Dlc;                /**< 数据长度 */
    uint8           ControllerId;       /**< 所属控制器ID */
    PduIdType       UpperPduId;         /**< 上层PDU ID */
} CanIf_RxPduConfigType;

/*==================================================================================================
 *                                      配置类型
==================================================================================================*/
typedef struct {
    uint8                           NumControllers;     /**< 控制器数量 */
    const CanIf_ControllerConfigType*   ControllerConfig;   /**< 控制器配置 */
    uint16                          NumTxPdus;          /**< 发送PDU数量 */
    const CanIf_TxPduConfigType*    TxPduConfig;        /**< 发送PDU配置 */
    uint16                          NumRxPdus;          /**< 接收PDU数量 */
    const CanIf_RxPduConfigType*    RxPduConfig;        /**< 接收PDU配置 */
} CanIf_ConfigType;

/*==================================================================================================
 *                                      回调函数类型
==================================================================================================*/
typedef void (*CanIf_RxIndicationType)(PduIdType RxPduId, const PduInfoType* PduInfoPtr);
typedef void (*CanIf_TxConfirmationType)(PduIdType TxPduId);
typedef void (*CanIf_ControllerBusOffType)(uint8 ControllerId);
typedef void (*CanIf_ControllerModeIndicationType)(uint8 ControllerId, CanIf_ControllerModeType ControllerMode);

/*==================================================================================================
 *                                      回调函数配置
==================================================================================================*/
typedef struct {
    CanIf_RxIndicationType          RxIndication;       /**< 接收指示回调 */
    CanIf_TxConfirmationType        TxConfirmation;     /**< 发送确认回调 */
    CanIf_ControllerBusOffType      ControllerBusOff;   /**< 总线关闭回调 */
    CanIf_ControllerModeIndicationType  ControllerModeIndication;   /**< 模式指示回调 */
} CanIf_CallbackType;

/*==================================================================================================
 *                                      全局变量声明
==================================================================================================*/
extern const CanIf_CallbackType CanIf_Callbacks;

/*==================================================================================================
 *                                      函数声明
==================================================================================================*/

/**
 * @brief       初始化CanIf模块
 * @param       ConfigPtr   指向配置结构的指针
 */
void CanIf_Init(const CanIf_ConfigType* ConfigPtr);

/**
 * @brief       反初始化CanIf模块
 */
void CanIf_DeInit(void);

/**
 * @brief       设置CAN控制器模式
 * @param       Controller      控制器ID
 * @param       ControllerMode  目标模式
 * @return      E_OK: 成功, E_NOT_OK: 失败
 */
Std_ReturnType CanIf_SetControllerMode(uint8 Controller, CanIf_ControllerModeType ControllerMode);

/**
 * @brief       获取CAN控制器模式
 * @param       Controller          控制器ID
 * @param       ControllerModePtr   指向存储模式的指针
 * @return      E_OK: 成功, E_NOT_OK: 失败
 */
Std_ReturnType CanIf_GetControllerMode(uint8 Controller, CanIf_ControllerModeType* ControllerModePtr);

/**
 * @brief       发送PDU
 * @param       TxPduId     发送PDU ID
 * @param       PduInfoPtr  指向PDU信息的指针
 * @return      E_OK: 成功, E_NOT_OK: 失败
 */
Std_ReturnType CanIf_Transmit(PduIdType TxPduId, const PduInfoType* PduInfoPtr);

/**
 * @brief       设置PDU模式
 * @param       Controller  控制器ID
 * @param       PduModeRequest  PDU模式请求
 * @return      E_OK: 成功, E_NOT_OK: 失败
 */
Std_ReturnType CanIf_SetPduMode(uint8 Controller, CanIf_PduModeType PduModeRequest);

/**
 * @brief       获取PDU模式
 * @param       Controller      控制器ID
 * @param       PduModePtr      指向存储模式的指针
 * @return      E_OK: 成功, E_NOT_OK: 失败
 */
Std_ReturnType CanIf_GetPduMode(uint8 Controller, CanIf_PduModeType* PduModePtr);

/**
 * @brief       获取版本信息
 * @param       VersionInfo 指向版本信息结构的指针
 */
void CanIf_GetVersionInfo(Std_VersionInfoType* VersionInfo);

/**
 * @brief       CAN驱动接收指示回调（由CAN驱动调用）
 * @param       Hrh         硬件接收句柄
 * @param       CanId       CAN ID
 * @param       CanDlc      数据长度
 * @param       CanSduPtr   指向SDU数据的指针
 */
void CanIf_RxIndication(uint8 Hrh, uint32 CanId, uint8 CanDlc, const uint8* CanSduPtr);

/**
 * @brief       CAN驱动发送确认回调（由CAN驱动调用）
 * @param       Hth         硬件发送句柄
 */
void CanIf_TxConfirmation(uint8 Hth);

/**
 * @brief       控制器总线关闭回调（由CAN驱动调用）
 * @param       Controller  控制器ID
 */
void CanIf_ControllerBusOff(uint8 Controller);

/**
 * @brief       检查唤醒事件
 * @param       WakeupSource    唤醒源
 * @return      E_OK: 检测到唤醒, E_NOT_OK: 未检测到
 */
Std_ReturnType CanIf_CheckWakeup(uint32 WakeupSource);

#endif /* CANIF_H */
