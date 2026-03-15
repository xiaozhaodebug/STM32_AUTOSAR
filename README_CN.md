# STM32F407 AUTOSAR CAN Demo

中文 | [English](README.md)

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)
[![Platform: STM32](https://img.shields.io/badge/Platform-STM32F407-blue.svg)](https://www.st.com/en/microcontrollers-microprocessors/stm32f407.html)
[![Standard: AUTOSAR](https://img.shields.io/badge/Standard-AUTOSAR-green.svg)](https://www.autosar.org/)

一个基于 STM32F407 的 AUTOSAR 通信栈演示项目，支持 CAN 通信和 UDS 诊断服务。

## 功能特性

- **AUTOSAR 协议栈**：集成 CanIf, PduR, Com, Dcm, EcuM 模块
- **CAN 通信**：支持标准帧和扩展帧，波特率 500Kbps
- **DBC 代码生成器**：从 Excel 自动生成 CAN 信号解析/打包代码
- **UDS 诊断**：支持 0x10, 0x11, 0x22, 0x2E, 0x27, 0x3E 服务
- **串口调试**：USART1 115200bps，带完整调试日志
- **LED 控制**：支持 3 个 LED 状态指示

## 🎬 演示视频

观看项目演示视频，了解 AUTOSAR CAN 通信和 UDS 诊断的实际运行效果！

<p align="center">
  <a href="docs/videos/demo.mp4">
    <img src="docs/images/qrcode.jpg" alt="演示视频" width="400">
  </a>
  <br>
  <b>📹 点击观看演示视频</b>
</p>

**视频内容包括：**
- 🔌 硬件连接和配置
- 📡 CAN 消息收发演示
- 🔍 UDS 诊断服务演示
- 🛠️ DBC 代码生成工作流
- 📊 串口实时调试输出

> **注意：** 演示视频 (`docs/videos/demo.mp4`) 已包含在仓库中。你也可以从 [Releases](https://github.com/xiaozhaodebug/STM32_AUTOSAR/releases) 页面下载。

## 硬件平台

### 主控芯片
- **MCU**: STM32F407ZGT6 (Cortex-M4, 168MHz, 1MB Flash, 128KB RAM)
- **外部晶振**: 8MHz HSE
- **系统时钟**: 168MHz

### 外设列表

| 外设类型 | 具体配置 | 引脚分配 | 时钟源 |
|---------|---------|---------|--------|
| **CAN** | CAN1, 500Kbps | PA11(RX), PA12(TX) | APB1 (42MHz) |
| **USART** | USART1, 115200bps | PA9(TX), PA10(RX) | APB2 (84MHz) |
| **GPIO** | 3个LED输出 | PE3, PE4, PG9 | AHB (168MHz) |
| **Timer** | SysTick | 内核定时器 | HCLK (168MHz) |

### 硬件连接图

```
                    8MHz HSE
                       │
                       ▼
┌─────────┐      ┌─────────┐      ┌─────────┐
│         │      │         │      │         │
│STM32F407│◄────►│   CAN   │◄────►│ PCAN-USB│
│  PA11   │      │ 收发器  │      │         │
│  PA12   │      │         │      │         │
│         │      └─────────┘      └─────────┘
│  PA9    │
│  PA10   │◄────► USB转串口模块
│         │
│  PE3    │◄────► LED0 (运行指示)
│  PE4    │◄────► LED1 (预留)
│  PG9    │◄────► LED2 (心跳指示)
│         │
└─────────┘
```

详细硬件规格请参考 [HARDWARE_SPEC.md](HARDWARE_SPEC.md)

## 项目结构

```
STM32_AUTOSAR/
├── AUTOSAR/              # AUTOSAR 协议栈
│   ├── CanIf/           # CAN 接口层
│   ├── PduR/            # PDU 路由
│   ├── Com/             # 通信服务
│   ├── Dcm/             # 诊断通信管理
│   ├── EcuM/            # ECU 状态管理
│   ├── Std/             # 标准类型定义
│   └── AUTOSAR_Cfg.h    # 配置文件
├── src/
│   ├── board/           # 板级支持
│   │   ├── main.c       # 主程序
│   │   ├── system_stm32f4xx.c
│   │   └── startup_stm32f407xx.s
│   ├── drivers/         # 设备驱动
│   │   └── CanDriver.c  # CAN 驱动
│   └── utils/           # 工具函数
│       └── DebugLog.c   # 调试日志
├── include/             # 头文件
├── ld/                  # 链接脚本
├── tools/               # 工具脚本
├── docs/                # 文档
├── examples/            # 示例代码
└── tests/               # 测试脚本
```

## 开发环境搭建

### 1. 编译工具链搭建

#### 1.1 安装 GCC ARM Embedded Toolchain

**Ubuntu/Debian:**
```bash
sudo apt update
sudo apt install gcc-arm-none-eabi binutils-arm-none-eabi
```

**验证安装:**
```bash
arm-none-eabi-gcc --version
# 应显示版本号，如: gcc version 13.2.1
```

**macOS:**
```bash
brew install --cask gcc-arm-embedded
```

**Windows:**
1. 下载 [GCC ARM Embedded](https://developer.arm.com/downloads/-/gnu-rm)
2. 解压到 `C:\gcc-arm-none-eabi`
3. 添加到系统 PATH 环境变量

#### 1.2 安装 CMake

**Ubuntu/Debian:**
```bash
sudo apt install cmake
```

**macOS:**
```bash
brew install cmake
```

**验证:**
```bash
cmake --version
# 要求 >= 3.16
```

#### 1.3 安装构建工具

```bash
sudo apt install build-essential ninja-build
```

---

### 2. 调试工具链搭建

#### 2.1 安装 OpenOCD

**Ubuntu/Debian:**
```bash
sudo apt install openocd
```

**验证:**
```bash
openocd --version
# 应显示: Open On-Chip Debugger 0.12.0
```

#### 2.2 配置 J-Link 调试器

**安装 J-Link 软件包:**

1. 从 [SEGGER 官网](https://www.segger.com/downloads/jlink/) 下载 J-Link 软件包
2. 安装:
```bash
# Ubuntu/Debian (x86_64)
cd /tmp
wget https://www.segger.com/downloads/jlink/JLink_Linux_V796a_x86_64.deb
sudo dpkg -i JLink_Linux_V796a_x86_64.deb
sudo apt-get install -f  # 修复依赖
```

3. 配置 udev 规则:
```bash
# 创建 udev 规则
sudo tee /etc/udev/rules.d/99-jlink.rules << 'EOF'
SUBSYSTEM=="usb", ATTR{idVendor}=="1366", MODE="0666", GROUP="dialout"
EOF

# 重新加载规则
sudo udevadm control --reload-rules
sudo udevadm trigger

# 将用户添加到 dialout 组
sudo usermod -a -G dialout $USER
# 重新登录后生效
```

4. 验证 J-Link:
```bash
JLinkExe -version
```

#### 2.3 使用 ST-Link 调试器 (替代方案)

**安装 stlink 工具:**
```bash
sudo apt install stlink-tools
```

**修改 CMakeLists.txt 使用 ST-Link:**
```bash
# 在 CMakeLists.txt 中修改 INTERFACE_CFG
set(INTERFACE_CFG "interface/stlink.cfg")
```

---

### 3. CAN 上位机工具搭建

#### 3.1 安装 CAN 工具集 (Linux)

```bash
sudo apt install can-utils
```

**主要工具说明:**

| 工具 | 功能 |
|------|------|
| `candump` | 监听 CAN 总线 |
| `cansend` | 发送 CAN 帧 |
| `cansniffer` | CAN 数据嗅探 |
| `canplayer` | 回放 CAN 日志 |
| `cangen` | 生成 CAN 测试数据 |

#### 3.2 配置 PCAN-USB 设备

**1. 安装驱动和工具:**
```bash
# 安装 PCAN 驱动
sudo apt install pcanview

# 或使用通用 SocketCAN 驱动 (推荐)
# Linux 内核已内置支持
```

**2. 加载 CAN 驱动:**
```bash
# 加载 CAN 设备驱动
sudo modprobe can
sudo modprobe can_raw
sudo modprobe peak_usb  # PCAN-USB 驱动

# 验证设备
lsusb | grep -i peak
# 显示: Bus 001 Device 0xx: ID 0c72:000c PEAK System PCAN-USB
```

**3. 启动 CAN 接口:**
```bash
# 创建 CAN 接口
sudo ip link add dev can0 type can bitrate 500000

# 启动接口
sudo ip link set can0 up

# 验证状态
ip link show can0
# 应显示: <UP,RUNNING>
```

#### 3.3 常用 CAN 操作命令

```bash
# 监听所有 CAN 帧
candump can0

# 监听特定 ID
candump can0,123:7FF

# 发送标准帧
cansend can0 123#1122334455667788

# 发送扩展帧
cansend can0 18DA00F1#0227010000000000

# 发送 RTR 帧
cansend can0 123#R

# 数据嗅探 (显示变化的数据)
cansniffer can0

# 生成测试数据 (每秒 100 帧)
cangen can0 -g 10 -I 123 -D 1122334455667788 -L 8
```

#### 3.4 Windows CAN 工具

**推荐工具:**
- [PCAN-View](https://www.peak-system.com/PCAN-View.242.0.html) - PEAK 官方工具
- [BusMaster](https://rbei-etas.github.io/busmaster/) - 开源 CAN 分析工具
- [CANalyzer](https://www.vector.com/canalyzer.html) - Vector 专业工具 (商业)

**PCAN-View 使用:**
1. 安装 PCAN 驱动
2. 连接 PCAN-USB 设备
3. 打开 PCAN-View，选择 500Kbps 波特率
4. 点击 Connect 开始通信

---

### 4. 串口调试工具

#### 4.1 Linux 串口工具

```bash
# 安装 minicom
sudo apt install minicom

# 使用 minicom
sudo minicom -D /dev/ttyACM0 -b 115200

# 或使用 screen
sudo apt install screen
sudo screen /dev/ttyACM0 115200

# 或使用 picocom
sudo apt install picocom
sudo picocom -b 115200 /dev/ttyACM0
```

#### 4.2 串口权限配置

```bash
# 将用户添加到 dialout 组
sudo usermod -a -G dialout $USER

# 重新登录后生效
# 验证
groups
# 应显示 dialout
```

---

## 快速开始

### 一键配置 (DBC 代码生成)

如果你修改了 DBC Excel 文件，使用一键配置脚本自动生成代码并编译：

```bash
cd tools

# 一键生成代码、编译、烧录
./generate_dbc.sh
```

详细说明请参考 [tools/README_DBC_GENERATOR.md](tools/README_DBC_GENERATOR.md)

### 编译

```bash
mkdir build && cd build
cmake -DENABLE_AUTOSAR=ON ..
make -j4
```

### 烧录

```bash
# 使用 J-Link
make flash

# 或使用 tools 目录下的脚本
../tools/flash.sh elf
```

### 调试

```bash
# 启动 OpenOCD 调试服务器
make debug-server

# 另一个终端启动 GDB
arm-none-eabi-gdb STM32F407_CAN.elf
(gdb) target remote localhost:3333
(gdb) load
(gdb) continue
```

## CAN 通信测试

### 启动 CAN 接口

```bash
# 1. 连接 PCAN-USB 设备，检查是否识别
lsusb | grep -i peak
# 输出: Bus 001 Device 0xx: ID 0c72:000c PEAK System PCAN-USB

# 2. 启动 CAN 接口
sudo ip link set can0 up type can bitrate 500000

# 3. 验证状态
ip link show can0
# 应显示: <UP,RUNNING>
```

### 发送测试帧

```bash
# 发送标准帧 (ID=0x123, 8字节数据)
cansend can0 123#1122334455667788

# UDS 诊断会话控制 (进入默认会话)
cansend can0 7E0#0210010000000000

# Tester Present (保持会话)
cansend can0 7E0#023E800000000000

# 读取 VIN (DID 0xF190)
cansend can0 7E0#0322F19000000000
```

### 监听 CAN 总线

```bash
# 监听所有帧
candump can0

# 监听特定 ID (只显示 ID=0x123)
candump can0,123:7FF

# 监听多个 ID (0x123 或 0x7E0)
candump can0,123:7FF,7E0:7FF

# 带时间戳显示
candump -t A can0

# 保存到文件
candump -l can0
# 生成 can0.log 文件
```

### 使用 PCAN-View (Windows)

1. **安装驱动**: 从 [PEAK 官网](https://www.peak-system.com/quick/DrvSetup) 下载驱动
2. **连接设备**: 插入 PCAN-USB，等待驱动安装完成
3. **配置参数**:
   - 打开 PCAN-View
   - 选择接口: PCAN-USB
   - 波特率: 500 kBit/s
   - 点击 "Connect"
4. **发送数据**:
   - 在 "Transmit" 窗口点击 "New Message"
   - 输入 ID: 123 (Hex)
   - 输入数据: 11 22 33 44 55 66 77 88
   - 点击 "Send"

### 查看串口日志

```bash
# 1. 查找串口设备
ls /dev/ttyACM*
# 输出: /dev/ttyACM0

# 2. 配置串口参数
stty -F /dev/ttyACM0 115200 cs8 -cstopb -parenb raw

# 3. 查看日志
cat /dev/ttyACM0

# 或使用 minicom
sudo minicom -D /dev/ttyACM0 -b 115200
```

**预期串口输出:**
```
================================
STM32F407 AUTOSAR CAN Demo
Hardware: CAN1(PA11/PA12), USART1(PA9/PA10)
LED: PE3, PE4, PG9 (Low Active)
================================
[ECUM] DriverInitTwo start...
[ECUM] Init CanIf...
[ECUM] CanIf init OK
...
[MAIN] Running, tick=5001
[CANIF] RX PDU ID=0 Len=8 Data=11 22 33 44 55 66 77 88
```

## 支持的 UDS 服务

| SID | 服务名称 | 功能描述 |
|-----|---------|---------|
| 0x10 | 诊断会话控制 | 切换诊断会话模式 |
| 0x11 | ECU 复位 | 复位 ECU |
| 0x22 | 读取 DID | 读取数据标识符 |
| 0x2E | 写入 DID | 写入数据标识符 |
| 0x27 | 安全访问 | 解锁安全等级 |
| 0x3E | Tester Present | 保持会话活跃 |

## 配置说明

### CAN ID 配置

在 `AUTOSAR/AUTOSAR_Cfg.h` 中配置 CAN ID：

```c
/* 发送 PDU */
#define CAN_TX_ID_TEST      0x123
#define CAN_TX_ID_OBD       0x7DF
#define CAN_TX_ID_UDS_RESP  0x18DAF100

/* 接收 PDU */
#define CAN_RX_ID_TEST      0x123
#define CAN_RX_ID_UDS_REQ   0x7E0
#define CAN_RX_ID_UDS_EXT   0x18DA00F1
```

### 波特率配置

默认 CAN 波特率为 500Kbps，计算公式：

```
波特率 = APB1时钟 / ((BS1 + BS2 + 1) * 预分频)
500Kbps = 42MHz / ((11 + 2 + 1) * 6) = 42MHz / 84
采样点 = (1 + BS1) / (1 + BS1 + BS2) = 85.7%
```

## 调试功能

项目集成了完整的调试日志系统，支持以下输出：

- 系统启动日志
- AUTOSAR 模块初始化日志
- CAN 发送/接收日志
- UDS 诊断服务日志
- 系统运行状态日志

通过串口以 115200bps 波特率输出。

## 测试

运行自动化测试脚本：

```bash
./tools/test_all.sh
```

## 贡献

欢迎提交 Issue 和 Pull Request！

请阅读 [CONTRIBUTING.md](CONTRIBUTING.md) 了解贡献指南。

## 许可证

本项目采用 MIT 许可证，详见 [LICENSE](LICENSE) 文件。

## 致谢

- [AUTOSAR](https://www.autosar.org/) - 汽车软件架构标准
- [STMicroelectronics](https://www.st.com/) - STM32 微控制器
- [EasyXMen](https://github.com/easyxmen) - AUTOSAR 协议栈参考

## 联系方式

如有问题或建议，欢迎通过以下方式联系：

- 提交 GitHub Issue
- 发送邮件至：xiaozhaodebug@gmail.com

## 故障排除

### 编译问题

**Q: `arm-none-eabi-gcc: command not found`**
```bash
# 检查是否正确安装
which arm-none-eabi-gcc

# 如果未找到，手动添加 PATH
export PATH=$PATH:/usr/bin
# 或重新安装
sudo apt install gcc-arm-none-eabi
```

**Q: `cannot find -lgcc` 链接错误**
```bash
# 安装 multilib 支持
sudo apt install gcc-multilib g++-multilib
```

### 烧录问题

**Q: `No J-Link device found`**
```bash
# 1. 检查设备连接
lsusb | grep -i segger

# 2. 检查 udev 规则
cat /etc/udev/rules.d/99-jlink.rules

# 3. 重新加载规则
sudo udevadm control --reload-rules
sudo udevadm trigger

# 4. 检查权限
ls -la /dev/bus/usb/*/* | grep -i segger
```

**Q: `Error: J-Link V9 is not supported`**
```bash
# 更新 J-Link 固件
JLinkExe -commanderscript update_fw.jlink

# 或尝试指定设备
openocd -f interface/jlink.cfg -c "jlink serial 123456789" -f target/stm32f4x.cfg
```

### CAN 通信问题

**Q: `Network is down` 错误**
```bash
# 重新启动 CAN 接口
sudo ip link set can0 down
sudo ip link set can0 up type can bitrate 500000

# 检查接口状态
ip link show can0
```

**Q: 无法发送 CAN 帧**
```bash
# 检查总线终端电阻
cat /sys/class/net/can0/termination

# 检查错误状态
cat /proc/net/can/stats
```

**Q: 收不到 CAN 数据**
```bash
# 1. 检查硬件连接
# - CAN_H 和 CAN_L 是否接反
# - 是否有 120Ω 终端电阻
# - 是否共地

# 2. 检查波特率
# 确保上位机和下位机波特率一致 (500Kbps)

# 3. 检查过滤器设置
cat /proc/net/can/rcvlist_all
```

### 串口问题

**Q: `/dev/ttyACM0: Permission denied`**
```bash
# 添加到 dialout 组
sudo usermod -a -G dialout $USER

# 临时权限
sudo chmod 666 /dev/ttyACM0
```

**Q: 串口输出乱码**
```bash
# 检查波特率设置
stty -F /dev/ttyACM0

# 重新配置
stty -F /dev/ttyACM0 115200 cs8 -cstopb -parenb raw
```

## 更新日志

详见 [CHANGELOG.md](CHANGELOG.md)

---

## 📱 关注公众号

欢迎关注我们的微信公众号，获取更多 AUTOSAR 和嵌入式开发干货！

<p align="center">
  <img src="docs/images/qrcode.jpg" alt="公众号二维码" width="200">
  <br>
  <b>扫码关注，一起学习成长！</b>
</p>

### 公众号内容

- 🔧 **AUTOSAR 实战教程** - 从入门到精通
- 📡 **CAN/LIN 总线技术** - 通信协议深度解析
- 💡 **嵌入式开发技巧** - 实战经验分享
- 📰 **行业动态资讯** - 汽车电子前沿技术

---

<p align="center">
  Made with ❤️ by STM32F407 AUTOSAR Project Contributors
</p>
