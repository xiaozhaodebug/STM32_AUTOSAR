/**
 * @file        EcuM.h
 * @brief       ECU管理模块头文件
 * @details     提供ECU启动、关闭和状态管理服务
 *              简化版AUTOSAR EcuM实现
 * 
 * @author      [小昭debug]
 * @date        2026-03-15
 */

#ifndef ECUM_H
#define ECUM_H

#include "Std_Types.h"

/*==================================================================================================
 *                                      版本信息
==================================================================================================*/
#define ECUM_VENDOR_ID              0x00
#define ECUM_MODULE_ID              0x08
#define ECUM_INSTANCE_ID            0x00

#define ECUM_SW_MAJOR_VERSION       1
#define ECUM_SW_MINOR_VERSION       0
#define ECUM_SW_PATCH_VERSION       0

/*==================================================================================================
 *                                      开发错误代码
==================================================================================================*/
#define ECUM_E_UNINIT                   0x16
#define ECUM_E_PARAM_POINTER            0x17
#define ECUM_E_PARAM_INVALID            0x18
#define ECUM_E_STATE_TRANSITION         0x19
#define ECUM_E_MULTIPLE_RUN_REQUESTS    0x1A
#define ECUM_E_MISMATCHED_RUN_RELEASE   0x1B
#define ECUM_E_INVALID_PAR              0x1C
#define ECUM_E_NULL_POINTER             0x1D
#define ECUM_E_ARC_RELEASE              0x1E
#define ECUM_E_ARC_REQUEST              0x1F

/*==================================================================================================
 *                                      运行时错误代码
==================================================================================================*/
#define ECUM_E_ARC_RELEASE_FAILED       0x01
#define ECUM_E_ARC_REQUEST_FAILED       0x02

/*==================================================================================================
 *                                      瞬态故障代码
==================================================================================================*/
#define ECUM_E_ARC_RELEASE_FAULT        0x01
#define ECUM_E_ARC_REQUEST_FAULT        0x02

/*==================================================================================================
 *                                      服务ID
==================================================================================================*/
#define ECUM_SID_INIT                   0x00
#define ECUM_SID_SHUTDOWN               0x01
#define ECUM_SID_REQUESTRUN             0x02
#define ECUM_SID_RELEASERUN             0x03
#define ECUM_SID_SELECTSHUTDOWNTARGET   0x06
#define ECUM_SID_GETSHUTDOWNTARGET      0x07
#define ECUM_SID_SELECTBOOTTARGET       0x08
#define ECUM_SID_GETBOOTTARGET          0x09
#define ECUM_SID_SELECTAPPLICATION      0x0B
#define ECUM_SID_GETAPPLICATION         0x0C
#define ECUM_SID_COMMCTRL               0x0D
#define ECUM_SID_GETSTATE               0x0E
#define ECUM_SID_SELECTSHUTDOWNCAUSE    0x0F
#define ECUM_SID_GETSHUTDOWNCAUSE       0x10
#define ECUM_SID_GETLASTSHUTDOWNCAUSE   0x11
#define ECUM_SID_GETNEXTRECENTWAKEUP    0x12
#define ECUM_SID_CLEARWAKEUPEVENTS      0x13
#define ECUM_SID_GETSTATUSOFWAKEUP      0x14
#define ECUM_SID_MAINFUNCTION           0x18
#define ECUM_SID_GOHALT                 0x19
#define ECUM_SID_GOPOLL                 0x1A
#define ECUM_SID_REQUESTPOSTRUN         0x1B
#define ECUM_SID_RELEASEPOSTRUN         0x1C
#define ECUM_SID_KILLALLRUNREQUESTS     0x1D
#define ECUM_SID_KILLALLPOSTRUNREQUESTS 0x1E
#define ECUM_SID_SETWAKEUPEVENT         0x23
#define ECUM_SID_AL_DRIVERINITZERO      0x2D
#define ECUM_SID_AL_DRIVERINITONE       0x2E
#define ECUM_SID_AL_DRIVERREINIT        0x2F
#define ECUM_SID_AL_SWITCHOFF           0x30
#define ECUM_SID_AL_PREOSCONFIG         0x31

