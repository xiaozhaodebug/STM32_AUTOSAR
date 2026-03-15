# DBC 代码生成器使用说明

一键从 Excel DBC 文件生成 STM32 CAN 代码！

## 功能特性

- 📊 **解析 Excel DBC**: 自动读取 XZ_CAN_V2.xlsx 格式的 DBC 文件
- 🔄 **自动生成代码**: 生成 DbcConfig.h 和 DbcHandler.c
- 📝 **代码模板**: 自动生成 main.c 集成代码模板
- ⚡ **一键配置**: 通过脚本自动完成生成、编译、烧录

## 文件说明

| 文件 | 说明 |
|------|------|
| `dbc_generator.py` | Python 代码生成器核心 |
| `generate_dbc.sh` | 一键配置脚本 |
| `XZ_CAN_V2.xlsx` | DBC Excel 源文件 |

## 使用方法

### 方法 1: 一键配置 (推荐)

```bash
cd STM32_AUTOSAR/tools

# 一键生成代码、编译、烧录
./generate_dbc.sh
```

脚本会自动：
1. ✅ 解析 Excel DBC 文件
2. ✅ 生成 DbcConfig.h 和 DbcHandler.c
3. ✅ 编译工程
4. ✅ 询问是否烧录

### 方法 2: 手动分步

#### 步骤 1: 生成代码

```bash
cd STM32_AUTOSAR/tools

# 使用默认 DBC 文件
python3 dbc_generator.py --input XZ_CAN_V2.xlsx --output ../

# 或使用自定义 DBC 文件
python3 dbc_generator.py --input /path/to/your_dbc.xlsx --output ../
```

#### 步骤 2: 集成到工程

生成的文件会自动保存到：
- `include/DbcConfig.h` - DBC 配置头文件
- `src/utils/DbcHandler.c` - 信号打包/解析函数
- `dbc_main_template.txt` - main.c 代码模板

#### 步骤 3: 修改 main.c

参考 `dbc_main_template.txt`，在 main.c 中添加：

```c
#include "DbcConfig.h"

// 在主循环中添加周期发送
static uint32_t lastLedTx = 0;
if ((gTickCount - lastLedTx) >= CYCLE_TIME_XZ_A_LED_STATE) {
    lastLedTx = gTickCount;
    
    XZ_A_LedState_t ledState = {0};
    ledState.Led1_State = 1;  // ON
    ledState.Led2_State = 2;  // Blink
    
    uint8_t txData[8];
    Dbc_Pack_XZ_A_LedState(txData, &ledState);
    
    CanMessage msg = {
        .Id = MSG_ID_XZ_A_LED_STATE,
        .IsExtId = 0,
        .Dlc = 8
    };
    memcpy(msg.Data, txData, 8);
    CanDriver_Send(&msg, 10);
}

// 接收处理
switch (rxMsg.Id) {
    case MSG_ID_XZ_B_CTRL_STATE: {
        XZ_B_CtrlState_t ctrl;
        Dbc_Parse_XZ_B_CtrlState(rxMsg.Data, &ctrl);
        // 处理控制信号...
        break;
    }
}
```

#### 步骤 4: 编译烧录

```bash
cd build
cmake ..
make -j4
make flash
```

## Excel DBC 格式规范

### Sheet 名称
必须是 `Sheet1`

### 列定义

| 列索引 | 名称 | 说明 | 示例 |
|--------|------|------|------|
| 0 | Message ID | 消息 ID (Hex) | 0x210 |
| 1 | Message Name | 消息名称 | XZ_A_Led_State |
| 2 | Message Type | 标准/扩展帧 | CAN_Standard |
| 3 | Message Length | DLC (字节) | 8 |
| 4 | Message Send Type | 发送类型 | Cyclic / Event |
| 5 | Cycle Time(ms) | 周期 (ms) | 50 |
| 6 | Signal Name | 信号名称 | Led1_State |
| 7 | Byte Order | 字节序 | intel |
| 8 | Signal Size(bit) | 信号长度 (bit) | 2 |
| 9 | Start Bit(LSB) | 起始位 | 0 |
| 10 | Factor | 精度 | 1 |
| 11 | Offset | 偏移 | 0 |
| 12 | Init Value | 初始值 | 0x0 |
| 13 | Minimum | 最小值 | 0 |
| 14 | Maximum | 最大值 | 3 |
| 15 | Sender | 发送节点 | XZ_A |
| 16 | Receiver | 接收节点 | XZ_B |
| 17 | Signal Group | 信号组 | Sig_Grp_0x210 |
| 18 | Comment | 注释 | LED状态 |
| 19 | Unit | 单位 |  |
| 20 | Coding | 编码定义 | 0x0:LED_ON |

