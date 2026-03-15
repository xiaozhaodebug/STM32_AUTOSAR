#!/bin/bash
# STM32F407 烧录前环境检查脚本
# Usage: ./flash_check.sh

set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(dirname "$SCRIPT_DIR")"
BUILD_DIR="${PROJECT_ROOT}/build"

# 颜色定义
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

print_header() {
    echo ""
    echo "=========================================="
    echo "$1"
    echo "=========================================="
}

print_ok() {
    echo -e "${GREEN}[✓]${NC} $1"
}

print_warn() {
    echo -e "${YELLOW}[!]${NC} $1"
}

print_error() {
    echo -e "${RED}[✗]${NC} $1"
}

print_info() {
    echo -e "${BLUE}[i]${NC} $1"
}

# ==================== 工具检查 ====================
check_tools() {
    print_header "1. 工具链检查"
    
    local all_ok=true
    
    # 检查GCC
    if command -v arm-none-eabi-gcc &> /dev/null; then
        local gcc_ver=$(arm-none-eabi-gcc --version | head -1)
        print_ok "GCC: $gcc_ver"
    else
        print_error "GCC: 未安装"
        print_info "安装命令: sudo apt install gcc-arm-none-eabi"
        all_ok=false
    fi
    
    # 检查OpenOCD
    if command -v openocd &> /dev/null; then
        local oocd_ver=$(openocd --version 2>&1 | head -1)
        print_ok "OpenOCD: $oocd_ver"
    else
        print_error "OpenOCD: 未安装"
        print_info "安装命令: sudo apt install openocd"
        all_ok=false
    fi
    
    # 检查GDB
    if command -v arm-none-eabi-gdb &> /dev/null; then
        local gdb_ver=$(arm-none-eabi-gdb --version | head -1)
        print_ok "GDB: $gdb_ver"
    else
        print_warn "GDB: 未安装 (仅烧录不需要)"
    fi
    
    # 检查CMake
    if command -v cmake &> /dev/null; then
        local cmake_ver=$(cmake --version | head -1)
        print_ok "CMake: $cmake_ver"
    else
        print_warn "CMake: 未安装"
        all_ok=false
    fi
    
    if $all_ok; then
        print_ok "工具链检查通过"
    else
        print_error "工具链检查失败，请先安装缺失工具"
        return 1
    fi
}

# ==================== 固件检查 ====================
check_firmware() {
    print_header "2. 固件检查"
    
    if [ -f "${BUILD_DIR}/STM32F407_CAN.elf" ]; then
        print_ok "ELF文件存在: ${BUILD_DIR}/STM32F407_CAN.elf"
        
        # 显示固件大小
        echo ""
        print_info "固件内存占用:"
        arm-none-eabi-size "${BUILD_DIR}/STM32F407_CAN.elf" | while read line; do
            echo "    $line"
        done
        
        # 检查BIN文件
        if [ -f "${BUILD_DIR}/STM32F407_CAN.bin" ]; then
            local bin_size=$(ls -lh "${BUILD_DIR}/STM32F407_CAN.bin" | awk '{print $5}')
            print_ok "BIN文件存在: $bin_size"
        else
            print_warn "BIN文件不存在 (可选)"
        fi
        
        # 检查HEX文件
        if [ -f "${BUILD_DIR}/STM32F407_CAN.hex" ]; then
            local hex_size=$(ls -lh "${BUILD_DIR}/STM32F407_CAN.hex" | awk '{print $5}')
            print_ok "HEX文件存在: $hex_size"
        else
            print_warn "HEX文件不存在 (可选)"
        fi
        
        return 0
    else
        print_error "ELF文件不存在: ${BUILD_DIR}/STM32F407_CAN.elf"
        print_info "请先编译项目: ./tools/build.sh"
        return 1
    fi
}

# ==================== OpenOCD配置检查 ====================
check_openocd_config() {
    print_header "3. OpenOCD配置检查"
    
    local config_ok=true
    
    # 检查J-Link配置
    if [ -f "/usr/share/openocd/scripts/interface/jlink.cfg" ]; then
        print_ok "J-Link配置: /usr/share/openocd/scripts/interface/jlink.cfg"
    else
        print_warn "J-Link配置未找到"
    fi
    
    # 检查ST-Link配置
    if [ -f "/usr/share/openocd/scripts/interface/stlink.cfg" ]; then
        print_ok "ST-Link配置: /usr/share/openocd/scripts/interface/stlink.cfg"
    else
        print_warn "ST-Link配置未找到"
    fi
    
    # 检查目标配置
    if [ -f "/usr/share/openocd/scripts/target/stm32f4x.cfg" ]; then
        print_ok "STM32F4目标配置: /usr/share/openocd/scripts/target/stm32f4x.cfg"
    else
        print_error "STM32F4目标配置未找到"
        config_ok=false
    fi
    
    if $config_ok; then
        # 测试OpenOCD能否加载配置
        print_info "测试OpenOCD配置..."
        if openocd -f interface/stlink.cfg -f target/stm32f4x.cfg -c "exit" 2>/dev/null; then
            print_ok "OpenOCD配置测试通过"
        else
            print_warn "OpenOCD配置测试失败 (可能需要连接调试器)"
        fi
    fi
    
    return 0
}

