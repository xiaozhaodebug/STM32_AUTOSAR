#!/bin/bash
# CAN总线接口配置脚本
# ===================
# 用于快速配置Linux系统的CAN接口

set -e

# 颜色定义
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

# 默认配置
INTERFACE="can0"
BITRATE=500000
TYPE="can"  # can 或 vcan

print_usage() {
    echo "CAN接口配置脚本"
    echo ""
    echo "用法: $0 [选项]"
    echo ""
    echo "选项:"
    echo "  -i, --interface    CAN接口名称 (默认: can0)"
    echo "  -b, --bitrate      波特率 bps (默认: 500000)"
    echo "  -t, --type         接口类型: can|vcan (默认: can)"
    echo "  -d, --down         关闭接口"
    echo "  -s, --status       显示接口状态"
    echo "  -h, --help         显示此帮助"
    echo ""
    echo "示例:"
    echo "  $0                                    # 配置can0为500Kbps"
    echo "  $0 -i can1 -b 250000                  # 配置can1为250Kbps"
    echo "  $0 -t vcan                            # 创建虚拟CAN接口"
    echo "  $0 -d                                 # 关闭can0"
    echo "  $0 -i can0 -s                         # 显示can0状态"
}

print_status() {
    echo -e "${GREEN}[INFO]${NC} $1"
}

print_error() {
    echo -e "${RED}[ERROR]${NC} $1"
}

print_warning() {
    echo -e "${YELLOW}[WARN]${NC} $1"
}

check_root() {
    if [ "$EUID" -ne 0 ]; then
        print_error "请使用sudo运行此脚本"
        exit 1
    fi
}

load_modules() {
    print_status "加载CAN内核模块..."
    
    if [ "$TYPE" = "vcan" ]; then
        modprobe vcan 2>/dev/null || {
            print_error "无法加载vcan模块"
            exit 1
        }
    else
        modprobe can 2>/dev/null || true
        modprobe can_raw 2>/dev/null || true
    fi
    
    print_status "内核模块加载完成"
}

setup_can_interface() {
    print_status "配置CAN接口: $INTERFACE"
    print_status "波特率: $BITRATE bps ($(echo "scale=1; $BITRATE/1000" | bc) Kbps)"
    
    # 检查接口是否存在
    if ip link show "$INTERFACE" &>/dev/null; then
        print_warning "接口 $INTERFACE 已存在，先关闭"
        ip link set "$INTERFACE" down 2>/dev/null || true
    fi
    
    # 配置接口
    if [ "$TYPE" = "vcan" ]; then
        # 虚拟CAN
        ip link add dev "$INTERFACE" type vcan 2>/dev/null || {
            print_warning "接口可能已存在，尝试直接启动"
        }
    else
        # 真实CAN
        ip link set "$INTERFACE" type can bitrate "$BITRATE" 2>/dev/null || {
            print_error "无法配置接口 $INTERFACE"
            print_error "可能原因:"
            print_error "  1. 硬件未连接"
            print_error "  2. 驱动未加载"
            print_error "  3. 接口名称错误"
            exit 1
        }
    fi
    
    # 启动接口
    ip link set up "$INTERFACE"
    
    print_status "接口 $INTERFACE 启动成功"
}

tear_down_interface() {
    print_status "关闭CAN接口: $INTERFACE"
    
    if ip link show "$INTERFACE" &>/dev/null; then
        ip link set "$INTERFACE" down 2>/dev/null || true
        ip link delete "$INTERFACE" 2>/dev/null || true
        print_status "接口 $INTERFACE 已关闭"
    else
        print_warning "接口 $INTERFACE 不存在"
    fi
}

show_status() {
    print_status "CAN接口状态: $INTERFACE"
    echo ""
    
    if ip link show "$INTERFACE" &>/dev/null; then
        ip -details link show "$INTERFACE"
        echo ""
        
        # 检查接口状态
        if ip link show "$INTERFACE" | grep -q "state UP"; then
            print_status "接口状态: UP (运行中)"
        else
            print_warning "接口状态: DOWN (未运行)"
        fi
    else
        print_error "接口 $INTERFACE 不存在"
        echo ""
        echo "可用的CAN接口:"
        ip link show | grep -i can || echo "  无"
    fi
}

# 解析参数
while [[ $# -gt 0 ]]; do
    case $1 in
        -i|--interface)
            INTERFACE="$2"
            shift 2
            ;;
        -b|--bitrate)
            BITRATE="$2"
            shift 2
            ;;
        -t|--type)
            TYPE="$2"
            shift 2
            ;;
        -d|--down)
            ACTION="down"
            shift
            ;;
        -s|--status)
            ACTION="status"
            shift
            ;;
        -h|--help)
            print_usage
            exit 0
            ;;
        *)
            print_error "未知选项: $1"
            print_usage
            exit 1
            ;;
    esac
done

# 默认动作
ACTION=${ACTION:-"up"}

# 显示信息
print_status "CAN接口配置工具"
echo "=================================="

# 执行动作
case $ACTION in
    up)
        check_root
        load_modules
        setup_can_interface
        show_status
        ;;
    down)
        check_root
        tear_down_interface
        ;;
    status)
        show_status
        ;;
esac

echo ""
print_status "操作完成"
