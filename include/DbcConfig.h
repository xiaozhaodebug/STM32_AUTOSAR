/**
 * @file        DbcConfig.h
 * @brief       DBC 配置头文件 (自动生成)
 * @generated   2026-03-15 16:13:39
 * 
 * @note        此文件由 dbc_generator.py 自动生成，请勿手动修改
 */

#ifndef DBC_CONFIG_H
#define DBC_CONFIG_H

#include <stdint.h>

/*===================================================================================
 *                                      消息 ID 定义
 *=================================================================================*/

#define MSG_ID_MEG_FUNDIAG                       0x7DF   /* XZ_B -> XZ_A */
#define MSG_ID_MEG_PHYDIAG                       0x735   /* XZ_B -> XZ_A */
#define MSG_ID_MEG_RESDIAG                       0x73D   /* XZ_A -> XZ_B */
#define MSG_ID_MEG_NMRX                          0x600   /* XZ_B -> XZ_A */
#define MSG_ID_MEG_NMTX                          0x601   /* XZ_A -> XZ_B */
#define MSG_ID_XZ_A_LED_STATE                    0x210   /* XZ_A -> XZ_B */
#define MSG_ID_XZ_A_KEY_STATE                    0x211   /* XZ_A -> XZ_B */
#define MSG_ID_XZ_B_MOTORSTATE                   0x123   /* XZ_B -> XZ_A */
#define MSG_ID_XZ_B_CTRL_STATE                   0x124   /* XZ_B -> XZ_A */
#define MSG_ID_XZ_A_POWER_STATE                  0x212   /* XZ_A -> XZ_B */

/*===================================================================================
 *                                      消息周期定义 (ms)
 *=================================================================================*/

#define CYCLE_TIME_XZ_A_LED_STATE                50
#define CYCLE_TIME_XZ_A_KEY_STATE                30
#define CYCLE_TIME_XZ_A_POWER_STATE              50

/*===================================================================================
 *                                      信号定义
 *=================================================================================*/

/* Meg_FunDiag (0x7DF) 信号定义 */
#define SIG_MEG_FUNDIAG_SIG_FUNDIAG_START_BIT   0
#define SIG_MEG_FUNDIAG_SIG_FUNDIAG_SIZE        64
#define SIG_MEG_FUNDIAG_SIG_FUNDIAG_FACTOR      1.0
#define SIG_MEG_FUNDIAG_SIG_FUNDIAG_OFFSET      0.0

/* Meg_PhyDiag (0x735) 信号定义 */
#define SIG_MEG_PHYDIAG_SIG_PHYDIAG_START_BIT   0
#define SIG_MEG_PHYDIAG_SIG_PHYDIAG_SIZE        64
#define SIG_MEG_PHYDIAG_SIG_PHYDIAG_FACTOR      1.0
#define SIG_MEG_PHYDIAG_SIG_PHYDIAG_OFFSET      0.0

/* Meg_ResDiag (0x73D) 信号定义 */
#define SIG_MEG_RESDIAG_SIG_RESDIAG_START_BIT   0
#define SIG_MEG_RESDIAG_SIG_RESDIAG_SIZE        64
#define SIG_MEG_RESDIAG_SIG_RESDIAG_FACTOR      1.0
#define SIG_MEG_RESDIAG_SIG_RESDIAG_OFFSET      0.0

/* Meg_NmRx (0x600) 信号定义 */
#define SIG_MEG_NMRX_SIG_NMRX_NID_START_BIT   0
#define SIG_MEG_NMRX_SIG_NMRX_NID_SIZE        8
#define SIG_MEG_NMRX_SIG_NMRX_NID_FACTOR      1.0
#define SIG_MEG_NMRX_SIG_NMRX_NID_OFFSET      0.0

/* Meg_NmTx (0x601) 信号定义 */
#define SIG_MEG_NMTX_SIG_NMTX_NID_START_BIT   0
#define SIG_MEG_NMTX_SIG_NMTX_NID_SIZE        8
#define SIG_MEG_NMTX_SIG_NMTX_NID_FACTOR      1.0
#define SIG_MEG_NMTX_SIG_NMTX_NID_OFFSET      0.0

/* XZ_A_Led_State (0x210) 信号定义 */
#define SIG_XZ_A_LED_STATE_LED1_STATE_START_BIT   0
#define SIG_XZ_A_LED_STATE_LED1_STATE_SIZE        2
#define SIG_XZ_A_LED_STATE_LED1_STATE_FACTOR      1.0
#define SIG_XZ_A_LED_STATE_LED1_STATE_OFFSET      0.0

