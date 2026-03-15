/**
 * @file        EcuM.c
 * @brief       ECU管理模块实现
 * @details     简化版AUTOSAR EcuM实现
 * 
 * @author      [小昭debug]
 * @date        2026-03-15
 */

#include "EcuM.h"
#include "CanIf.h"
#include "PduR.h"
#include "Com.h"
#include "Dcm.h"

/*==================================================================================================
 *                                      宏定义
==================================================================================================*/
#define ECUM_MAX_USERS          16      /**< 最大用户数 */
#define ECUM_RUN_TIMEOUT        5000    /**< RUN超时时间 */

/*==================================================================================================
 *                                      内部类型
==================================================================================================*/
typedef struct {
    bool                    Initialized;
    EcuM_StateType          State;
    EcuM_ShutdownTargetType ShutdownTarget;
    uint32                  ShutdownMode;
    EcuM_ShutdownCauseType  ShutdownCause;
    EcuM_WakeupSourceType   WakeupSource;
    const EcuM_ConfigType*  ConfigPtr;
    uint32                  RunCounter;
    uint32                  PostRunCounter;
    uint32                  RunTimer;
} EcuM_GlobalType;

/*==================================================================================================
 *                                      全局变量
==================================================================================================*/
static EcuM_GlobalType EcuM_Global = {
    .Initialized = FALSE,
    .State = ECUM_STATE_STARTUP,
    .ShutdownTarget = ECUM_SHUTDOWN_TARGET_OFF,
    .ShutdownMode = 0,
    .ShutdownCause = ECUM_CAUSE_ECU_STATE,
    .WakeupSource = ECUM_WKSOURCE_POWER,
    .ConfigPtr = NULL_PTR,
    .RunCounter = 0,
    .PostRunCounter = 0,
    .RunTimer = 0
};

static bool EcuM_Users[ECUM_MAX_USERS] = {FALSE};

/*==================================================================================================
 *                                      私有函数
==================================================================================================*/

static void EcuM_ProcessStartup(void)
{
    switch (EcuM_Global.State) {
        case ECUM_STATE_STARTUP:
            /* 初始化阶段1 */
            EcuM_DriverInitZero();
            EcuM_Global.State = ECUM_STATE_STARTUP_ONE;
            break;
            
        case ECUM_STATE_STARTUP_ONE:
            /* 初始化阶段2 */
            EcuM_AL_DriverInitOne();
            EcuM_Global.State = ECUM_STATE_STARTUP_TWO;
            break;
            
        default:
            break;
    }
}

static void EcuM_ProcessRun(void)
{
    if (EcuM_Global.RunCounter == 0) {
        EcuM_Global.RunTimer++;
        
        if (EcuM_Global.RunTimer >= ECUM_RUN_TIMEOUT) {
            /* 超时，进入POST_RUN */
            EcuM_Global.State = ECUM_STATE_POST_RUN;
        }
    } else {
        EcuM_Global.RunTimer = 0;
    }
}

static void EcuM_ProcessPostRun(void)
{
    if (EcuM_Global.PostRunCounter == 0) {
        /* 进入关闭流程 */
        EcuM_Global.State = ECUM_STATE_SHUTDOWN;
    }
}

static void EcuM_ProcessShutdown(void)
{
    switch (EcuM_Global.State) {
        case ECUM_STATE_SHUTDOWN:
            /* 关闭阶段1 */
            EcuM_AL_DriverInitTwo();
            EcuM_Global.State = ECUM_STATE_SHUTDOWN_ONE;
            break;
            
        case ECUM_STATE_SHUTDOWN_ONE:
            /* 关闭阶段2 */
            EcuM_AL_SwitchOff();
            EcuM_Global.State = ECUM_STATE_SHUTDOWN_TWO;
            break;
            
        default:
            break;
    }
}

/*==================================================================================================
 *                                      公共函数
==================================================================================================*/

void EcuM_Init(const EcuM_ConfigType* ConfigPtr)
{
    if (ConfigPtr == NULL_PTR) {
        return;
    }
    
    if (EcuM_Global.Initialized) {
        return;
    }
    
    EcuM_Global.ConfigPtr = ConfigPtr;
    EcuM_Global.ShutdownTarget = (EcuM_ShutdownTargetType)ConfigPtr->DefaultShutdownTarget;
    EcuM_Global.ShutdownMode = ConfigPtr->NormalMcuMode;
    
    /* 初始化阶段 */
    EcuM_ProcessStartup();
    
    EcuM_Global.Initialized = TRUE;
}

void EcuM_StartupTwo(void)
{
    /* OS启动后调用 */
    if (!EcuM_Global.Initialized) {
        return;
    }
    
    /* 初始化BSW模块 */
    EcuM_AL_DriverInitTwo();
    
    /* 进入RUN状态 */
    EcuM_Global.State = ECUM_STATE_RUN;
}

