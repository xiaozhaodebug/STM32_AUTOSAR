#!/bin/bash
# STM32F407 Build Script
# Usage: ./build.sh [Debug|Release|MinSizeRel] [clean]

set -e

# Default build type
BUILD_TYPE=${1:-Debug}
CLEAN_BUILD=$2

# Script directory
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(dirname "$SCRIPT_DIR")"
BUILD_DIR="${PROJECT_ROOT}/build"

echo "======================================"
echo "STM32F407 CAN Project Build"
echo "======================================"
echo ""

# Check toolchain
if ! command -v arm-none-eabi-gcc &> /dev/null; then
    echo "Error: arm-none-eabi-gcc not found in PATH"
    echo "Please install GCC ARM Embedded toolchain:"
    echo "  sudo apt install gcc-arm-none-eabi  (Ubuntu/Debian)"
    echo "  or download from: https://developer.arm.com/downloads/-/gnu-rm"
    exit 1
fi

if ! command -v cmake &> /dev/null; then
    echo "Error: cmake not found in PATH"
    echo "Please install CMake:"
    echo "  sudo apt install cmake"
    exit 1
fi

echo "Toolchain found:"
echo "  GCC: $(arm-none-eabi-gcc --version | head -n1)"
echo "  CMake: $(cmake --version | head -n1)"
echo ""

# Clean build if requested
if [ "$CLEAN_BUILD" == "clean" ]; then
    echo "Cleaning build directory..."
    rm -rf "${BUILD_DIR}"
fi

# Create build directory
mkdir -p "${BUILD_DIR}"
cd "${BUILD_DIR}"

# Configure
echo "Configuring with CMake..."
echo "  Build Type: ${BUILD_TYPE}"
echo ""

cmake -DCMAKE_BUILD_TYPE="${BUILD_TYPE}" \
      -DCMAKE_TOOLCHAIN_FILE="${PROJECT_ROOT}/cmake/toolchain.cmake" \
      "${PROJECT_ROOT}"

# Build
echo ""
echo "Building..."
echo ""

make -j$(nproc) 2>&1 | tee build.log

echo ""
echo "======================================"
echo "Build completed successfully!"
echo "======================================"
echo ""
echo "Output files:"
echo "  ${BUILD_DIR}/STM32F407_CAN.elf"
echo "  ${BUILD_DIR}/STM32F407_CAN.hex"
echo "  ${BUILD_DIR}/STM32F407_CAN.bin"
echo ""
echo "Flash commands:"
echo "  cd ${BUILD_DIR} && make flash      # Flash ELF via OpenOCD"
echo "  cd ${BUILD_DIR} && make flash-bin  # Flash BIN via OpenOCD"
echo ""
