#!/bin/bash
# Debug script for STM32F407 via OpenOCD + GDB
# Usage: ./debug.sh [server|gdb|attach]

set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(dirname "$SCRIPT_DIR")"
BUILD_DIR="${PROJECT_ROOT}/build"
ELF_FILE="${BUILD_DIR}/STM32F407_CAN.elf"

DEBUG_MODE=${1:-server}
INTERFACE_CFG="interface/jlink.cfg"
TARGET_CFG="target/stm32f4x.cfg"

echo "======================================"
echo "STM32F407 Debug Tool"
echo "======================================"
echo ""

# Check tools
if ! command -v openocd &> /dev/null; then
    echo "Error: openocd not found"
    exit 1
fi

if ! command -v arm-none-eabi-gdb &> /dev/null; then
    echo "Error: arm-none-eabi-gdb not found"
    exit 1
fi

if [ ! -f "$ELF_FILE" ]; then
    echo "Error: ELF file not found: ${ELF_FILE}"
    exit 1
fi

case "$DEBUG_MODE" in
    server)
        echo "Starting OpenOCD GDB server..."
        echo "Connect with: arm-none-eabi-gdb -ex 'target remote localhost:3333'"
        echo ""
        openocd \
            -f "${INTERFACE_CFG}" \
            -f "${TARGET_CFG}"
        ;;
        
    gdb)
        echo "Starting GDB client..."
        echo ""
        arm-none-eabi-gdb \
            -ex "target remote localhost:3333" \
            -ex "monitor reset halt" \
            -ex "load" \
            "${ELF_FILE}"
        ;;
        
    attach)
        echo "Attaching to running target..."
        echo ""
        arm-none-eabi-gdb \
            -ex "target remote localhost:3333" \
            -ex "monitor halt" \
            "${ELF_FILE}"
        ;;
        
    *)
        echo "Usage: $0 [server|gdb|attach]"
        echo ""
        echo "Modes:"
        echo "  server - Start OpenOCD GDB server (default)"
        echo "  gdb    - Start GDB client and connect"
        echo "  attach - Attach to running target"
        exit 1
        ;;
esac
