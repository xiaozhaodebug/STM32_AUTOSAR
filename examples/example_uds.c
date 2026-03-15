/**
 * @file        example_uds.c
 * @brief       UDS 诊断示例
 * @details     演示如何使用 UDS 诊断服务
 * 
 * @author      [你的名字]
 * @date        2026-03-15
 */

#include "Dcm.h"
#include "DebugLog.h"
#include "ComStack_Types.h"

/**
 * @brief  UDS 会话控制示例
 * @note   0x10 服务 - 切换诊断会话
 */
void Example_UdsSessionControl(void)
{
    /* UDS 会话类型 */
    #define UDS_SESSION_DEFAULT     0x01
    #define UDS_SESSION_PROGRAMMING 0x02
    #define UDS_SESSION_EXTENDED    0x03
    #define UDS_SESSION_SAFETY      0x04
    
    uint8_t sessionType = UDS_SESSION_EXTENDED;
    
    DebugLog_String("[UDS] Requesting extended session\r\n");
    
    /* 实际应用中会调用 DCM 服务处理 */
    /* Dcm_ProcessSessionControl(sessionType); */
}

/**
 * @brief  UDS 读取 DID 示例
 * @note   0x22 服务 - 读取数据标识符
 */
void Example_UdsReadDid(void)
{
    /* 常见 DID */
    #define DID_VIN             0xF190
    #define DID_ECU_NAME        0xF197
    #define DID_SYSTEM_STATUS   0xF100
    #define DID_VEHICLE_SPEED   0x0100
    #define DID_ENGINE_TEMP     0x0105
    
    uint16_t did = DID_VIN;
    uint8_t data[17];
    
    DebugLog_String("[UDS] Reading DID 0x");
    DebugLog_HexNum(did, 4);
    DebugLog_NewLine();
    
    /* 模拟读取 VIN */
    const char* vin = "1HGBH41JXMN109186";
    for (int i = 0; i < 17; i++) {
        data[i] = vin[i];
    }
    
    DebugLog_String("[UDS] VIN: ");
    DebugLog_String(vin);
    DebugLog_NewLine();
}

/**
 * @brief  UDS 写入 DID 示例
 * @note   0x2E 服务 - 写入数据标识符
 */
void Example_UdsWriteDid(void)
{
    uint16_t did = DID_SYSTEM_STATUS;
    uint8_t data[4] = {0x01, 0x02, 0x03, 0x04};
    
    DebugLog_String("[UDS] Writing DID 0x");
    DebugLog_HexNum(did, 4);
    DebugLog_NewLine();
    
    /* 检查是否允许写入 */
    /* 实际应用中会调用 DCM 服务处理 */
}

/**
 * @brief  UDS 安全访问示例
 * @note   0x27 服务 - 安全访问
 */
void Example_UdsSecurityAccess(void)
{
    /* 安全访问子功能 */
    #define UDS_SEC_REQUEST_SEED    0x01
    #define UDS_SEC_SEND_KEY        0x02
    
    uint8_t subFunction = UDS_SEC_REQUEST_SEED;
    uint8_t seed[4];
    
    DebugLog_String("[UDS] Requesting security seed\r\n");
    
    /* 生成随机种子 */
    seed[0] = 0x12;
    seed[1] = 0x34;
    seed[2] = 0x56;
    seed[3] = 0x78;
    
    DebugLog_String("[UDS] Seed: ");
    DebugLog_Hex(seed, 4);
    DebugLog_NewLine();
    
    /* 等待客户端发送密钥 */
    /* 密钥计算：通常使用 XOR 或 AES 算法 */
}

/**
 * @brief  UDS ECU 复位示例
 * @note   0x11 服务 - ECU 复位
 */
void Example_UdsEcuReset(void)
{
    /* 复位类型 */
    #define UDS_RESET_HARD      0x01
    #define UDS_RESET_KEY_OFF   0x02
    #define UDS_RESET_SOFT      0x03
    
    uint8_t resetType = UDS_RESET_HARD;
    
    DebugLog_String("[UDS] Requesting ECU reset\r\n");
    
    /* 发送正响应 */
    /* 执行复位 */
    /* NVIC_SystemReset(); */
}

/**
 * @brief  UDS Tester Present 示例
 * @note   0x3E 服务 - Tester Present
 */
void Example_UdsTesterPresent(void)
{
    DebugLog_String("[UDS] Tester present received\r\n");
    
    /* 重置 S3 定时器 */
    /* 保持当前会话 */
}

/**
 * @brief  处理 UDS 请求
 * @param  data 请求数据
 * @param  len  数据长度
 */
void Example_ProcessUdsRequest(const uint8_t* data, uint8_t len)
{
    if (len < 1) {
        return;
    }
    
    uint8_t sid = data[0];  /* 服务标识符 */
    
    switch (sid) {
        case 0x10:
            DebugLog_String("[UDS] Session control request\r\n");
            break;
            
        case 0x11:
            DebugLog_String("[UDS] ECU reset request\r\n");
            break;
            
        case 0x22:
            DebugLog_String("[UDS] Read DID request\r\n");
            break;
            
        case 0x2E:
            DebugLog_String("[UDS] Write DID request\r\n");
            break;
            
        case 0x27:
            DebugLog_String("[UDS] Security access request\r\n");
            break;
            
        case 0x3E:
            DebugLog_String("[UDS] Tester present request\r\n");
            break;
            
        default:
            DebugLog_String("[UDS] Unsupported service\r\n");
            break;
    }
}
