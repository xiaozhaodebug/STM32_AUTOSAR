/**
 * @file        DbcHandler.c
 * @brief       DBC 信号处理实现 (自动生成)
 * @generated   2026-03-15 16:13:39
 * 
 * @note        此文件由 dbc_generator.py 自动生成，请勿手动修改
 */

#include "DbcConfig.h"
#include "DebugLog.h"

/*===================================================================================
 *                                      内部函数
 *=================================================================================*/

uint64_t Dbc_SignalUnpack(const uint8_t* data, uint16_t startBit, uint8_t bitSize)
{
    uint64_t value = 0;
    uint16_t byteIndex = startBit / 8;
    uint8_t bitOffset = startBit % 8;
    uint8_t bitsRemaining = bitSize;
    uint8_t shift = 0;
    
    while (bitsRemaining > 0) {
        uint8_t bitsInByte = 8 - bitOffset;
        if (bitsInByte > bitsRemaining) {
            bitsInByte = bitsRemaining;
        }
        uint8_t mask = ((1U << bitsInByte) - 1) << bitOffset;
        uint64_t byteValue = (data[byteIndex] & mask) >> bitOffset;
        value |= (byteValue << shift);
        shift += bitsInByte;
        bitsRemaining -= bitsInByte;
        bitOffset = 0;
        byteIndex++;
    }
    return value;
}

void Dbc_SignalPack(uint8_t* data, uint64_t value, uint16_t startBit, uint8_t bitSize)
{
    uint16_t byteIndex = startBit / 8;
    uint8_t bitOffset = startBit % 8;
    uint8_t bitsRemaining = bitSize;
    uint8_t shift = 0;
    
    while (bitsRemaining > 0) {
        uint8_t bitsInByte = 8 - bitOffset;
        if (bitsInByte > bitsRemaining) {
            bitsInByte = bitsRemaining;
        }
        uint8_t mask = (1U << bitsInByte) - 1;
        uint8_t byteValue = (value >> shift) & mask;
        data[byteIndex] &= ~(mask << bitOffset);
        data[byteIndex] |= (byteValue << bitOffset);
        shift += bitsInByte;
        bitsRemaining -= bitsInByte;
        bitOffset = 0;
        byteIndex++;
    }
}

/*===================================================================================
 *                                      接收消息解析
 *=================================================================================*/

void Dbc_Parse_Meg_FunDiag(const uint8_t* data, Meg_FunDiag_t* msg)
{
    if (data == 0 || msg == 0) return;
    
    msg->Sig_FunDiag = (uint32_t)Dbc_SignalUnpack(data, SIG_MEG_FUNDIAG_SIG_FUNDIAG_START_BIT, SIG_MEG_FUNDIAG_SIG_FUNDIAG_SIZE);
}

void Dbc_Parse_Meg_PhyDiag(const uint8_t* data, Meg_PhyDiag_t* msg)
{
    if (data == 0 || msg == 0) return;
    
    msg->Sig_PhyDiag = (uint32_t)Dbc_SignalUnpack(data, SIG_MEG_PHYDIAG_SIG_PHYDIAG_START_BIT, SIG_MEG_PHYDIAG_SIG_PHYDIAG_SIZE);
}

void Dbc_Parse_Meg_NmRx(const uint8_t* data, Meg_NmRx_t* msg)
{
    if (data == 0 || msg == 0) return;
    
    msg->Sig_NmRx_Nid = (uint8_t)Dbc_SignalUnpack(data, SIG_MEG_NMRX_SIG_NMRX_NID_START_BIT, SIG_MEG_NMRX_SIG_NMRX_NID_SIZE);
}

void Dbc_Parse_XZ_B_MotorState(const uint8_t* data, XZ_B_MotorState_t* msg)
{
    if (data == 0 || msg == 0) return;
    
    msg->OnOff = (uint8_t)Dbc_SignalUnpack(data, SIG_XZ_B_MOTORSTATE_ONOFF_START_BIT, SIG_XZ_B_MOTORSTATE_ONOFF_SIZE);
    msg->CarSpeed = (uint16_t)Dbc_SignalUnpack(data, SIG_XZ_B_MOTORSTATE_CARSPEED_START_BIT, SIG_XZ_B_MOTORSTATE_CARSPEED_SIZE);
}

void Dbc_Parse_XZ_B_Ctrl_State(const uint8_t* data, XZ_B_Ctrl_State_t* msg)
{
    if (data == 0 || msg == 0) return;
    
    msg->Ctrl_Led3_State = (uint8_t)Dbc_SignalUnpack(data, SIG_XZ_B_CTRL_STATE_CTRL_LED3_STATE_START_BIT, SIG_XZ_B_CTRL_STATE_CTRL_LED3_STATE_SIZE);
    msg->Ctrl_Led4_State = (uint8_t)Dbc_SignalUnpack(data, SIG_XZ_B_CTRL_STATE_CTRL_LED4_STATE_START_BIT, SIG_XZ_B_CTRL_STATE_CTRL_LED4_STATE_SIZE);
}

