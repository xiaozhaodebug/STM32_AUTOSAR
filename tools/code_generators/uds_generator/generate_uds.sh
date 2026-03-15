#!/bin/bash
# UDS Configuration Generator Script
# One-click generation of UDS diagnostic code from Excel configuration
#
# Usage:
#   ./generate_uds.sh [config.xlsx] [output_dir]
#   ./generate_uds.sh --example    # Create example configuration

set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
GENERATOR="$SCRIPT_DIR/uds_generator.py"

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

echo -e "${BLUE}========================================${NC}"
echo -e "${BLUE}   UDS Configuration Generator${NC}"
echo -e "${BLUE}========================================${NC}"
echo ""

# Check Python and openpyxl
check_dependencies() {
    if ! command -v python3 &> /dev/null; then
        echo -e "${RED}Error: Python3 not found${NC}"
        exit 1
    fi
    
    if ! python3 -c "import openpyxl" 2>/dev/null; then
        echo -e "${YELLOW}Installing required dependency: openpyxl${NC}"
        pip3 install openpyxl
    fi
}

# Main function
main() {
    check_dependencies
    
    # Handle arguments
    if [ "$1" == "--example" ]; then
        echo -e "${YELLOW}Creating example configuration...${NC}"
        python3 "$GENERATOR" --example
        exit 0
    fi
    
    CONFIG_FILE="${1:-$SCRIPT_DIR/UDS_Config.xlsx}"
    OUTPUT_DIR="${2:-$SCRIPT_DIR/../../../AUTOSAR/UdsConfig}"
    
    # Check if config file exists
    if [ ! -f "$CONFIG_FILE" ]; then
        echo -e "${YELLOW}Configuration file not found: $CONFIG_FILE${NC}"
        echo ""
        echo -e "${BLUE}Creating example configuration...${NC}"
        python3 "$GENERATOR" --example
        echo ""
        echo -e "${YELLOW}Please edit UDS_Config_Example.xlsx and rename it to your config file${NC}"
        echo -e "Then run: ${GREEN}./generate_uds.sh UDS_Config_Example.xlsx${NC}"
        exit 0
    fi
    
    # Create output directory
    mkdir -p "$OUTPUT_DIR"
    
    # Run generator
    echo -e "${BLUE}Generating UDS configuration code...${NC}"
    echo "  Config: $CONFIG_FILE"
    echo "  Output: $OUTPUT_DIR"
    echo ""
    
    if python3 "$GENERATOR" "$CONFIG_FILE" "$OUTPUT_DIR"; then
        echo ""
        echo -e "${GREEN}✅ UDS configuration generated successfully!${NC}"
        echo ""
        echo -e "${BLUE}Generated files:${NC}"
        ls -la "$OUTPUT_DIR"/UdsConfig_Generated.* 2>/dev/null || true
        echo ""
        echo -e "${YELLOW}Next steps:${NC}"
        echo "  1. Review the generated code in: $OUTPUT_DIR"
        echo "  2. Check INTEGRATION.md for integration instructions"
        echo "  3. Rebuild your project"
    else
        echo -e "${RED}❌ Code generation failed${NC}"
        exit 1
    fi
}

main "$@"
