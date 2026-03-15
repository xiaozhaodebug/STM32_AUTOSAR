# STM32F407 AUTOSAR CAN Demo

[中文](README_CN.md) | English

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)
[![Platform: STM32](https://img.shields.io/badge/Platform-STM32F407-blue.svg)](https://www.st.com/en/microcontrollers-microprocessors/stm32f407.html)
[![Standard: AUTOSAR](https://img.shields.io/badge/Standard-AUTOSAR-green.svg)](https://www.autosar.org/)

An AUTOSAR communication stack demonstration project based on STM32F407, supporting CAN communication and UDS diagnostic services.

## 📋 Table of Contents

- [Features](#-features)
- [Hardware Platform](#-hardware-platform)
- [Quick Start](#-quick-start)
- [Project Structure](#-project-structure)
- [Build Instructions](#-build-instructions)
- [CAN Tools Setup](#-can-tools-setup)
- [UDS Diagnostics](#-uds-diagnostics)
- [Code Generators](#-code-generators)
- [Troubleshooting](#-troubleshooting)
- [Follow Us](#-follow-us)

---

## ✨ Features

- **AUTOSAR Stack**: Integrated CanIf, PduR, Com, Dcm, EcuM modules
- **CAN Communication**: Support standard and extended frames, 500Kbps baudrate
- **DBC Code Generator**: Auto-generate CAN signal packing/unpacking code from Excel
- **UDS Diagnostics**: Support 0x10, 0x11, 0x22, 0x2E, 0x27, 0x3E services
- **Serial Debug**: USART1 115200bps with full debug logging
- **LED Control**: Support 3 LED status indicators

---

## 🎬 Demo Video

Watch the project demonstration video to see the AUTOSAR CAN communication and UDS diagnostics in action!

<p align="center">
  <a href="docs/videos/demo.mp4">
    <img src="docs/images/qrcode.jpg" alt="Demo Video" width="400">
  </a>
  <br>
  <b>📹 Click to view demo video</b>
</p>

**Video Contents:**
- 🔌 Hardware connection and setup
- 📡 CAN message transmission and reception
- 🔍 UDS diagnostic service demonstration
- 🛠️ DBC code generation workflow
- 📊 Real-time debugging with serial output

> **Note:** The demo video (`docs/videos/demo.mp4`) is included in the repository. You can also download it from the [Releases](https://github.com/xiaozhaodebug/STM32_AUTOSAR/releases) page.

---

## 🔧 Hardware Platform

### Main Controller

| Parameter | Specification |
|-----------|---------------|
| **MCU** | STM32F407ZGT6 (Cortex-M4, 168MHz, 1MB Flash, 192KB RAM) |
| **External Crystal** | 8MHz HSE |
| **System Clock** | 168MHz |

### Peripheral List

| Peripheral | Configuration | Pin Assignment | Clock Source |
|------------|---------------|----------------|--------------|
| **CAN** | CAN1, 500Kbps | PA11(RX), PA12(TX) | APB1 (42MHz) |
| **USART** | USART1, 115200bps | PA9(TX), PA10(RX) | APB2 (84MHz) |
| **GPIO** | 3 LED outputs | PE3, PE4, PG9 | AHB (168MHz) |
| **Timer** | SysTick | Core timer | HCLK (168MHz) |

### Hardware Connection Diagram

```
                    8MHz HSE
                       |
                       ▼
┌─────────┐      ┌─────────┐      ┌─────────┐
│         │      │         │      │         │
│STM32F407│◄────►│   CAN   │◄────►│ PCAN-USB│
│  PA11   │      │Tranceiver│      │         │
│  PA12   │      │         │      │         │
│         │      └─────────┘      └─────────┘
│  PA9    │
│  PA10   │◄────► USB to Serial Module
│         │
│  PE3    │◄────► LED0 (Run Indicator)
│  PE4    │◄────► LED1 (Reserved)
│  PG9    │◄────► LED2 (Reserved)
└─────────┘
```

### Development Tools

| Tool | Model | Purpose |
|------|-------|---------|
| Debugger | J-Link Plus / ST-Link V2 | Program and Debug |
| CAN Interface | PCAN-USB / PEAK CAN | CAN Communication |
| Serial Tool | USB to TTL (CH340/CP2102) | Log Output |

---

## 🚀 Quick Start

### 1. Clone Repository

```bash
git clone https://github.com/your-username/STM32_AUTOSAR.git
cd STM32_AUTOSAR
```

### 2. Build Project

```bash
cd STM32_AUTOSAR
mkdir -p build && cd build
cmake ..
make -j$(nproc)
```

### 3. Flash Firmware

```bash
# Using J-Link (Default)
make flash

# Or using OpenOCD
openocd -f interface/jlink.cfg -f target/stm32f4x.cfg \
    -c "program STM32F407_CAN.elf verify reset exit"
```

### 4. Setup CAN Interface

```bash
# Load CAN drivers
sudo modprobe can
sudo modprobe can_raw
sudo modprobe peak_usb

# Create CAN interface
sudo ip link add dev can0 type can bitrate 500000
sudo ip link set can0 up

# Verify
ip link show can0
```

### 5. Monitor Communication

```bash
# Terminal 1: Monitor CAN frames
candump -ta can0

# Terminal 2: Send test frame
cansend can0 7DF#0227010000000000

# Terminal 3: View serial logs
minicom -D /dev/ttyACM0 -b 115200
```

---

## 📁 Project Structure

```
STM32_AUTOSAR/
├── src/                    # Source code
│   ├── board/             # Board support
│   │   ├── main.c         # Main entry
│   │   ├── startup_stm32f407xx.s
│   │   └── system_stm32f4xx.c
│   ├── drivers/           # Hardware drivers
│   │   └── CanDriver.c    # CAN driver
│   └── utils/             # Utilities
│       ├── DebugLog.c     # Debug logging
│       └── DbcHandler.c   # DBC handling
├── include/               # Header files
├── ld/                    # Linker scripts
├── tools/                 # Development tools
│   ├── code_generators/   # Code generators
│   │   ├── dbc_generator.py
│   │   └── uds_generator/
│   ├── build_tools/       # Build scripts
│   ├── debug_tools/       # Debug utilities
│   └── diagnostic_tools/  # Diagnostic tools
├── AUTOSAR/               # AUTOSAR modules
│   ├── Std/              # Standard types
│   ├── CanIf/            # CAN Interface
│   ├── PduR/             # PDU Router
│   ├── Com/              # Communication
│   ├── Dcm/              # Diagnostic
│   ├── EcuM/             # ECU Manager
│   └── UdsConfig/        # UDS configuration
├── docs/                  # Documentation
│   └── images/           # Images
├── CMakeLists.txt         # CMake configuration
├── LICENSE               # MIT License
└── README.md             # This file
```

---

## 🛠️ Build Instructions

### Prerequisites

| Tool | Version | Installation |
|------|---------|--------------|
| CMake | >= 3.20 | `sudo apt install cmake` |
| GCC ARM | >= 13.2 | `sudo apt install gcc-arm-none-eabi` |
| OpenOCD | >= 0.12.0 | `sudo apt install openocd` |
| Python | >= 3.6 | `sudo apt install python3 python3-pip` |
| openpyxl | - | `pip3 install openpyxl` |

### Build Steps

```bash
# Generate build files
cd STM32_AUTOSAR
mkdir -p build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Debug

# Build
make -j$(nproc)

# Output files:
# - STM32F407_CAN.elf  (with debug info)
# - STM32F407_CAN.hex  (Intel Hex)
# - STM32F407_CAN.bin  (Binary)
```

### Build Types

```bash
# Debug build (default)
cmake .. -DCMAKE_BUILD_TYPE=Debug

# Release build
cmake .. -DCMAKE_BUILD_TYPE=Release
```

### Custom Build Options

```bash
# Disable AUTOSAR stack
cmake .. -DENABLE_AUTOSAR=OFF

# Use ST-Link instead of J-Link
cmake .. -DINTERFACE_CFG="interface/stlink.cfg"
```

---

## 📡 CAN Tools Setup

### Linux (Recommended)

```bash
# Install CAN utilities
sudo apt install can-utils

# Load drivers
sudo modprobe can can_raw peak_usb

# Setup interface
sudo ip link add dev can0 type can bitrate 500000
sudo ip link set can0 up
```

### Common Commands

```bash
# Monitor all frames
candump can0

# Monitor specific ID
candump can0,123:7FF

# Send standard frame
cansend can0 123#1122334455667788

# Send extended frame (UDS request)
cansend can0 7DF#0227010000000000

# Send diagnostic request
cansend can0 735#0322F19000000000

# Data sniffing (show changed data)
cansniffer can0
```

### Windows Tools

- [PCAN-View](https://www.peak-system.com/PCAN-View.242.0.html) - PEAK official tool
- [BusMaster](https://rbei-etas.github.io/busmaster/) - Open source CAN analyzer
- [CANalyzer](https://www.vector.com/canalyzer.html) - Vector professional tool (commercial)

---

## 🔍 UDS Diagnostics

### Supported Services

| SID | Service | Description |
|-----|---------|-------------|
| 0x10 | DiagnosticSessionControl | Switch diagnostic sessions |
| 0x11 | ECUReset | Reset ECU |
| 0x22 | ReadDataByIdentifier | Read DID data |
| 0x2E | WriteDataByIdentifier | Write DID data |
| 0x27 | SecurityAccess | Security access |
| 0x3E | TesterPresent | Keep session alive |

### Test Examples

```bash
# 0x3E - TesterPresent
cansend can0 7DF#023E800000000000
# Response: 7E 00

# 0x10 - Default Session
cansend can0 7DF#0210010000000000
# Response: 50 01 00 32 13 88

# 0x22 - Read VIN (0xF190)
cansend can0 7DF#0322F19000000000
# Response: 62 F1 90 31 32 33...

# 0x22 - Read ECU Name (0xF197)
cansend can0 735#0322F19700000000
# Response: 62 F1 97 53 54 4D...
```

### Diagnostic IDs (DBC Matrix)

| ID | Name | Direction | Description |
|----|------|-----------|-------------|
| 0x7DF | Meg_FunDiag | RX | Functional diagnostic request |
| 0x735 | Meg_PhyDiag | RX | Physical diagnostic request |
| 0x73D | Meg_ResDiag | TX | Diagnostic response |

### Automated Testing

```bash
# Run diagnostic test suite
cd tools/diagnostic_tools
python3 diag_test.py can0
```

---

## 🎨 Code Generators

### DBC Generator

Generate C code from Excel DBC configuration:

```bash
cd tools/code_generators
./generate_dbc.sh
```

**Input:** `tools/config/XZ_CAN_V2.xlsx`

**Output:**
- `include/DbcConfig.h`
- `src/utils/DbcHandler.c`

### UDS Configuration Generator

Generate UDS diagnostic configuration from Excel:

```bash
cd tools/code_generators/uds_generator

# Create example config
./generate_uds.sh --example

# Edit UDS_Config_Example.xlsx, then generate
./generate_uds.sh UDS_Config_Example.xlsx
```

**Configuration Sheets:**
- **SessionConfig**: Session parameters (0x10 service)
- **DIDConfig**: Data identifiers (0x22/0x2E services)
- **SecurityConfig**: Security levels (0x27 service)
- **RoutineConfig**: Routines (0x31 service)
- **DTCConfig**: DTC definitions (0x19 service)

---

## 🔧 Troubleshooting

### CAN Issues

**Q: `can0: No such device`**
```bash
# Check if drivers are loaded
lsmod | grep can

# Reload drivers
sudo modprobe -r peak_usb
sudo modprobe peak_usb

# Verify USB device
lsusb | grep -i peak
```

**Q: CAN frames not received**
```bash
# Check interface status
ip -details link show can0

# Verify bitrate matches
# Hardware: 500Kbps, Software: 500Kbps

# Check statistics
ip -s link show can0
```

### Flash Issues

**Q: `Error: J-Link not found`**
```bash
# Check USB connection
lsusb | grep -i jlink

# Check permissions
sudo usermod -a -G dialout $USER

# Or use ST-Link
cmake .. -DINTERFACE_CFG="interface/stlink.cfg"
```

### Serial Issues

**Q: `/dev/ttyACM0: Permission denied`**
```bash
# Add user to dialout group
sudo usermod -a -G dialout $USER

# Temporary fix
sudo chmod 666 /dev/ttyACM0
```

**Q: Garbled serial output**
```bash
# Check baudrate
stty -F /dev/ttyACM0

# Reconfigure
stty -F /dev/ttyACM0 115200 cs8 -cstopb -parenb raw
```

---

## 📜 License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

---

## 🤝 Contributing

Contributions are welcome! Please read [CONTRIBUTING.md](CONTRIBUTING.md) for details on our code of conduct and the process for submitting pull requests.

---

## 📱 Follow Us

Follow our WeChat Official Account for more AUTOSAR and embedded development content!

<p align="center">
  <img src="docs/images/qrcode.jpg" alt="WeChat QR Code" width="200">
  <br>
  <b>Scan to follow and learn together!</b>
</p>

### Content Preview

- 🔧 **AUTOSAR Tutorials** - From beginner to advanced
- 📡 **CAN/LIN Bus Technology** - Protocol deep dive
- 💡 **Embedded Tips** - Practical experience sharing
- 📰 **Industry News** - Automotive electronics trends

---

<p align="center">
  Made with ❤️ by STM32F407 AUTOSAR Project Contributors
</p>
