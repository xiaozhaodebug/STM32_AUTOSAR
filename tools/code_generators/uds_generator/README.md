# UDS Diagnostic Configuration Generator

UDS诊断配置代码生成器 - 类似于DBC生成器，用户只需配置Excel表格即可一键生成诊断代码。

## 功能特点

- 📊 **Excel配置**：通过直观的Excel表格配置所有诊断参数
- ⚡ **一键生成**：单条命令生成完整的C代码
- 🔧 **全面支持**：支持Session、DID、Security、Routine、DTC配置
- 📋 **集成友好**：生成代码可直接集成到现有AUTOSAR项目

## 支持的UDS服务

| 服务 | 名称 | 配置支持 |
|------|------|----------|
| 0x10 | DiagnosticSessionControl | ✅ Session参数、P2/S3时间 |
| 0x11 | ECUReset | ✅ 会话支持配置 |
| 0x19 | ReadDTCInformation | ✅ DTC列表、快照、扩展数据 |
| 0x22 | ReadDataByIdentifier | ✅ DID定义、数据类型、权限 |
| 0x27 | SecurityAccess | ✅ 安全级别、算法、延时 |
| 0x2E | WriteDataByIdentifier | ✅ 读写权限配置 |
| 0x31 | RoutineControl | ✅ 例程ID、启动/停止/结果 |
| 0x3E | TesterPresent | ✅ 会话保持配置 |

## 快速开始

### 1. 创建示例配置

```bash
cd tools/uds_generator
./generate_uds.sh --example
```

这将创建 `UDS_Config_Example.xlsx`，包含完整的示例配置。

### 2. 编辑配置

使用Excel打开生成的配置文件，修改以下工作表：

#### SessionConfig - 会话配置

配置诊断会话参数（10服务）：

| SessionType | Name | P2_max | P2*_max | S3_client | S3_server | SupportServices |
|-------------|------|--------|---------|-----------|-----------|-----------------|
| 0x01 | DefaultSession | 50 | 5000 | - | 5000 | 10,11,19,22,3E |
| 0x02 | ProgrammingSession | 50 | 5000 | 2000 | 5000 | 10,11,27,31,34,35,36,37 |
| 0x03 | ExtendedSession | 50 | 5000 | 2000 | 5000 | 10,11,19,22,2E,27,31,3E |

#### DIDConfig - 数据标识符配置

配置可读写的DID（22/2E服务）：

| DID(Hex) | Name | DataLength | ReadOnly | SecurityLevel | SessionMask | DataType |
|----------|------|------------|----------|---------------|-------------|----------|
| 0xF190 | VIN | 17 | Yes | 0x01 | 0x07 | ASCII |
| 0xF197 | ECU_Name | 16 | Yes | 0x01 | 0x07 | ASCII |
| 0xF100 | SystemState | 4 | No | 0x03 | 0x06 | HEX |
| 0x0100 | VehicleSpeed | 2 | No | 0x01 | 0x07 | UINT16 |

#### SecurityConfig - 安全访问配置

配置安全级别（27服务）：

| Level | Name | SeedLength | KeyLength | MaxAttempts | DelayTime | Algorithm |
|-------|------|------------|-----------|-------------|-----------|-----------|
| 1 | Level1_Unlock | 4 | 4 | 3 | 10000 | XOR_Seed |
| 2 | Level2_Unlock | 4 | 4 | 3 | 10000 | AES128 |

#### RoutineConfig - 例行程序配置

配置例行程序（31服务）：

| RID(Hex) | Name | StartRoutine | StopRoutine | GetResult | SecurityLevel | SessionMask |
|----------|------|--------------|-------------|-----------|---------------|-------------|
| 0xFF01 | EraseMemory | Yes | No | Yes | 0x02 | 0x02 |
| 0xFF02 | CheckProgramming | Yes | No | Yes | 0x02 | 0x02 |

#### DTCConfig - 故障码配置

配置DTC（19服务）：

| DTC(Hex) | Name | Severity | FaultType | AgingCounter | Snapshot | ExtendedData |
|----------|------|----------|-----------|--------------|----------|--------------|
| 0x0100 | MAF_Circuit | 2 | Circuit | 40 | Yes | Yes |
| 0x0300 | Misfire_Detect | 3 | Plausibility | 40 | Yes | Yes |

#### Communication - 通信配置

配置CAN ID和PDU：

| Parameter | Value | Unit | Description |
|-----------|-------|------|-------------|
| RequestID_Functional | 0x7DF | Hex | 功能请求ID |
| RequestID_Physical | 0x735 | Hex | 物理请求ID |
| ResponseID | 0x73D | Hex | 响应ID |
| TxPduId | 20 | Dec | 发送PDU ID |
| RxPduId_Functional | 10 | Dec | 功能接收PDU ID |
| RxPduId_Physical | 11 | Dec | 物理接收PDU ID |