# ==================== 调试器检查 ====================
check_debugger() {
    print_header "4. 调试器连接检查"
    
    local found_debugger=false
    
    # 检查USB设备
    if command -v lsusb &> /dev/null; then
        echo "连接的USB设备:"
        
        # 检查ST-Link
        if lsusb | grep -qi "st-link"; then
            local stlink_info=$(lsusb | grep -i "st-link" | head -1)
            print_ok "发现ST-Link: $stlink_info"
            found_debugger=true
        fi
        
        # 检查J-Link
        if lsusb | grep -qi "segger\|j-link"; then
            local jlink_info=$(lsusb | grep -i "segger\|j-link" | head -1)
            print_ok "发现J-Link: $jlink_info"
            found_debugger=true
        fi
        
        # 检查CMSIS-DAP
        if lsusb | grep -qi "cmsis-dap"; then
            local dap_info=$(lsusb | grep -i "cmsis-dap" | head -1)
            print_ok "发现CMSIS-DAP: $dap_info"
            found_debugger=true
        fi
        
        if ! $found_debugger; then
            print_warn "未检测到调试器"
            print_info "请连接ST-Link、J-Link或CMSIS-DAP调试器"
            print_info "连接后可能需要安装驱动:"
            print_info "  - ST-Link: 通常无需额外驱动"
            print_info "  - J-Link: 请安装J-Link驱动包"
        fi
    fi
    
    return 0
}

# ==================== 脚本检查 ====================
check_scripts() {
    print_header "5. 项目脚本检查"
    
    local scripts_ok=true
    
    # 检查flash.sh
    if [ -f "${PROJECT_ROOT}/tools/flash.sh" ]; then
        if [ -x "${PROJECT_ROOT}/tools/flash.sh" ]; then
            print_ok "flash.sh: 存在且可执行"
        else
            print_warn "flash.sh: 存在但不可执行，正在修复..."
            chmod +x "${PROJECT_ROOT}/tools/flash.sh"
            print_ok "flash.sh: 已添加执行权限"
        fi
    else
        print_error "flash.sh: 不存在"
        scripts_ok=false
    fi
    
    # 检查debug.sh
    if [ -f "${PROJECT_ROOT}/tools/debug.sh" ]; then
        if [ -x "${PROJECT_ROOT}/tools/debug.sh" ]; then
            print_ok "debug.sh: 存在且可执行"
        else
            chmod +x "${PROJECT_ROOT}/tools/debug.sh"
            print_ok "debug.sh: 已添加执行权限"
        fi
    else
        print_warn "debug.sh: 不存在"
    fi
    
    # 检查build.sh
    if [ -f "${PROJECT_ROOT}/tools/build.sh" ]; then
        if [ -x "${PROJECT_ROOT}/tools/build.sh" ]; then
            print_ok "build.sh: 存在且可执行"
        else
            chmod +x "${PROJECT_ROOT}/tools/build.sh"
            print_ok "build.sh: 已添加执行权限"
        fi
    else
        print_warn "build.sh: 不存在"
    fi
    
    return 0
}

# ==================== 烧录前确认 ====================
pre_flash_checklist() {
    print_header "6. 烧录前确认清单"
    
    echo "请确认以下检查项:"
    echo ""
    
    local checklist=(
        "目标板已上电且连接稳定"
        "调试器已正确连接"
        "USB数据线连接牢固 (非充电线)"
        "固件已编译且是最新版本"
        "CAN总线已正确接线 (如测试CAN)"
        "已备份原有固件 (如需要)"
    )
    
    for item in "${checklist[@]}"; do
        echo "  ☐ $item"
    done
    
    echo ""
    print_info "完成上述检查后，可以执行烧录:"
    print_info "  ./tools/flash.sh elf"
}

# ==================== 主程序 ====================
main() {
    echo ""
    echo "╔════════════════════════════════════════════════╗"
    echo "║     STM32F407 烧录前环境检查脚本               ║"
    echo "╚════════════════════════════════════════════════╝"
    
    cd "$PROJECT_ROOT"
    
    local exit_code=0
    
    # 执行检查
    check_tools || exit_code=1
    check_firmware || exit_code=1
    check_openocd_config || exit_code=1
    check_debugger || true  # 调试器非必需
    check_scripts || exit_code=1
    pre_flash_checklist
    
    echo ""
    if [ $exit_code -eq 0 ]; then
        print_ok "基础检查完成，请确认烧录前清单"
    else
        print_error "部分检查未通过，请解决上述问题后再烧录"
    fi
    
    return $exit_code
}

# 运行主程序
main "$@"