### 消息行和信号行

- **消息行**: Message ID 列不为空
- **信号行**: Message ID 列为空，Signal Name 列不为空

示例：
```
Row1: 0x210 | XZ_A_Led_State | CAN_Standard | 8 | Cyclic | 50 | ...
Row2:       |                |              |   |        |    | Led1_State | intel | 2 | 0 | ...
Row3:       |                |              |   |        |    | Led2_State | intel | 2 | 2 | ...
```

## 维护 CAN 矩阵

### 添加新消息

1. 打开 `XZ_CAN_V2.xlsx`
2. 添加新的消息行和信号行
3. 运行 `./generate_dbc.sh`
4. 代码自动生成并编译

### 修改信号

1. 修改 Excel 中的信号定义
2. 运行 `./generate_dbc.sh`
3. 代码自动更新

### 删除消息

1. 从 Excel 中删除对应的行
2. 运行 `./generate_dbc.sh`

## 生成的代码结构

### DbcConfig.h

```c
// 消息 ID 定义
#define MSG_ID_XZ_A_LED_STATE   0x210

// 周期定义
#define CYCLE_TIME_XZ_A_LED_STATE   50

// 信号定义
#define SIG_XZ_A_LED_STATE_LED1_STATE_START_BIT   0
#define SIG_XZ_A_LED_STATE_LED1_STATE_SIZE        2

// 数据结构
typedef struct {
    uint8_t Led1_State;
    uint8_t Led2_State;
    uint8_t Led3_State;
    uint8_t Led4_State;
} XZ_A_LedState_t;

// 函数声明
void Dbc_Pack_XZ_A_LedState(uint8_t* data, const XZ_A_LedState_t* msg);
void Dbc_Parse_XZ_B_CtrlState(const uint8_t* data, XZ_B_CtrlState_t* msg);
```

### DbcHandler.c

```c
// 信号打包
void Dbc_Pack_XZ_A_LedState(uint8_t* data, const XZ_A_LedState_t* msg) {
    // 自动生成的打包代码
}

// 信号解析
void Dbc_Parse_XZ_B_CtrlState(const uint8_t* data, XZ_B_CtrlState_t* msg) {
    // 自动生成的解析代码
}
```

## 故障排除

### 问题 1: `ModuleNotFoundError: No module named 'openpyxl'`

```bash
pip3 install openpyxl
```

### 问题 2: 生成的代码有编译错误

检查 Excel 格式是否符合规范：
- Message ID 必须以 `0x` 开头
- Start Bit 必须是数字
- Signal Size 必须是数字

### 问题 3: TX/RX 判断错误

确保信号的 Sender/Receiver 列正确填写：
- 如果 XZ_A 发送，填写 `XZ_A`
- 如果 XZ_A 接收，Receiver 填写 `XZ_A`

## 高级用法

### 自定义代码模板

修改 `dbc_generator.py` 中的以下函数：
- `generate_config_h()` - 修改头文件模板
- `generate_handler_c()` - 修改 C 文件模板

### 集成到 CI/CD

```yaml
# .github/workflows/generate_dbc.yml
- name: Generate DBC Code
  run: |
    cd tools
    python3 dbc_generator.py --input XZ_CAN_V2.xlsx --output ../
    
- name: Build
  run: |
    cd build
    cmake ..
    make -j4
```

## 示例

### 示例 1: LED 控制

DBC 定义：
```
Msg: XZ_A_Led_State (0x210)
  Sig: Led1_State (2bit @ bit0)
  Sig: Led2_State (2bit @ bit2)
```

生成的代码：
```c
XZ_A_LedState_t led = {1, 2, 0, 0};  // Led1=ON, Led2=Blink
uint8_t data[8];
Dbc_Pack_XZ_A_LedState(data, &led);
```

### 示例 2: 车速接收

DBC 定义：
```
Msg: XZ_B_MotorState (0x123)
  Sig: CarSpeed (15bit @ bit1)
```

生成的代码：
```c
XZ_B_MotorState_t motor;
Dbc_Parse_XZ_B_MotorState(rxData, &motor);
uint16_t speed = motor.CarSpeed;  // 0-32767
```

## 贡献

欢迎提交 Issue 和 PR！

## 许可证

MIT License
