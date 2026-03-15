#!/bin/bash
# DBC 一键配置脚本
# 从 Excel 自动生成 CAN 代码并集成到工程中

set -e

# 颜色定义
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# 路径配置
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_DIR="$(dirname "$(dirname "$SCRIPT_DIR")")"
DBC_FILE="${PROJECT_DIR}/tools/config/XZ_CAN_V2.xlsx"

# 打印信息
print_info() {
    echo -e "${BLUE}[INFO]${NC} $1"
}

print_success() {
    echo -e "${GREEN}[SUCCESS]${NC} $1"
}

print_error() {
    echo -e "${RED}[ERROR]${NC} $1"
}

print_warn() {
    echo -e "${YELLOW}[WARN]${NC} $1"
}

# 检查依赖
check_dependencies() {
    print_info "检查依赖..."
    
    # 检查 Python3
    if ! command -v python3 &> /dev/null; then
        print_error "未找到 Python3，请先安装"
        exit 1
    fi
    
    # 检查 openpyxl
    if ! python3 -c "import openpyxl" 2>/dev/null; then
        print_warn "未找到 openpyxl 模块，正在安装..."
        pip3 install openpyxl -q
    fi
    
    print_success "依赖检查完成"
}

# 查找 DBC 文件
find_dbc_file() {
    print_info "查找 DBC Excel 文件..."
    
    # 优先查找脚本所在目录
    if [ -f "$DBC_FILE" ]; then
        print_success "找到 DBC 文件: $DBC_FILE"
        return
    fi
    
    # 查找上级目录
    if [ -f "${PROJECT_DIR}/dbc/XZ_CAN_V2.xlsx" ]; then
        DBC_FILE="${PROJECT_DIR}/dbc/XZ_CAN_V2.xlsx"
        print_success "找到 DBC 文件: $DBC_FILE"
        return
    fi
    
    # 提示用户输入
    print_warn "未找到默认 DBC 文件"
    read -p "请输入 DBC Excel 文件路径: " DBC_FILE
    
    if [ ! -f "$DBC_FILE" ]; then
        print_error "文件不存在: $DBC_FILE"
        exit 1
    fi
}

# 生成代码
generate_code() {
    print_info "正在生成 DBC 代码..."
    
    cd "$SCRIPT_DIR"
    
    # 运行生成器
    python3 dbc_generator.py --input "$DBC_FILE" --output "$PROJECT_DIR"
    
    if [ $? -ne 0 ]; then
        print_error "代码生成失败"
        exit 1
    fi
    
    print_success "代码生成完成"
}

# 更新 CMakeLists.txt
update_cmake() {
    print_info "检查 CMakeLists.txt..."
    
    CMAKE_FILE="${PROJECT_DIR}/CMakeLists.txt"
    
    # 检查是否已经包含 DbcHandler.c
    if grep -q "DbcHandler.c" "$CMAKE_FILE"; then
        print_success "CMakeLists.txt 已包含 DbcHandler.c"
    else
        print_warn "需要手动添加 DbcHandler.c 到 CMakeLists.txt"
        print_info "请在 SOURCES 中添加: \${CMAKE_CURRENT_SOURCE_DIR}/src/utils/DbcHandler.c"
    fi
}

# 编译工程
build_project() {
    print_info "正在编译工程..."
    
    cd "$PROJECT_DIR"
    
    # 创建 build 目录
    mkdir -p build
    cd build
    
    # 配置
    cmake .. > /dev/null 2>&1
    
    # 编译
    make -j4 2>&1 | tail -20
    
    if [ $? -eq 0 ]; then
        print_success "编译成功!"
        
        # 显示固件大小
        if [ -f "STM32F407_CAN.elf" ]; then
            arm-none-eabi-size STM32F407_CAN.elf 2>/dev/null || true
        fi
    else
        print_error "编译失败"
        exit 1
    fi
}

# 烧录固件
flash_firmware() {
    print_info "准备烧录固件..."
    
    read -p "是否立即烧录? (y/n): " answer
    
    if [ "$answer" = "y" ] || [ "$answer" = "Y" ]; then
        cd "$PROJECT_DIR/build"
        make flash 2>&1 | tail -30
        
        if [ $? -eq 0 ]; then
            print_success "烧录成功!"
        else
            print_error "烧录失败"
            exit 1
        fi
    else
        print_info "跳过烧录"
        print_info "稍后请手动执行: cd ${PROJECT_DIR}/build && make flash"
    fi
}

# 打印使用说明
print_usage() {
    echo ""
    echo "========================================"
    echo "DBC 一键配置完成!"
    echo "========================================"
    echo ""
    echo "生成的文件:"
    echo "  - include/DbcConfig.h    (DBC 配置头文件)"
    echo "  - src/utils/DbcHandler.c (DBC 信号处理)"
    echo "  - dbc_main_template.txt  (main.c 代码模板)"
    echo ""
    echo "下一步:"
    echo "1. 在 main.c 中添加 #include \"DbcConfig.h\""
    echo "2. 参考 dbc_main_template.txt 添加发送/接收代码"
    echo "3. 重新编译并烧录"
    echo ""
    echo "维护 CAN 矩阵:"
    echo "1. 修改 XZ_CAN_V2.xlsx"
    echo "2. 再次运行: ./generate_dbc.sh"
    echo "3. 自动重新生成代码并编译"
    echo "========================================"
}

# 主函数
main() {
    echo "========================================"
    echo "    DBC 一键配置工具"
    echo "========================================"
    echo ""
    
    # 检查依赖
    check_dependencies
    
    # 查找 DBC 文件
    find_dbc_file
    
    # 生成代码
    generate_code
    
    # 更新 CMake
    update_cmake
    
    # 编译
    build_project
    
    # 烧录
    flash_firmware
    
    # 打印使用说明
    print_usage
}

# 运行主函数
main
