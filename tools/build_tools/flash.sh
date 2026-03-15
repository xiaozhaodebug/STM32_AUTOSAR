#!/bin/bash
# Flash firmware to STM32F407 via OpenOCD
# Usage: ./flash.sh [elf|bin|erase|reset]

set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(dirname "$SCRIPT_DIR")"
BUILD_DIR="${PROJECT_ROOT}/build"

FLASH_MODE=${1:-elf}
INTERFACE=${2:-jlink}

# OpenOCD configuration files
if [ "$INTERFACE" == "jlink" ]; then
    INTERFACE_CFG="interface/jlink.cfg"
else
    INTERFACE_CFG="interface/${INTERFACE}.cfg"
fi

TARGET_CFG="target/stm32f4x.cfg"

echo "======================================"
echo "STM32F407 Flash Tool"
echo "======================================"
echo ""

# Check OpenOCD
if ! command -v openocd &> /dev/null; then
    echo "Error: openocd not found in PATH"
    echo "Please install OpenOCD:"
    echo "  sudo apt install openocd  (Ubuntu/Debian)"
    exit 1
fi

echo "OpenOCD version: $(openocd --version | head -n1)"
echo "Interface: ${INTERFACE}"
echo "Target: STM32F4x"
echo ""

# Flash based on mode
case "$FLASH_MODE" in
    elf)
        ELF_FILE="${BUILD_DIR}/STM32F407_CAN.elf"
        if [ ! -f "$ELF_FILE" ]; then
            echo "Error: ELF file not found: ${ELF_FILE}"
            echo "Please build first: ./build.sh"
            exit 1
        fi
        
        echo "Flashing ELF file: ${ELF_FILE}"
        echo ""
        
        openocd \
            -f "${INTERFACE_CFG}" \
            -c "transport select swd" \
            -f "${TARGET_CFG}" \
            -c "program ${ELF_FILE} verify reset exit"
        ;;
        
    bin)
        BIN_FILE="${BUILD_DIR}/STM32F407_CAN.bin"
        if [ ! -f "$BIN_FILE" ]; then
            echo "Error: BIN file not found: ${BIN_FILE}"
            echo "Please build first: ./build.sh"
            exit 1
        fi
        
        echo "Flashing BIN file: ${BIN_FILE}"
        echo ""
        
        openocd \
            -f "${INTERFACE_CFG}" \
            -f "${TARGET_CFG}" \
            -c "init" \
            -c "reset halt" \
            -c "flash write_image erase ${BIN_FILE} 0x08000000" \
            -c "verify_image ${BIN_FILE} 0x08000000" \
            -c "reset run" \
            -c "shutdown"
        ;;
        
    hex)
        HEX_FILE="${BUILD_DIR}/STM32F407_CAN.hex"
        if [ ! -f "$HEX_FILE" ]; then
            echo "Error: HEX file not found: ${HEX_FILE}"
            echo "Please build first: ./build.sh"
            exit 1
        fi
        
        echo "Flashing HEX file: ${HEX_FILE}"
        echo ""
        
        openocd \
            -f "${INTERFACE_CFG}" \
            -f "${TARGET_CFG}" \
            -c "init" \
            -c "reset halt" \
            -c "flash write_image erase ${HEX_FILE}" \
            -c "verify_image ${HEX_FILE}" \
            -c "reset run" \
            -c "shutdown"
        ;;
        
    erase)
        echo "Erasing entire chip..."
        echo ""
        
        openocd \
            -f "${INTERFACE_CFG}" \
            -f "${TARGET_CFG}" \
            -c "init" \
            -c "reset halt" \
            -c "flash erase_sector 0 0 last" \
            -c "shutdown"
        ;;
        
    reset)
        echo "Resetting target..."
        echo ""
        
        openocd \
            -f "${INTERFACE_CFG}" \
            -f "${TARGET_CFG}" \
            -c "init" \
            -c "reset run" \
            -c "shutdown"
        ;;
        
    *)
        echo "Usage: $0 [elf|bin|hex|erase|reset] [stlink|jlink|cmsis-dap]"
        echo ""
        echo "Modes:"
        echo "  elf   - Flash ELF file (default)"
        echo "  bin   - Flash BIN file to 0x08000000"
        echo "  hex   - Flash HEX file"
        echo "  erase - Erase entire chip"
        echo "  reset - Reset target"
        echo ""
        echo "Interfaces:"
        echo "  stlink    - ST-Link (default)"
        echo "  jlink     - J-Link"
        echo "  cmsis-dap - CMSIS-DAP"
        exit 1
        ;;
esac

echo ""
echo "======================================"
echo "Operation completed successfully!"
echo "======================================"
