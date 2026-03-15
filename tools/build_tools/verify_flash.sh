#!/bin/bash
################################################################################
# @file        verify_flash.sh
# @brief       STM32F407 AUTOSAR固件烧录验证脚本
# @details     用于验证固件是否正确烧录和运行的自动化脚本
#
# @usage       ./verify_flash.sh [jlink|stlink|openocd]
# @example     ./verify_flash.sh jlink
#
# @author      [小昭debug]
# @date        2026-03-15
################################################################################

set -e

# 颜色定义
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# 配置
DEVICE="STM32F407ZG"
INTERFACE="SWD"
SPEED="4000"
BUILD_DIR="../build"
HEX_FILE="${BUILD_DIR}/STM32F407_CAN.hex"
BIN_FILE="${BUILD_DIR}/STM32F407_CAN.bin"
ELF_FILE="${BUILD_DIR}/STM32F407_CAN.elf"

# 统计
TESTS_PASSED=0
TESTS_FAILED=0

################################################################################
# 工具函数
################################################################################

print_header() {
    echo -e "${BLUE}========================================${NC}"
    echo -e "${BLUE}$1${NC}"
    echo -e "${BLUE}========================================${NC}"
}

print_info() {
    echo -e "${BLUE}[INFO]${NC} $1"
}

print_success() {
    echo -e "${GREEN}[PASS]${NC} $1"
    ((TESTS_PASSED++))
}

print_error() {
    echo -e "${RED}[FAIL]${NC} $1"
    ((TESTS_FAILED++))
}

print_warning() {
    echo -e "${YELLOW}[WARN]${NC} $1"
}

################################################################################
# 检查函数
################################################################################

check_files() {
    print_header "1. 固件文件检查"
    
    local all_exist=true
    
    if [ -f "$HEX_FILE" ]; then
        local hex_size=$(stat -c%s "$HEX_FILE" 2>/dev/null || stat -f%z "$HEX_FILE" 2>/dev/null)
        print_success "HEX文件存在: ${HEX_FILE} (${hex_size} bytes)"
    else
        print_error "HEX文件不存在: ${HEX_FILE}"
        all_exist=false
    fi
    
    if [ -f "$BIN_FILE" ]; then
        local bin_size=$(stat -c%s "$BIN_FILE" 2>/dev/null || stat -f%z "$BIN_FILE" 2>/dev/null)
        print_success "BIN文件存在: ${BIN_FILE} (${bin_size} bytes)"
    else
        print_error "BIN文件不存在: ${BIN_FILE}"
        all_exist=false
    fi
    
    if [ -f "$ELF_FILE" ]; then
        local elf_size=$(stat -c%s "$ELF_FILE" 2>/dev/null || stat -f%z "$ELF_FILE" 2>/dev/null)
        print_success "ELF文件存在: ${ELF_FILE} (${elf_size} bytes)"
    else
        print_error "ELF文件不存在: ${ELF_FILE}"
        all_exist=false
    fi
    
    $all_exist
}

check_tools() {
    print_header "2. 工具链检查"
    
    local tool=$1
    local found=false
    
    case $tool in
        jlink)
            if command -v JLinkExe &> /dev/null; then
                print_success "J-Link工具已安装"
                JLinkExe -version 2>&1 | head -1 || true
                found=true
            else
                print_error "J-Link工具未安装 (JLinkExe)"
                print_info "下载地址: https://www.segger.com/downloads/jlink"
            fi
            ;;
        stlink)
            if command -v STM32_Programmer_CLI &> /dev/null; then
                print_success "STM32CubeProgrammer已安装"
                found=true
            elif command -v st-flash &> /dev/null; then
                print_success "st-flash已安装 (开源ST-Link工具)"
                found=true
            else
                print_error "ST-Link工具未安装"
                print_info "下载地址: https://www.st.com/en/development-tools/stm32cubeprog.html"
            fi
            ;;
        openocd)
            if command -v openocd &> /dev/null; then
                print_success "OpenOCD已安装"
                openocd --version 2>&1 | head -1 || true
                found=true
            else
                print_error "OpenOCD未安装"
                print_info "安装命令: sudo apt-get install openocd"
            fi
            ;;
        *)
            print_error "未知工具: $tool"
            ;;
    esac
    
    $found
}

