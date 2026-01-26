cmake_minimum_required(VERSION 3.13)

# Include the Pico SDK
include($ENV{PICO_SDK_PATH}/external/pico_sdk_import.cmake)

# Project name
project(pico_ssd1681_3wire C CXX ASM)

# Set C standard
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

# Initialize the Pico SDK
pico_sdk_init()

# Add executable
add_executable(pico_ssd1681_3wire
    src/main.c
    driver/src/rp2350_pico_driver_ssd1681_interface.c
    ../../src/driver_ssd1681.c
    ../../example/driver_ssd1681_basic.c
)

# Include directories
target_include_directories(pico_ssd1681_3wire PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/driver/inc
    ${CMAKE_CURRENT_LIST_DIR}/../../src
    ${CMAKE_CURRENT_LIST_DIR}/../../interface
    ${CMAKE_CURRENT_LIST_DIR}/../../example
)

# Link libraries
target_link_libraries(pico_ssd1681_3wire
    pico_stdlib
    hardware_spi
    hardware_gpio
)

# Enable USB output, disable UART output (change these if you want UART instead)
pico_enable_stdio_usb(pico_ssd1681_3wire 1)
pico_enable_stdio_uart(pico_ssd1681_3wire 0)

# Create map/bin/hex/uf2 files
pico_add_extra_outputs(pico_ssd1681_3wire)