/*==================================================================================================
 *                                      ECU状态类型
==================================================================================================*/
typedef uint8 EcuM_StateType;
#define ECUM_STATE_STARTUP              (EcuM_StateType)0x10
#define ECUM_STATE_RUN                  (EcuM_StateType)0x30
#define ECUM_STATE_POST_RUN             (EcuM_StateType)0x40
#define ECUM_STATE_SHUTDOWN             (EcuM_StateType)0x50
#define ECUM_STATE_SLEEP                (EcuM_StateType)0x60
#define ECUM_STATE_OFF                  (EcuM_StateType)0x70

/* 子状态 */
#define ECUM_SUBSTATE_MASK              (EcuM_StateType)0x0F

/* STARTUP子状态 */
#define ECUM_STATE_STARTUP_ONE          (EcuM_StateType)0x11
#define ECUM_STATE_STARTUP_TWO          (EcuM_StateType)0x12

/* SHUTDOWN子状态 */
#define ECUM_STATE_SHUTDOWN_ONE         (EcuM_StateType)0x51
#define ECUM_STATE_SHUTDOWN_TWO         (EcuM_StateType)0x52

/*==================================================================================================
 *                                      关闭目标类型
==================================================================================================*/
typedef uint8 EcuM_ShutdownTargetType;
#define ECUM_SHUTDOWN_TARGET_SLEEP      (EcuM_ShutdownTargetType)0x00
#define ECUM_SHUTDOWN_TARGET_RESET      (EcuM_ShutdownTargetType)0x01
#define ECUM_SHUTDOWN_TARGET_OFF        (EcuM_ShutdownTargetType)0x02

/*==================================================================================================
 *                                      唤醒源类型
==================================================================================================*/
typedef uint32 EcuM_WakeupSourceType;
#define ECUM_WKSOURCE_POWER             (EcuM_WakeupSourceType)0x00000001
#define ECUM_WKSOURCE_RESET             (EcuM_WakeupSourceType)0x00000002
#define ECUM_WKSOURCE_INTERNAL_RESET    (EcuM_WakeupSourceType)0x00000004
#define ECUM_WKSOURCE_INTERNAL_WDG      (EcuM_WakeupSourceType)0x00000008
#define ECUM_WKSOURCE_EXTERNAL_WDG      (EcuM_WakeupSourceType)0x00000010
#define ECUM_WKSOURCE_CAN               (EcuM_WakeupSourceType)0x00000020
#define ECUM_WKSOURCE_LIN               (EcuM_WakeupSourceType)0x00000040
#define ECUM_WKSOURCE_FLEXRAY           (EcuM_WakeupSourceType)0x00000080
#define ECUM_WKSOURCE_ETHERNET          (EcuM_WakeupSourceType)0x00000100

/*==================================================================================================
 *                                      启动原因类型
==================================================================================================*/
typedef uint8 EcuM_ShutdownCauseType;

/*==================================================================================================
 *                                      唤醒状态类型
==================================================================================================*/
typedef uint8 EcuM_WakeupStatusType;
#define ECUM_WKS_CLEAR      (EcuM_WakeupStatusType)0x00
#define ECUM_WKS_PENDING    (EcuM_WakeupStatusType)0x01
#define ECUM_WKS_VALIDATED  (EcuM_WakeupStatusType)0x02
#define ECUM_WKS_EXPIRED    (EcuM_WakeupStatusType)0x03

/*==================================================================================================
 *                                      复位类型
==================================================================================================*/
typedef uint8 EcuM_ResetType;
#define ECUM_RESET_MCU      (EcuM_ResetType)0x00
#define ECUM_RESET_WDG      (EcuM_ResetType)0x01
#define ECUM_RESET_IO       (EcuM_ResetType)0x02
#define ECUM_CAUSE_ECU_STATE            (EcuM_ShutdownCauseType)0x00
#define ECUM_CAUSE_WDG                  (EcuM_ShutdownCauseType)0x01
#define ECUM_CAUSE_DCM                  (EcuM_ShutdownCauseType)0x02
#define ECUM_CAUSE_EOL                  (EcuM_ShutdownCauseType)0x03

/*==================================================================================================
 *                                      用户ID类型
==================================================================================================*/
typedef uint8 EcuM_UserType;
#define ECUM_USER_NONE                  (EcuM_UserType)0x00
#define ECUM_USER_DCM                   (EcuM_UserType)0x01
#define ECUM_USER_COMM                  (EcuM_UserType)0x02
#define ECUM_USER_BswM                  (EcuM_UserType)0x03
#define ECUM_USER_ECUM                  (EcuM_UserType)0x04

