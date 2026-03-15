/**
 * @file        AUTOSAR_Cfg.h
 * @brief       AUTOSAR配置头文件
 * @details     STM32F407工程的AUTOSAR模块配置
 * 
 * @author      [小昭debug]
 * @date        2026-03-15
 */

#ifndef AUTOSAR_CFG_H
#define AUTOSAR_CFG_H

#include "Std_Types.h"
#include "ComStack_Types.h"
#include "CanIf.h"
#include "PduR.h"
#include "Com.h"
#include "Dcm.h"
#include "EcuM.h"
#include "UdsConfig_Generated.h"  /* UDS配置（代码生成器生成） */

/*==================================================================================================
 *                                      回调函数声明
==================================================================================================*/
/* CanIf回调函数（在AUTOSAR_Integration.c中实现） */
extern void CanIf_Callback_RxIndication(PduIdType RxPduId, const PduInfoType* PduInfoPtr);
extern void CanIf_Callback_TxConfirmation(PduIdType TxPduId);

/*==================================================================================================
 *                                      CANIF配置
==================================================================================================*/
/* CANIF控制器配置 */
static const CanIf_ControllerConfigType CanIf_ControllerConfig[1] = {
    {
        .ControllerId = 0,
        .BaudRate = 500000,
        .PropSeg = 1,
        .PhaseSeg1 = 11,
        .PhaseSeg2 = 2,
        .SyncJumpWidth = 1
    }
};

/* CANIF发送PDU配置 - 适配DBC矩阵
 * PDU ID分配：
 *   0-9:   COM层应用消息
 *   10-19: DCM层诊断消息
 */
static const CanIf_TxPduConfigType CanIf_TxPduConfig[] = {
    {
        .CanIfTxPduId = 0,
        .CanPduId = 0,
        .CanId = 0x123,
        .CanIdType = FALSE,     /* 标准帧 */
        .Dlc = 8,
        .ControllerId = 0
    },
    {
        .CanIfTxPduId = 10,
        .CanPduId = 10,
        .CanId = 0x73D,         /* Meg_ResDiag - 诊断响应ID (DBC) */
        .CanIdType = FALSE,
        .Dlc = 8,
        .ControllerId = 0
    }
};

/* CANIF接收PDU配置 - 适配DBC矩阵
 * PDU ID分配：
 *   0-9:   COM层应用消息
 *   10-19: DCM层诊断消息
 */
static const CanIf_RxPduConfigType CanIf_RxPduConfig[] = {
    {
        .CanIfRxPduId = 0,
        .CanId = 0x123,
        .CanIdMask = 0x7FF,
        .CanIdType = FALSE,
        .Dlc = 8,
        .ControllerId = 0,
        .UpperPduId = 0
    },
    {
        .CanIfRxPduId = 10,
        .CanId = 0x7DF,         /* Meg_FunDiag - 功能诊断请求ID (DBC) */
        .CanIdMask = 0x7FF,
        .CanIdType = FALSE,
        .Dlc = 8,
        .ControllerId = 0,
        .UpperPduId = 10
    },
    {
        .CanIfRxPduId = 11,
        .CanId = 0x735,         /* Meg_PhyDiag - 物理诊断请求ID (DBC) */
        .CanIdMask = 0x7FF,
        .CanIdType = FALSE,
        .Dlc = 8,
        .ControllerId = 0,
        .UpperPduId = 11
    }
};

/* CANIF配置 */
static const CanIf_ConfigType CanIf_Config = {
    .NumControllers = 1,
    .ControllerConfig = CanIf_ControllerConfig,
    .NumTxPdus = sizeof(CanIf_TxPduConfig) / sizeof(CanIf_TxPduConfig[0]),
    .TxPduConfig = CanIf_TxPduConfig,
    .NumRxPdus = sizeof(CanIf_RxPduConfig) / sizeof(CanIf_RxPduConfig[0]),
    .RxPduConfig = CanIf_RxPduConfig
};

/*==================================================================================================
 *                                      PDUR配置
==================================================================================================*/
/* PDUR路由路径配置 - 适配DBC矩阵
 * 路由ID分配：
 *   0-9:   COM层应用消息
 *   10-19: DCM层诊断消息 (接收)
 *   20-29: DCM层诊断消息 (发送)
 */