check_elf_info() {
    print_header "3. ELF文件分析"
    
    if [ ! -f "$ELF_FILE" ]; then
        print_error "ELF文件不存在"
        return 1
    fi
    
    # 检查文件类型
    local file_type=$(file "$ELF_FILE")
    if echo "$file_type" | grep -q "ARM.*ELF"; then
        print_success "文件类型正确: ARM ELF"
    else
        print_error "文件类型错误: $file_type"
    fi
    
    # 检查内存使用
    if command -v arm-none-eabi-size &> /dev/null; then
        print_info "内存使用情况:"
        arm-none-eabi-size "$ELF_FILE" | while read line; do
            echo "  $line"
        done
        
        # 解析输出
        local size_output=$(arm-none-eabi-size "$ELF_FILE" | tail -1)
        local text_size=$(echo "$size_output" | awk '{print $1}')
        local data_size=$(echo "$size_output" | awk '{print $2}')
        local bss_size=$(echo "$size_output" | awk '{print $3}')
        local total_size=$(echo "$size_output" | awk '{print $4}')
        
        # Flash限制 1MB = 1048576 bytes
        # RAM限制 128KB = 131072 bytes
        if [ "$total_size" -lt 1048576 ]; then
            print_success "Flash使用: ${total_size} bytes (可用: 1048576 bytes)"
        else
            print_error "Flash超出限制!"
        fi
    else
        print_warning "arm-none-eabi-size 未安装，跳过内存分析"
    fi
    
    # 检查符号表
    if command -v arm-none-eabi-readelf &> /dev/null; then
        local symbol_count=$(arm-none-eabi-readelf -s "$ELF_FILE" 2>/dev/null | wc -l)
        print_success "符号表条目数: $symbol_count"
        
        # 检查关键函数
        print_info "关键函数检查:"
        local key_functions=("main" "EcuM_Init" "CanIf_Init" "Dcm_Init" "SysTick_Handler")
        for func in "${key_functions[@]}"; do
            if arm-none-eabi-readelf -s "$ELF_FILE" 2>/dev/null | grep -q "$func"; then
                echo "  ✓ $func"
            else
                echo "  ✗ $func (缺失)"
            fi
        done
    fi
}

check_hex_validity() {
    print_header "4. HEX文件验证"
    
    if [ ! -f "$HEX_FILE" ]; then
        print_error "HEX文件不存在"
        return 1
    fi
    
    # 检查HEX格式
    if head -1 "$HEX_FILE" | grep -q '^:[0-9A-Fa-f]\{2\}'; then
        print_success "HEX格式正确 (Intel HEX)"
    else
        print_error "HEX格式错误"
        return 1
    fi
    
    # 统计记录数
    local record_count=$(wc -l < "$HEX_FILE")
    print_info "HEX记录数: $record_count"
    
    # 检查起始地址
    if grep -q ':0400000508000000' "$HEX_FILE"; then
        print_success "入口地址: 0x08000000 (正确)"
    else
        local entry=$(grep ':04000005' "$HEX_FILE" | head -1)
        if [ -n "$entry" ]; then
            print_info "入口地址记录: $entry"
        else
            print_warning "未找到入口地址记录"
        fi
    fi
    
    # 检查结束记录
    if tail -1 "$HEX_FILE" | grep -q ':00000001FF'; then
        print_success "结束记录正确"
    else
        print_warning "结束记录异常"
    fi
}

################################################################################
# 烧录函数
################################################################################

flash_jlink() {
    print_header "5. J-Link烧录"
    
    if [ ! -f "$HEX_FILE" ]; then
        print_error "HEX文件不存在"
        return 1
    fi
    
    # 创建临时脚本
    local temp_script=$(mktemp)
    cat > "$temp_script" << EOF
connect
loadfile $(realpath "$HEX_FILE")
verify
r
g
qc
EOF
    
    print_info "执行J-Link烧录..."
    if JLinkExe -device "$DEVICE" -if "$INTERFACE" -speed "$SPEED" -CommanderScript "$temp_script" 2>&1; then
        print_success "J-Link烧录成功"
        rm -f "$temp_script"
        return 0
    else
        print_error "J-Link烧录失败"
        rm -f "$temp_script"
        return 1
    fi
}

flash_stlink() {
    print_header "5. ST-Link烧录"
    
    if [ ! -f "$HEX_FILE" ]; then
        print_error "HEX文件不存在"
        return 1
    fi
    
    if command -v STM32_Programmer_CLI &> /dev/null; then
        print_info "使用STM32CubeProgrammer烧录..."
        if STM32_Programmer_CLI -c port=SWD -w $(realpath "$HEX_FILE") -v -rst 2>&1; then
            print_success "ST-Link烧录成功"
            return 0
        else
            print_error "ST-Link烧录失败"
            return 1
        fi
    elif command -v st-flash &> /dev/null; then
        print_info "使用st-flash烧录..."
        if st-flash write $(realpath "$BIN_FILE") 0x08000000 2>&1; then
            print_success "ST-Link烧录成功"
            st-flash reset
            return 0
        else
            print_error "ST-Link烧录失败"
            return 1
        fi
    else
        print_error "未找到ST-Link工具"
        return 1
    fi
}