#define SIG_XZ_A_LED_STATE_LED2_STATE_START_BIT   2
#define SIG_XZ_A_LED_STATE_LED2_STATE_SIZE        2
#define SIG_XZ_A_LED_STATE_LED2_STATE_FACTOR      1.0
#define SIG_XZ_A_LED_STATE_LED2_STATE_OFFSET      0.0

#define SIG_XZ_A_LED_STATE_LED3_STATE_START_BIT   4
#define SIG_XZ_A_LED_STATE_LED3_STATE_SIZE        2
#define SIG_XZ_A_LED_STATE_LED3_STATE_FACTOR      1.0
#define SIG_XZ_A_LED_STATE_LED3_STATE_OFFSET      0.0

#define SIG_XZ_A_LED_STATE_LED4_STATE_START_BIT   6
#define SIG_XZ_A_LED_STATE_LED4_STATE_SIZE        2
#define SIG_XZ_A_LED_STATE_LED4_STATE_FACTOR      1.0
#define SIG_XZ_A_LED_STATE_LED4_STATE_OFFSET      0.0

/* XZ_A_KEY_State (0x211) 信号定义 */
#define SIG_XZ_A_KEY_STATE_KEY1_STATE_START_BIT   0
#define SIG_XZ_A_KEY_STATE_KEY1_STATE_SIZE        2
#define SIG_XZ_A_KEY_STATE_KEY1_STATE_FACTOR      1.0
#define SIG_XZ_A_KEY_STATE_KEY1_STATE_OFFSET      0.0

#define SIG_XZ_A_KEY_STATE_KEY2_STATE_START_BIT   2
#define SIG_XZ_A_KEY_STATE_KEY2_STATE_SIZE        2
#define SIG_XZ_A_KEY_STATE_KEY2_STATE_FACTOR      1.0
#define SIG_XZ_A_KEY_STATE_KEY2_STATE_OFFSET      0.0

#define SIG_XZ_A_KEY_STATE_KEY3_STATE_START_BIT   4
#define SIG_XZ_A_KEY_STATE_KEY3_STATE_SIZE        2
#define SIG_XZ_A_KEY_STATE_KEY3_STATE_FACTOR      1.0
#define SIG_XZ_A_KEY_STATE_KEY3_STATE_OFFSET      0.0

#define SIG_XZ_A_KEY_STATE_KEY4_STATE_START_BIT   6
#define SIG_XZ_A_KEY_STATE_KEY4_STATE_SIZE        2
#define SIG_XZ_A_KEY_STATE_KEY4_STATE_FACTOR      1.0
#define SIG_XZ_A_KEY_STATE_KEY4_STATE_OFFSET      0.0

/* XZ_B_MotorState (0x123) 信号定义 */
#define SIG_XZ_B_MOTORSTATE_ONOFF_START_BIT   0
#define SIG_XZ_B_MOTORSTATE_ONOFF_SIZE        1
#define SIG_XZ_B_MOTORSTATE_ONOFF_FACTOR      1.0
#define SIG_XZ_B_MOTORSTATE_ONOFF_OFFSET      0.0

#define SIG_XZ_B_MOTORSTATE_CARSPEED_START_BIT   1
#define SIG_XZ_B_MOTORSTATE_CARSPEED_SIZE        15
#define SIG_XZ_B_MOTORSTATE_CARSPEED_FACTOR      1.0
#define SIG_XZ_B_MOTORSTATE_CARSPEED_OFFSET      0.0

/* XZ_B_Ctrl_State (0x124) 信号定义 */
#define SIG_XZ_B_CTRL_STATE_CTRL_LED3_STATE_START_BIT   2
#define SIG_XZ_B_CTRL_STATE_CTRL_LED3_STATE_SIZE        1
#define SIG_XZ_B_CTRL_STATE_CTRL_LED3_STATE_FACTOR      1.0
#define SIG_XZ_B_CTRL_STATE_CTRL_LED3_STATE_OFFSET      0.0

#define SIG_XZ_B_CTRL_STATE_CTRL_LED4_STATE_START_BIT   3
#define SIG_XZ_B_CTRL_STATE_CTRL_LED4_STATE_SIZE        1
#define SIG_XZ_B_CTRL_STATE_CTRL_LED4_STATE_FACTOR      1.0
#define SIG_XZ_B_CTRL_STATE_CTRL_LED4_STATE_OFFSET      0.0