### 3. 生成代码

```bash
./generate_uds.sh UDS_Config_Example.xlsx
```

生成的文件：
- `UdsConfig_Generated.h` - 头文件（宏定义、函数声明）
- `UdsConfig_Generated.c` - 实现文件（数据表、函数实现）
- `INTEGRATION.md` - 集成指南

### 4. 集成到项目

将生成的代码复制到项目目录：

```bash
cp generated/* $PROJECT/AUTOSAR/UdsConfig/
```

在 `AUTOSAR_Cfg.h` 中包含生成的配置：

```c
#include "UdsConfig_Generated.h"

/* DCM配置使用生成的表 */
#define DCM_NUM_DIDS    (sizeof(Uds_DidTable)/sizeof(Uds_DidTable[0]))
#define DCM_NUM_DTCS    (sizeof(Uds_DtcTable)/sizeof(Uds_DtcTable[0]))
```

## 文件结构

```
tools/uds_generator/
├── README.md                    # 本文件
├── uds_generator.py             # 代码生成器
├── generate_uds.sh              # 一键生成脚本
├── UDS_Config_Template.xlsx     # Excel模板（运行--example生成）
└── example_output/              # 示例输出
    ├── UdsConfig_Generated.h
    ├── UdsConfig_Generated.c
    └── INTEGRATION.md
```

## 依赖安装

```bash
# Ubuntu/Debian
sudo apt-get install python3 python3-pip
pip3 install openpyxl

# macOS
brew install python3
pip3 install openpyxl
```

## 高级用法

### 自定义输出目录

```bash
./generate_uds.sh MyConfig.xlsx /path/to/output
```

### Python直接调用

```bash
python3 uds_generator.py MyConfig.xlsx output_dir
```

### 创建新配置模板

```bash
python3 uds_generator.py --example
```

## 生成代码示例

### 头文件片段

```c
/* Session Configuration */
#define UDS_SESSION_DEFAULTSESSION     0x01U
#define UDS_SESSION_PROGRAMMINGSESSION 0x02U
#define UDS_SESSION_EXTENDEDSESSION    0x03U

/* DID Definitions */
#define UDS_DID_VIN                    0xF190U  /* 车辆识别号 */
#define UDS_DID_ECU_NAME               0xF197U  /* ECU名称 */
#define UDS_DID_SYSTEMSTATE            0xF100U  /* 系统状态 */

/* Communication */
#define UDS_CAN_REQUEST_FUNC_ID        0x007DFU
#define UDS_CAN_RESPONSE_ID            0x0073DU
```

### 源文件片段

```c
/* DID Data Storage */
static uint8 Uds_DidData_VIN[17] = (uint8*)"123456789ABCDEFGH";
static uint8 Uds_DidData_ECU_Name[16] = (uint8*)"STM32F407_CAN   ";
static uint8 Uds_DidData_SystemState[4] = {0x00, 0x00, 0x00, 0x00};

/* DID Configuration Table */
static const Uds_DidConfigType Uds_DidTable[] = {
    { /* 0xF190 - 车辆识别号 */
        .did = 0xF190,
        .dataPtr = Uds_DidData_VIN,
        .dataLen = 17,
        .readOnly = True,
        .securityLevel = 0x01,
        .sessionMask = 0x07
    },
    /* ... more DIDs ... */
};
```

## 时间参数说明

| 参数 | 说明 | 典型值 |
|------|------|--------|
| P2_max | 服务器响应超时时间 | 50ms |
| P2*_max | 服务器扩展响应超时 | 5000ms |
| S3_client | 客户端保持会话间隔 | 2000ms |
| S3_server | 服务器会话超时 | 5000ms |

## 故障排除

### 问题：ImportError: No module named 'openpyxl'

```bash
pip3 install openpyxl
```

### 问题：生成的代码编译错误

检查Excel中的数值格式：
- 十六进制值使用 `0x` 前缀（如 `0xF190`）
- 布尔值使用 `Yes`/`No`
- 数值列不要包含文本

### 问题：DID读取失败

检查：
1. `SecurityLevel` 是否匹配当前安全级别
2. `SessionMask` 是否支持当前会话
3. `DataLength` 是否与数据匹配

## 更新日志

### v1.0.0 (2026-03-15)
- ✅ 支持Session配置（10服务）
- ✅ 支持DID配置（22/2E服务）
- ✅ 支持Security配置（27服务）
- ✅ 支持Routine配置（31服务）
- ✅ 支持DTC配置（19服务）
- ✅ 一键生成脚本
- ✅ 完整集成指南