/*===================================================================================
 *                                      发送消息打包
 *=================================================================================*/

void Dbc_Pack_Meg_ResDiag(uint8_t* data, const Meg_ResDiag_t* msg)
{
    if (data == 0 || msg == 0) return;
    
    data[0] = 0;
    data[1] = 0;
    data[2] = 0;
    data[3] = 0;
    data[4] = 0;
    data[5] = 0;
    data[6] = 0;
    data[7] = 0;
    Dbc_SignalPack(data, msg->Sig_ResDiag, SIG_MEG_RESDIAG_SIG_RESDIAG_START_BIT, SIG_MEG_RESDIAG_SIG_RESDIAG_SIZE);
}

void Dbc_Pack_Meg_NmTx(uint8_t* data, const Meg_NmTx_t* msg)
{
    if (data == 0 || msg == 0) return;
    
    data[0] = 0;
    data[1] = 0;
    data[2] = 0;
    data[3] = 0;
    data[4] = 0;
    data[5] = 0;
    data[6] = 0;
    data[7] = 0;
    Dbc_SignalPack(data, msg->Sig_NmTx_Nid, SIG_MEG_NMTX_SIG_NMTX_NID_START_BIT, SIG_MEG_NMTX_SIG_NMTX_NID_SIZE);
}

void Dbc_Pack_XZ_A_Led_State(uint8_t* data, const XZ_A_Led_State_t* msg)
{
    if (data == 0 || msg == 0) return;
    
    data[0] = 0;
    data[1] = 0;
    data[2] = 0;
    data[3] = 0;
    data[4] = 0;
    data[5] = 0;
    data[6] = 0;
    data[7] = 0;
    Dbc_SignalPack(data, msg->Led1_State, SIG_XZ_A_LED_STATE_LED1_STATE_START_BIT, SIG_XZ_A_LED_STATE_LED1_STATE_SIZE);
    Dbc_SignalPack(data, msg->Led2_State, SIG_XZ_A_LED_STATE_LED2_STATE_START_BIT, SIG_XZ_A_LED_STATE_LED2_STATE_SIZE);
    Dbc_SignalPack(data, msg->Led3_State, SIG_XZ_A_LED_STATE_LED3_STATE_START_BIT, SIG_XZ_A_LED_STATE_LED3_STATE_SIZE);
    Dbc_SignalPack(data, msg->Led4_State, SIG_XZ_A_LED_STATE_LED4_STATE_START_BIT, SIG_XZ_A_LED_STATE_LED4_STATE_SIZE);
}

void Dbc_Pack_XZ_A_KEY_State(uint8_t* data, const XZ_A_KEY_State_t* msg)
{
    if (data == 0 || msg == 0) return;
    
    data[0] = 0;
    data[1] = 0;
    data[2] = 0;
    data[3] = 0;
    data[4] = 0;
    data[5] = 0;
    data[6] = 0;
    data[7] = 0;
    Dbc_SignalPack(data, msg->Key1_State, SIG_XZ_A_KEY_STATE_KEY1_STATE_START_BIT, SIG_XZ_A_KEY_STATE_KEY1_STATE_SIZE);
    Dbc_SignalPack(data, msg->Key2_State, SIG_XZ_A_KEY_STATE_KEY2_STATE_START_BIT, SIG_XZ_A_KEY_STATE_KEY2_STATE_SIZE);
    Dbc_SignalPack(data, msg->Key3_State, SIG_XZ_A_KEY_STATE_KEY3_STATE_START_BIT, SIG_XZ_A_KEY_STATE_KEY3_STATE_SIZE);
    Dbc_SignalPack(data, msg->Key4_State, SIG_XZ_A_KEY_STATE_KEY4_STATE_START_BIT, SIG_XZ_A_KEY_STATE_KEY4_STATE_SIZE);
}

void Dbc_Pack_XZ_A_Power_State(uint8_t* data, const XZ_A_Power_State_t* msg)
{
    if (data == 0 || msg == 0) return;
    
    data[0] = 0;
    data[1] = 0;
    data[2] = 0;
    data[3] = 0;
    data[4] = 0;
    data[5] = 0;
    data[6] = 0;
    data[7] = 0;
    Dbc_SignalPack(data, msg->KL15_State, SIG_XZ_A_POWER_STATE_KL15_STATE_START_BIT, SIG_XZ_A_POWER_STATE_KL15_STATE_SIZE);
    Dbc_SignalPack(data, msg->ADC_Value, SIG_XZ_A_POWER_STATE_ADC_VALUE_START_BIT, SIG_XZ_A_POWER_STATE_ADC_VALUE_SIZE);
}