static const PduR_RoutingPathConfigType PduR_RoutingPaths[] = {
    /* COM到CANIF */
    {
        .SrcPduId = 0,
        .DestPduId = 0,
        .DestModule = PDUR_DEST_MODULE_CANIF,
        .PathType = PDUR_ROUTINGPATH_IF
    },
    /* CANIF到COM */
    {
        .SrcPduId = 0,
        .DestPduId = 0,
        .DestModule = PDUR_DEST_MODULE_COM,
        .PathType = PDUR_ROUTINGPATH_IF
    },
    /* CANIF到DCM - 功能诊断请求 0x7DF (Meg_FunDiag) */
    {
        .SrcPduId = 10,
        .DestPduId = 10,
        .DestModule = PDUR_DEST_MODULE_DCM,
        .PathType = PDUR_ROUTINGPATH_IF
    },
    /* CANIF到DCM - 物理诊断请求 0x735 (Meg_PhyDiag) */
    {
        .SrcPduId = 11,
        .DestPduId = 11,
        .DestModule = PDUR_DEST_MODULE_DCM,
        .PathType = PDUR_ROUTINGPATH_IF
    },
    /* DCM到CANIF - 诊断响应 0x73D (Meg_ResDiag) */
    {
        .SrcPduId = 20,
        .DestPduId = 10,
        .DestModule = PDUR_DEST_MODULE_CANIF,
        .PathType = PDUR_ROUTINGPATH_IF
    }
};

/* PDUR配置 */
static const PduR_ConfigType PduR_Config = {
    .NumRoutingPaths = sizeof(PduR_RoutingPaths) / sizeof(PduR_RoutingPaths[0]),
    .RoutingPaths = PduR_RoutingPaths
};

/*==================================================================================================
 *                                      COM配置
==================================================================================================*/
/* COM信号配置 */
static const Com_SignalConfigType Com_SignalConfig[] = {
    {
        .SignalId = 0,
        .SignalType = COM_UINT8_TYPE,
        .Endianness = COM_LITTLE_ENDIAN,
        .BitPosition = 0,
        .BitSize = 8,
        .InitialValue = 0,
        .IpduId = 0,
        .TransferProperty = COM_TRIGGERED_ON_CHANGE
    },
    {
        .SignalId = 1,
        .SignalType = COM_UINT16_TYPE,
        .Endianness = COM_LITTLE_ENDIAN,
        .BitPosition = 8,
        .BitSize = 16,
        .InitialValue = 0,
        .IpduId = 0,
        .TransferProperty = COM_TRIGGERED_ON_CHANGE
    },
    {
        .SignalId = 2,
        .SignalType = COM_UINT32_TYPE,
        .Endianness = COM_LITTLE_ENDIAN,
        .BitPosition = 24,
        .BitSize = 32,
        .InitialValue = 0,
        .IpduId = 0,
        .TransferProperty = COM_TRIGGERED_ON_CHANGE
    }
};

/* COM I-PDU配置 */
static const Com_IpduConfigType Com_IpduConfig[] = {
    {
        .PduId = 0,
        .PduLength = 8,
        .Period = 100,      /* 100ms周期 */
        .IsTx = TRUE
    },
    {
        .PduId = 1,
        .PduLength = 8,
        .Period = 0,        /* 非周期 */
        .IsTx = FALSE
    }
};

/* COM配置 */
static const Com_ConfigType Com_Config = {
    .NumSignals = sizeof(Com_SignalConfig) / sizeof(Com_SignalConfig[0]),
    .SignalConfig = Com_SignalConfig,
    .NumIpdu = sizeof(Com_IpduConfig) / sizeof(Com_IpduConfig[0]),
    .IpduConfig = Com_IpduConfig
};

/*==================================================================================================
 *                                      DCM配置
==================================================================================================*/
/* DCM DID数据存储 */
static uint8 Dcm_DidData_VIN[17] = {0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 
                                     0x39, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48};
static uint8 Dcm_DidData_ECUName[16] = {"STM32F407_CAN"};
static uint8 Dcm_DidData_SystemStatus[4] = {0x00, 0x00, 0x00, 0x00};
static uint8 Dcm_DidData_VehicleSpeed[2] = {0x00, 0x00};
static uint8 Dcm_DidData_EngineTemp[1] = {0x50};  /* 80度 */