/*==================================================================================================
 *                                      配置类型
==================================================================================================*/
typedef struct {
    uint32                      DefaultShutdownTarget;  /**< 默认关闭目标 */
    uint32                      DefaultBootTarget;      /**< 默认启动目标 */
    uint32                      NormalMcuMode;          /**< 正常MCU模式 */
    uint32                      SleepMcuMode;           /**< 睡眠MCU模式 */
} EcuM_ConfigType;

/*==================================================================================================
 *                                      函数声明
==================================================================================================*/

/**
 * @brief       初始化EcuM
 * @param       ConfigPtr   指向配置结构的指针
 */
void EcuM_Init(const EcuM_ConfigType* ConfigPtr);

/**
 * @brief       启动第二步（OS启动后调用）
 */
void EcuM_StartupTwo(void);

/**
 * @brief       主函数
 */
void EcuM_MainFunction(void);

/**
 * @brief       请求RUN状态
 * @param       user    用户ID
 * @return      E_OK: 成功, E_NOT_OK: 失败
 */
Std_ReturnType EcuM_RequestRUN(EcuM_UserType user);

/**
 * @brief       释放RUN状态
 * @param       user    用户ID
 * @return      E_OK: 成功, E_NOT_OK: 失败
 */
Std_ReturnType EcuM_ReleaseRUN(EcuM_UserType user);

/**
 * @brief       请求POST_RUN状态
 * @param       user    用户ID
 * @return      E_OK: 成功, E_NOT_OK: 失败
 */
Std_ReturnType EcuM_RequestPOST_RUN(EcuM_UserType user);

/**
 * @brief       释放POST_RUN状态
 * @param       user    用户ID
 * @return      E_OK: 成功, E_NOT_OK: 失败
 */
Std_ReturnType EcuM_ReleasePOST_RUN(EcuM_UserType user);

/**
 * @brief       选择关闭目标
 * @param       target      关闭目标
 * @param       mode        模式
 * @return      E_OK: 成功, E_NOT_OK: 失败
 */
Std_ReturnType EcuM_SelectShutdownTarget(EcuM_ShutdownTargetType target, uint32 mode);

/**
 * @brief       获取关闭目标
 * @param       shutdownTarget  指向关闭目标的指针
 * @param       mode            指向模式的指针
 * @return      E_OK: 成功, E_NOT_OK: 失败
 */
Std_ReturnType EcuM_GetShutdownTarget(EcuM_ShutdownTargetType* shutdownTarget, uint32* mode);

/**
 * @brief       选择关闭原因
 * @param       target      关闭目标
 * @param       cause       关闭原因
 * @return      E_OK: 成功, E_NOT_OK: 失败
 */
Std_ReturnType EcuM_SelectShutdownCause(EcuM_ShutdownTargetType target, EcuM_ShutdownCauseType cause);

/**
 * @brief       获取当前状态
 * @param       state   指向状态的指针
 * @return      E_OK: 成功, E_NOT_OK: 失败
 */
Std_ReturnType EcuM_GetState(EcuM_StateType* state);

/**
 * @brief       设置唤醒事件
 * @param       sources     唤醒源
 */
void EcuM_SetWakeupEvent(EcuM_WakeupSourceType sources);

/**
 * @brief       清除唤醒事件
 * @param       sources     唤醒源
 */
void EcuM_ClearWakeupEvent(EcuM_WakeupSourceType sources);

/**
 * @brief       获取唤醒事件状态
 * @param       sources     唤醒源
 * @return      唤醒状态
 */
EcuM_WakeupStatusType EcuM_GetStatusOfWakeupSource(EcuM_WakeupSourceType sources);

/**
 * @brief       获取版本信息
 * @param       versioninfo 指向版本信息结构的指针
 */
void EcuM_GetVersionInfo(Std_VersionInfoType* versioninfo);

/**
 * @brief       进入HALT模式
 */
void EcuM_GoHalt(void);

/**
 * @brief       进入POLL模式
 */
void EcuM_GoPoll(void);

/* 回调函数声明（由应用实现） */
extern void EcuM_DriverInitZero(void);
extern void EcuM_DriverInitOne(void);
extern void EcuM_AL_DriverInitOne(void);
extern void EcuM_AL_DriverInitTwo(void);
extern void EcuM_AL_DriverInitThree(void);
extern void EcuM_AL_DriverRestart(void);
extern void EcuM_AL_SwitchOff(void);
extern void EcuM_AL_Reset(EcuM_ResetType resetType);

#endif /* ECUM_H */
