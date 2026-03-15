# API 参考

本文档描述 STM32F407 AUTOSAR 项目的主要 API 接口。

## 目录

- [CAN 驱动 API](#can-驱动-api)
- [调试日志 API](#调试日志-api)
- [COM 模块 API](#com-模块-api)
- [DCM 模块 API](#dcm-模块-api)
- [ECUM 模块 API](#ecum-模块-api)

---

## CAN 驱动 API

### 头文件

```c
#include "CanDriver.h"
```

### 数据类型

#### CanMessage

CAN 消息结构体：

```c
typedef struct {
    uint32_t Id;        /* CAN ID */
    uint8_t  IsExtId;   /* 1=扩展帧, 0=标准帧 */
    uint8_t  IsRemote;  /* 1=远程帧, 0=数据帧 */
    uint8_t  Dlc;       /* 数据长度 (0-8) */
    uint8_t  Data[8];   /* 数据 */
} CanMessage;
```

#### CanStatus

状态码：

| 常量 | 值 | 说明 |
|------|-----|------|
| CAN_STATUS_OK | 0 | 成功 |
| CAN_STATUS_ERROR | 1 | 通用错误 |
| CAN_STATUS_BUSY | 2 | 忙 |
| CAN_STATUS_TIMEOUT | 3 | 超时 |
| CAN_STATUS_NOT_INIT | 4 | 未初始化 |
| CAN_STATUS_TX_FULL | 5 | 发送邮箱满 |
| CAN_STATUS_RX_EMPTY | 6 | 接收缓冲区空 |

### 函数

#### CanDriver_Init

初始化 CAN 驱动。

```c
CanStatus CanDriver_Init(void);
```

**参数**: 无

**返回**: 
- `CAN_STATUS_OK` - 初始化成功
- `CAN_STATUS_ERROR` - 初始化失败

**示例**:
```c
CanStatus status = CanDriver_Init();
if (status != CAN_STATUS_OK) {
    // 处理错误
}
```

---

#### CanDriver_Send

发送 CAN 消息。

```c
CanStatus CanDriver_Send(const CanMessage *msg, uint32_t timeout);
```

**参数**:
- `msg` - 指向 CAN 消息的指针
- `timeout` - 超时时间（毫秒），0 表示非阻塞

**返回**:
- `CAN_STATUS_OK` - 发送成功
- `CAN_STATUS_NOT_INIT` - 未初始化
- `CAN_STATUS_TX_FULL` - 发送邮箱满
- `CAN_STATUS_BUSY` - 发送超时

**示例**:
```c
CanMessage msg = {
    .Id = 0x123,
    .IsExtId = 0,
    .IsRemote = 0,
    .Dlc = 8,
    .Data = {0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88}
};

CanStatus status = CanDriver_Send(&msg, 100);
```

---

#### CanDriver_Receive

接收 CAN 消息。

```c
CanStatus CanDriver_Receive(CanMessage *msg);
```

**参数**:
- `msg` - 指向 CAN 消息的指针，用于存储接收到的数据

**返回**:
- `CAN_STATUS_OK` - 接收成功
- `CAN_STATUS_NOT_INIT` - 未初始化
- `CAN_STATUS_RX_EMPTY` - 接收缓冲区空

**示例**:
```c
CanMessage msg;
CanStatus status = CanDriver_Receive(&msg);
if (status == CAN_STATUS_OK) {
    // 处理接收到的消息
}
```

---

#### CanDriver_IsMessagePending

检查是否有待接收的消息。

```c
uint8_t CanDriver_IsMessagePending(void);
```

**参数**: 无

**返回**:
- `1` - 有消息待接收
- `0` - 无消息

**示例**:
```c
if (CanDriver_IsMessagePending()) {
    CanMessage msg;
    CanDriver_Receive(&msg);
}
```

---

## 调试日志 API

### 头文件

```c
#include "DebugLog.h"
```

### 函数

#### DebugLog_Init

初始化调试日志模块。

```c
void DebugLog_Init(void);
```

**参数**: 无

**返回**: 无

**说明**: 配置 USART1 为 115200bps

---

#### DebugLog_String

输出字符串。

```c
void DebugLog_String(const char* str);
```

**参数**:
- `str` - 要输出的字符串

**示例**:
```c
DebugLog_String("Hello World\r\n");
```

---

#### DebugLog_Dec

输出十进制数字。

```c
void DebugLog_Dec(uint32_t num);
```

**参数**:
- `num` - 要输出的数字

**示例**:
```c
DebugLog_Dec(12345);  // 输出: 12345
```

---

#### DebugLog_Hex

输出十六进制数据。

```c
void DebugLog_Hex(const uint8_t* data, uint8_t len);
```

**参数**:
- `data` - 数据指针
- `len` - 数据长度

**示例**:
```c
uint8_t data[] = {0x11, 0x22, 0x33};
DebugLog_Hex(data, 3);  // 输出: 11 22 33
```

---

#### DebugLog_CanRx

输出 CAN 接收日志。

```c
void DebugLog_CanRx(uint32_t canId, uint8_t isExtId, uint8_t dlc, const uint8_t* data);
```

**参数**:
- `canId` - CAN ID
- `isExtId` - 是否为扩展帧
- `dlc` - 数据长度
- `data` - 数据指针

---

## COM 模块 API

### 头文件

```c
#include "Com.h"
```

### 函数

#### Com_Init

初始化 COM 模块。

```c
void Com_Init(const Com_ConfigType* config);
```

---

#### Com_SendSignal

发送信号。

```c
uint8_t Com_SendSignal(uint16_t signalId, const void* signalDataPtr);
```

**参数**:
- `signalId` - 信号 ID
- `signalDataPtr` - 信号数据指针

**返回**:
- `E_OK` - 成功
- `E_NOT_OK` - 失败

**示例**:
```c
uint16_t speed = 100;
Com_SendSignal(1, &speed);
```

---

#### Com_ReceiveSignal

接收信号。

```c
uint8_t Com_ReceiveSignal(uint16_t signalId, void* signalDataPtr);
```

**参数**:
- `signalId` - 信号 ID
- `signalDataPtr` - 用于存储信号数据的指针

**返回**:
- `E_OK` - 成功
- `E_NOT_OK` - 失败

---

## DCM 模块 API

### 头文件

```c
#include "Dcm.h"
```

### 函数

#### Dcm_Init

初始化 DCM 模块。

```c
void Dcm_Init(const Dcm_ConfigType* config);
```

---

#### Dcm_MainFunction

DCM 主函数（周期性调用）。

```c
void Dcm_MainFunction(void);
```

**说明**: 应在 1ms 周期任务中调用

---

#### Dcm_ProcessDiagnosticRequest

处理诊断请求。

```c
void Dcm_ProcessDiagnosticRequest(const PduInfoType* pduInfo);
```

**参数**:
- `pduInfo` - PDU 信息

---

## ECUM 模块 API

### 头文件

```c
#include "EcuM.h"
```

### 函数

#### EcuM_Init

初始化 ECU 状态管理模块。

```c
void EcuM_Init(const EcuM_ConfigType* config);
```

---

#### EcuM_StartupTwo

启动第二步（OS 启动后调用）。

```c
void EcuM_StartupTwo(void);
```

---

#### EcuM_MainFunction

ECUM 主函数（周期性调用）。

```c
void EcuM_MainFunction(void);
```

**说明**: 应在 5ms 周期任务中调用

---

#### EcuM_RequestRUN

请求 RUN 状态。

```c
void EcuM_RequestRUN(uint8_t user);
```

**参数**:
- `user` - 用户 ID

---

#### EcuM_ReleaseRUN

释放 RUN 状态。

```c
void EcuM_ReleaseRUN(uint8_t user);
```

**参数**:
- `user` - 用户 ID

---

## 配置 API

### AUTOSAR 配置

配置文件：`AUTOSAR/AUTOSAR_Cfg.h`

#### CAN ID 配置

```c
/* 发送 PDU ID */
#define CAN_TX_ID_TEST      0x123       /* 测试帧 */
#define CAN_TX_ID_OBD       0x7DF       /* OBD 功能请求 */
#define CAN_TX_ID_UDS_RESP  0x18DAF100  /* UDS 诊断响应 */

/* 接收 PDU ID */
#define CAN_RX_ID_TEST      0x123       /* 测试帧 */
#define CAN_RX_ID_UDS_REQ   0x7E0       /* UDS 诊断请求 */
#define CAN_RX_ID_UDS_EXT   0x18DA00F1  /* UDS 扩展帧请求 */
```

#### 信号配置

```c
/* 信号 ID */
#define COM_SIGNAL_COUNTER  0   /* 计数器信号 */
#define COM_SIGNAL_SPEED    1   /* 车速信号 */
#define COM_SIGNAL_STATUS   2   /* 状态信号 */
```

#### DID 配置

```c
/* 数据标识符 */
#define DID_VIN             0xF190  /* VIN */
#define DID_ECU_NAME        0xF197  /* ECU 名称 */
#define DID_SYSTEM_STATUS   0xF100  /* 系统状态 */
#define DID_VEHICLE_SPEED   0x0100  /* 车速 */
#define DID_ENGINE_TEMP     0x0105  /* 发动机温度 */
```

---

## 错误处理

### 错误码定义

```c
#define E_OK        0   /* 成功 */
#define E_NOT_OK    1   /* 失败 */
```

### 调试输出

使用 DebugLog 模块输出调试信息：

```c
DebugLog_String("[APP] Error occurred\r\n");
DebugLog_Dec(errorCode);
```

---

## 更多示例

### 完整示例：发送 CAN 消息

```c
#include "CanDriver.h"
#include "DebugLog.h"

void SendCanMessage(void)
{
    CanMessage msg;
    
    /* 构造消息 */
    msg.Id = 0x123;
    msg.IsExtId = 0;  /* 标准帧 */
    msg.IsRemote = 0; /* 数据帧 */
    msg.Dlc = 8;
    msg.Data[0] = 0x11;
    msg.Data[1] = 0x22;
    msg.Data[2] = 0x33;
    msg.Data[3] = 0x44;
    msg.Data[4] = 0x55;
    msg.Data[5] = 0x66;
    msg.Data[6] = 0x77;
    msg.Data[7] = 0x88;
    
    /* 发送 */
    CanStatus status = CanDriver_Send(&msg, 100);
    if (status == CAN_STATUS_OK) {
        DebugLog_String("[APP] CAN message sent\r\n");
    } else {
        DebugLog_String("[APP] CAN send failed\r\n");
    }
}
```

### 完整示例：周期性任务

```c
#include "Com.h"
#include "EcuM.h"
#include "Dcm.h"

void MainLoop(void)
{
    static uint32_t tick = 0;
    tick++;
    
    /* 1ms 任务 */
    Dcm_MainFunction();
    
    /* 5ms 任务 */
    if ((tick % 5) == 0) {
        EcuM_MainFunction();
    }
    
    /* 10ms 任务 */
    if ((tick % 10) == 0) {
        Com_MainFunctionRx();
        Com_MainFunctionTx();
    }
    
    /* 100ms 任务 */
    if ((tick % 100) == 0) {
        Com_MainFunctionRouteSignals();
    }
}
```
