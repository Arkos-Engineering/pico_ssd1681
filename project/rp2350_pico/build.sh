#!/bin/bash

# Build script for RP2350 Pico SSD1681 project
# Author: OpenCode
# Date: 2026-01-26

set -e  # Exit on error

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

echo -e "${GREEN}=== RP2350 Pico SSD1681 Build Script ===${NC}"

# Check if PICO_SDK_PATH is set
if [ -z "$PICO_SDK_PATH" ]; then
    echo -e "${RED}ERROR: PICO_SDK_PATH is not set${NC}"
    echo "Please set it with:"
    echo "  export PICO_SDK_PATH=/path/to/pico-sdk"
    exit 1
fi

echo -e "${GREEN}PICO_SDK_PATH: $PICO_SDK_PATH${NC}"

# Check if SDK exists
if [ ! -d "$PICO_SDK_PATH" ]; then
    echo -e "${RED}ERROR: PICO_SDK_PATH directory does not exist${NC}"
    echo "Path: $PICO_SDK_PATH"
    exit 1
fi

# Check for required tools
echo "Checking for required tools..."

if ! command -v cmake &> /dev/null; then
    echo -e "${RED}ERROR: cmake not found${NC}"
    echo "Install with: sudo apt install cmake"
    exit 1
fi

if ! command -v arm-none-eabi-gcc &> /dev/null; then
    echo -e "${RED}ERROR: arm-none-eabi-gcc not found${NC}"
    echo "Install with: sudo apt install gcc-arm-none-eabi"
    exit 1
fi

echo -e "${GREEN}All required tools found${NC}"

# Create build directory
if [ "$1" == "clean" ]; then
    echo -e "${YELLOW}Cleaning build directory...${NC}"
    rm -rf build
fi

if [ ! -d "build" ]; then
    echo "Creating build directory..."
    mkdir build
fi

cd build

# Configure
echo -e "${GREEN}Configuring with CMake...${NC}"
cmake .. || {
    echo -e "${RED}CMake configuration failed${NC}"
    exit 1
}

# Build
echo -e "${GREEN}Building project...${NC}"
make -j$(nproc) || {
    echo -e "${RED}Build failed${NC}"
    exit 1
}

# Success message
echo ""
echo -e "${GREEN}========================================${NC}"
echo -e "${GREEN}Build completed successfully!${NC}"
echo -e "${GREEN}========================================${NC}"
echo ""
echo "Output files:"
echo "  - pico_ssd1681.uf2  (Flash this to your Pico)"
echo "  - pico_ssd1681.elf  (For debugging)"
echo "  - pico_ssd1681.bin  (Binary firmware)"
echo ""
echo "To flash to RP2350 Pico:"
echo "  1. Hold BOOTSEL button on Pico"
echo "  2. Connect USB cable"
echo "  3. Release BOOTSEL"
echo "  4. Copy pico_ssd1681.uf2 to the RPI-RP2 drive"
echo ""
echo -e "${YELLOW}Build directory: $(pwd)${NC}"