/* DCM DID表 */
static Dcm_DidInfoType Dcm_DidTable[] = {
    {
        .Did = 0xF190,      /* VIN */
        .Data = Dcm_DidData_VIN,
        .DataLength = 17,
        .IsReadOnly = TRUE
    },
    {
        .Did = 0xF197,      /* ECU名称 */
        .Data = Dcm_DidData_ECUName,
        .DataLength = 16,
        .IsReadOnly = TRUE
    },
    {
        .Did = 0xF100,      /* 系统状态 */
        .Data = Dcm_DidData_SystemStatus,
        .DataLength = 4,
        .IsReadOnly = FALSE
    },
    {
        .Did = 0x0100,      /* 车速 */
        .Data = Dcm_DidData_VehicleSpeed,
        .DataLength = 2,
        .IsReadOnly = FALSE
    },
    {
        .Did = 0x0105,      /* 发动机温度 */
        .Data = Dcm_DidData_EngineTemp,
        .DataLength = 1,
        .IsReadOnly = FALSE
    }
};

/* DCM DTC表 */
static Dcm_DtcInfoType Dcm_DtcTable[] = {
    {
        .Dtc = 0x0100,     /* 空气质量流量计电路故障 */
        .Status = 0x00,
        .Snapshot = {0},
        .ExtData = {0}
    },
    {
        .Dtc = 0x0101,     /* 空气质量流量计性能 */
        .Status = 0x00,
        .Snapshot = {0},
        .ExtData = {0}
    },
    {
        .Dtc = 0x0300,     /* 随机/多缸失火 */
        .Status = 0x00,
        .Snapshot = {0},
        .ExtData = {0}
    },
    {
        .Dtc = 0x1601,     /* U1601: 与ECM通信丢失 */
        .Status = 0x00,
        .Snapshot = {0},
        .ExtData = {0}
    }
};

/* 自定义DID读写函数 */
static Std_ReturnType Dcm_ReadDid_Custom(Dcm_DidType Did, uint8* Data, uint16* DataLen)
{
    (void)Did;
    (void)Data;
    (void)DataLen;
    return E_NOT_OK;
}

static Std_ReturnType Dcm_WriteDid_Custom(Dcm_DidType Did, const uint8* Data, uint16 DataLen)
{
    (void)Did;
    (void)Data;
    (void)DataLen;
    return E_NOT_OK;
}

/* DCM配置 - 适配DBC矩阵：RX 0x7DF/0x735, TX 0x73D
 * DCM使用PDU ID 10,11接收，PDU ID 20发送
 * 
 * 使用代码生成器生成的UDS配置：
 * - 外部DID表：Uds_DidTable (在UdsConfig_Generated.c中定义)
 * - 外部DTC表：Uds_DtcTable (在UdsConfig_Generated.c中定义)
 * - 读写函数：UdsConfig_ReadDid / UdsConfig_WriteDid
 */
extern const Dcm_DidInfoType Uds_DidTable[];
extern const Dcm_DtcInfoType Uds_DtcTable[];
extern const uint8 UDS_DID_COUNT;
extern const uint8 UDS_DTC_COUNT;

/* 包装函数：适配DCM接口到生成器接口 */
static Std_ReturnType Dcm_ReadDid_Wrapper(Dcm_DidType Did, uint8* Data, uint16* DataLen)
{
    return UdsConfig_ReadDid((uint16)Did, Data, DataLen);
}

static Std_ReturnType Dcm_WriteDid_Wrapper(Dcm_DidType Did, const uint8* Data, uint16 DataLen)
{
    return UdsConfig_WriteDid((uint16)Did, Data, DataLen);
}

/* DCM配置 */
static const Dcm_ConfigType Dcm_Config = {
    .NumDids = 8,       /* 使用生成的DID数量 */
    .DidTable = NULL_PTR,  /* 使用外部Uds_DidTable */
    .NumDtcs = 5,       /* 使用生成的DTC数量 */
    .DtcTable = NULL_PTR,  /* 使用外部Uds_DtcTable */
    .ReadDidFunc = Dcm_ReadDid_Wrapper,
    .WriteDidFunc = Dcm_WriteDid_Wrapper,
    .RoutineFunc = NULL_PTR,
    .RxPduId = 10,      /* DCM接收PDU ID - 对应 0x7DF (Meg_FunDiag) */
    .TxPduId = 20       /* DCM发送PDU ID - 对应 0x73D (Meg_ResDiag) */
};

/*==================================================================================================
 *                                      ECUM配置
==================================================================================================*/
/* ECUM配置 */
static const EcuM_ConfigType EcuM_Config = {
    .DefaultShutdownTarget = ECUM_SHUTDOWN_TARGET_OFF,
    .DefaultBootTarget = 0,
    .NormalMcuMode = 0,
    .SleepMcuMode = 0
};

#endif /* AUTOSAR_CFG_H */