/* XZ_A_Power_State (0x212) 信号定义 */
#define SIG_XZ_A_POWER_STATE_KL15_STATE_START_BIT   0
#define SIG_XZ_A_POWER_STATE_KL15_STATE_SIZE        2
#define SIG_XZ_A_POWER_STATE_KL15_STATE_FACTOR      1.0
#define SIG_XZ_A_POWER_STATE_KL15_STATE_OFFSET      0.0

#define SIG_XZ_A_POWER_STATE_ADC_VALUE_START_BIT   8
#define SIG_XZ_A_POWER_STATE_ADC_VALUE_SIZE        16
#define SIG_XZ_A_POWER_STATE_ADC_VALUE_FACTOR      1.0
#define SIG_XZ_A_POWER_STATE_ADC_VALUE_OFFSET      0.0

/*===================================================================================
 *                                      数据结构定义
 *=================================================================================*/

typedef struct {
    uint32_t   Sig_FunDiag;   /* 64bit, N/A */
} Meg_FunDiag_t;

typedef struct {
    uint32_t   Sig_PhyDiag;   /* 64bit, N/A */
} Meg_PhyDiag_t;

typedef struct {
    uint32_t   Sig_ResDiag;   /* 64bit, N/A */
} Meg_ResDiag_t;

typedef struct {
    uint8_t    Sig_NmRx_Nid;   /* 8bit, N/A */
} Meg_NmRx_t;

typedef struct {
    uint8_t    Sig_NmTx_Nid;   /* 8bit, N/A */
} Meg_NmTx_t;

typedef struct {
    uint8_t    Led1_State;   /* 2bit, N/A */
    uint8_t    Led2_State;   /* 2bit, N/A */
    uint8_t    Led3_State;   /* 2bit, N/A */
    uint8_t    Led4_State;   /* 2bit, N/A */
} XZ_A_Led_State_t;

typedef struct {
    uint8_t    Key1_State;   /* 2bit, N/A */
    uint8_t    Key2_State;   /* 2bit, N/A */
    uint8_t    Key3_State;   /* 2bit, N/A */
    uint8_t    Key4_State;   /* 2bit, N/A */
} XZ_A_KEY_State_t;

typedef struct {
    uint8_t    OnOff;   /* 1bit, N/A */
    uint16_t   CarSpeed;   /* 15bit, N/A */
} XZ_B_MotorState_t;

typedef struct {
    uint8_t    Ctrl_Led3_State;   /* 1bit, N/A */
    uint8_t    Ctrl_Led4_State;   /* 1bit, N/A */
} XZ_B_Ctrl_State_t;

typedef struct {
    uint8_t    KL15_State;   /* 2bit, N/A */
    uint16_t   ADC_Value;   /* 16bit, N/A */
} XZ_A_Power_State_t;

/*===================================================================================
 *                                      函数声明
 *=================================================================================*/

#ifdef __cplusplus
extern "C" {
#endif

void Dbc_Parse_Meg_FunDiag(const uint8_t* data, Meg_FunDiag_t* msg);
void Dbc_Parse_Meg_PhyDiag(const uint8_t* data, Meg_PhyDiag_t* msg);
void Dbc_Parse_Meg_NmRx(const uint8_t* data, Meg_NmRx_t* msg);
void Dbc_Parse_XZ_B_MotorState(const uint8_t* data, XZ_B_MotorState_t* msg);
void Dbc_Parse_XZ_B_Ctrl_State(const uint8_t* data, XZ_B_Ctrl_State_t* msg);

void Dbc_Pack_Meg_ResDiag(uint8_t* data, const Meg_ResDiag_t* msg);
void Dbc_Pack_Meg_NmTx(uint8_t* data, const Meg_NmTx_t* msg);
void Dbc_Pack_XZ_A_Led_State(uint8_t* data, const XZ_A_Led_State_t* msg);
void Dbc_Pack_XZ_A_KEY_State(uint8_t* data, const XZ_A_KEY_State_t* msg);
void Dbc_Pack_XZ_A_Power_State(uint8_t* data, const XZ_A_Power_State_t* msg);

/* 信号打包/解包辅助函数 (Little Endian) */
uint64_t Dbc_SignalUnpack(const uint8_t* data, uint16_t startBit, uint8_t bitSize);
void Dbc_SignalPack(uint8_t* data, uint64_t value, uint16_t startBit, uint8_t bitSize);

#ifdef __cplusplus
}
#endif

#endif /* DBC_CONFIG_H */