flash_openocd() {
    print_header "5. OpenOCD烧录"
    
    if [ ! -f "$HEX_FILE" ]; then
        print_error "HEX文件不存在"
        return 1
    fi
    
    print_info "启动OpenOCD烧录..."
    
    # 创建OpenOCD脚本
    local temp_script=$(mktemp)
    cat > "$temp_script" << EOF
reset halt
flash write_image erase $(realpath "$HEX_FILE")
verify_image $(realpath "$HEX_FILE")
reset run
shutdown
EOF
    
    if openocd -f interface/stlink.cfg -f target/stm32f4x.cfg -f "$temp_script" 2>&1; then
        print_success "OpenOCD烧录成功"
        rm -f "$temp_script"
        return 0
    else
        print_error "OpenOCD烧录失败"
        rm -f "$temp_script"
        return 1
    fi
}

################################################################################
# 验证函数
################################################################################

verify_flash() {
    print_header "6. 烧录验证"
    
    print_info "请目视检查以下项目:"
    echo "  □ 绿色LED (PD12) 常亮 - 表示AUTOSAR初始化成功"
    echo "  □ 蓝色LED (PD15) 闪烁 - 表示系统运行中"
    echo "  □ 无红色LED常亮 - 表示无致命错误"
    
    print_warning "注意: 需要连接硬件才能进行实际验证"
}

################################################################################
# 主函数
################################################################################

print_usage() {
    echo "STM32F407 AUTOSAR固件烧录验证脚本"
    echo ""
    echo "用法: $0 [命令] [选项]"
    echo ""
    echo "命令:"
    echo "  check       - 仅检查固件文件和工具"
    echo "  flash       - 烧录固件 (需指定工具)"
    echo "  verify      - 验证烧录结果"
    echo "  full        - 完整流程 (检查+烧录+验证)"
    echo ""
    echo "工具 (用于flash/full命令):"
    echo "  jlink       - 使用J-Link烧录"
    echo "  stlink      - 使用ST-Link烧录"
    echo "  openocd     - 使用OpenOCD烧录"
    echo ""
    echo "示例:"
    echo "  $0 check              # 仅检查"
    echo "  $0 flash jlink        # 使用J-Link烧录"
    echo "  $0 full stlink        # 使用ST-Link完整流程"
    echo ""
}

main() {
    local command=${1:-check}
    local tool=${2:-jlink}
    
    print_header "STM32F407 AUTOSAR固件烧录验证"
    print_info "工作目录: $(pwd)"
    print_info "固件目录: $BUILD_DIR"
    echo ""
    
    case $command in
        check)
            check_files
            check_elf_info
            check_hex_validity
            check_tools "$tool"
            ;;
        flash)
            check_files || exit 1
            check_tools "$tool" || exit 1
            
            case $tool in
                jlink) flash_jlink ;;
                stlink) flash_stlink ;;
                openocd) flash_openocd ;;
                *)
                    print_error "未知工具: $tool"
                    print_usage
                    exit 1
                    ;;
            esac
            ;;
        verify)
            verify_flash
            ;;
        full)
            check_files || exit 1
            check_elf_info
            check_hex_validity
            check_tools "$tool" || exit 1
            
            case $tool in
                jlink) flash_jlink ;;
                stlink) flash_stlink ;;
                openocd) flash_openocd ;;
                *)
                    print_error "未知工具: $tool"
                    print_usage
                    exit 1
                    ;;
            esac
            
            verify_flash
            ;;
        help|--help|-h)
            print_usage
            exit 0
            ;;
        *)
            print_error "未知命令: $command"
            print_usage
            exit 1
            ;;
    esac
    
    echo ""
    print_header "验证完成"
    print_info "通过: $TESTS_PASSED, 失败: $TESTS_FAILED"
    
    if [ $TESTS_FAILED -eq 0 ]; then
        echo -e "${GREEN}✓ 所有检查通过!${NC}"
        exit 0
    else
        echo -e "${RED}✗ 存在失败的检查项${NC}"
        exit 1
    fi
}

# 运行主函数
main "$@"