void EcuM_MainFunction(void)
{
    if (!EcuM_Global.Initialized) {
        return;
    }
    
    switch (EcuM_Global.State) {
        case ECUM_STATE_STARTUP:
        case ECUM_STATE_STARTUP_ONE:
        case ECUM_STATE_STARTUP_TWO:
            EcuM_ProcessStartup();
            break;
            
        case ECUM_STATE_RUN:
            EcuM_ProcessRun();
            break;
            
        case ECUM_STATE_POST_RUN:
            EcuM_ProcessPostRun();
            break;
            
        case ECUM_STATE_SHUTDOWN:
        case ECUM_STATE_SHUTDOWN_ONE:
        case ECUM_STATE_SHUTDOWN_TWO:
            EcuM_ProcessShutdown();
            break;
            
        default:
            break;
    }
}

Std_ReturnType EcuM_RequestRUN(EcuM_UserType user)
{
    if (user >= ECUM_MAX_USERS) {
        return E_NOT_OK;
    }
    
    if (!EcuM_Users[user]) {
        EcuM_Users[user] = TRUE;
        EcuM_Global.RunCounter++;
    }
    
    return E_OK;
}

Std_ReturnType EcuM_ReleaseRUN(EcuM_UserType user)
{
    if (user >= ECUM_MAX_USERS) {
        return E_NOT_OK;
    }
    
    if (EcuM_Users[user]) {
        EcuM_Users[user] = FALSE;
        if (EcuM_Global.RunCounter > 0) {
            EcuM_Global.RunCounter--;
        }
    }
    
    return E_OK;
}

Std_ReturnType EcuM_RequestPOST_RUN(EcuM_UserType user)
{
    (void)user;
    EcuM_Global.PostRunCounter++;
    return E_OK;
}

Std_ReturnType EcuM_ReleasePOST_RUN(EcuM_UserType user)
{
    (void)user;
    if (EcuM_Global.PostRunCounter > 0) {
        EcuM_Global.PostRunCounter--;
    }
    return E_OK;
}

Std_ReturnType EcuM_SelectShutdownTarget(EcuM_ShutdownTargetType target, uint32 mode)
{
    EcuM_Global.ShutdownTarget = target;
    EcuM_Global.ShutdownMode = mode;
    return E_OK;
}

Std_ReturnType EcuM_GetShutdownTarget(EcuM_ShutdownTargetType* shutdownTarget, uint32* mode)
{
    if (shutdownTarget == NULL_PTR || mode == NULL_PTR) {
        return E_NOT_OK;
    }
    
    *shutdownTarget = EcuM_Global.ShutdownTarget;
    *mode = EcuM_Global.ShutdownMode;
    
    return E_OK;
}

Std_ReturnType EcuM_SelectShutdownCause(EcuM_ShutdownTargetType target, EcuM_ShutdownCauseType cause)
{
    (void)target;
    EcuM_Global.ShutdownCause = cause;
    return E_OK;
}

Std_ReturnType EcuM_GetState(EcuM_StateType* state)
{
    if (state == NULL_PTR) {
        return E_NOT_OK;
    }
    
    *state = EcuM_Global.State;
    return E_OK;
}

void EcuM_SetWakeupEvent(EcuM_WakeupSourceType sources)
{
    EcuM_Global.WakeupSource |= sources;
}

void EcuM_ClearWakeupEvent(EcuM_WakeupSourceType sources)
{
    EcuM_Global.WakeupSource &= ~sources;
}

EcuM_WakeupStatusType EcuM_GetStatusOfWakeupSource(EcuM_WakeupSourceType sources)
{
    EcuM_WakeupStatusType status;
    if (EcuM_Global.WakeupSource & sources) {
        status = ECUM_WKS_VALIDATED;
    } else {
        status = ECUM_WKS_CLEAR;
    }
    return status;
}

void EcuM_GetVersionInfo(Std_VersionInfoType* versioninfo)
{
    if (versioninfo == NULL_PTR) {
        return;
    }
    
    versioninfo->vendorID = ECUM_VENDOR_ID;
    versioninfo->moduleID = ECUM_MODULE_ID;
    versioninfo->sw_major_version = ECUM_SW_MAJOR_VERSION;
    versioninfo->sw_minor_version = ECUM_SW_MINOR_VERSION;
    versioninfo->sw_patch_version = ECUM_SW_PATCH_VERSION;
}

void EcuM_GoHalt(void)
{
    EcuM_Global.State = ECUM_STATE_SLEEP;
    /* 进入HALT模式的硬件相关代码 */
}

void EcuM_GoPoll(void)
{
    EcuM_Global.State = ECUM_STATE_SLEEP;
    /* 进入POLL模式的硬件相关代码 */
}

/* 弱定义的回调函数 */
__attribute__((weak)) void EcuM_DriverInitZero(void)
{
    /* 默认实现 */
}

__attribute__((weak)) void EcuM_DriverInitOne(void)
{
    /* 默认实现 */
}

__attribute__((weak)) void EcuM_AL_DriverInitOne(void)
{
    /* 默认实现 */
}

__attribute__((weak)) void EcuM_AL_DriverInitTwo(void)
{
    /* 默认实现 */
}

__attribute__((weak)) void EcuM_AL_DriverInitThree(void)
{
    /* 默认实现 */
}

__attribute__((weak)) void EcuM_AL_DriverRestart(void)
{
    /* 默认实现 */
}

__attribute__((weak)) void EcuM_AL_SwitchOff(void)
{
    /* 默认实现 */
}

__attribute__((weak)) void EcuM_AL_Reset(EcuM_ResetType resetType)
{
    (void)resetType;
    /* 默认实现 */
}
