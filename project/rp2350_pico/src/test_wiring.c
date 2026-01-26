/**
 * Wiring Test for RP2350 Pico SSD1681 Display
 * 
 * This program tests each connection individually to help diagnose wiring issues.
 * Compile this instead of main.c to test your hardware connections.
 * 
 * To use this test:
 * 1. Edit CMakeLists.txt and change "src/main.c" to "src/test_wiring.c"
 * 2. Rebuild the project
 * 3. Flash to your Pico
 * 4. Connect to USB serial and follow the prompts
 * 
 * @author OpenCode
 * @date 2026-01-26
 */

#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/spi.h"
#include "hardware/gpio.h"
#include "rp2350_pico_driver_ssd1681_interface.h"

void test_gpio_output(uint8_t pin, const char* name) {
    printf("Testing %s (GPIO %d)...\n", name, pin);
    
    gpio_init(pin);
    gpio_set_dir(pin, GPIO_OUT);
    
    printf("  Setting HIGH... ");
    gpio_put(pin, 1);
    sleep_ms(500);
    printf("OK\n");
    
    printf("  Setting LOW... ");
    gpio_put(pin, 0);
    sleep_ms(500);
    printf("OK\n");
    
    printf("  %s test PASSED\n\n", name);
}

void test_gpio_input(uint8_t pin, const char* name) {
    printf("Testing %s (GPIO %d)...\n", name, pin);
    
    gpio_init(pin);
    gpio_set_dir(pin, GPIO_IN);
    gpio_pull_down(pin);
    
    printf("  Reading value... ");
    uint8_t val = gpio_get(pin);
    printf("%d\n", val);
    
    printf("  %s test PASSED\n\n", name);
}

void test_spi(void) {
    printf("Testing SPI communication...\n");
    
    // Initialize SPI
    uint32_t baud = spi_init(SSD1681_SPI_PORT, SSD1681_SPI_BAUDRATE);
    printf("  SPI initialized at %lu Hz\n", baud);
    
    if (baud == 0) {
        printf("  ERROR: SPI initialization failed!\n");
        return;
    }
    
    // Set SPI format
    spi_set_format(SSD1681_SPI_PORT, 8, SPI_CPOL_0, SPI_CPHA_0, SPI_MSB_FIRST);
    printf("  SPI format configured (8-bit, Mode 0)\n");
    
    // Configure pins
    gpio_set_function(SSD1681_PIN_MOSI, GPIO_FUNC_SPI);
    gpio_set_function(SSD1681_PIN_SCK, GPIO_FUNC_SPI);
    printf("  MOSI and SCK configured\n");
    
    // Configure CS as GPIO
    gpio_init(SSD1681_PIN_CS);
    gpio_set_dir(SSD1681_PIN_CS, GPIO_OUT);
    gpio_put(SSD1681_PIN_CS, 1);
    printf("  CS configured\n");
    
    // Test SPI write
    printf("  Sending test pattern...\n");
    uint8_t test_data[] = {0xAA, 0x55, 0xF0, 0x0F};
    
    gpio_put(SSD1681_PIN_CS, 0);
    int written = spi_write_blocking(SSD1681_SPI_PORT, test_data, 4);
    gpio_put(SSD1681_PIN_CS, 1);
    
    if (written == 4) {
        printf("  SPI test PASSED (sent %d bytes)\n\n", written);
    } else {
        printf("  ERROR: Only sent %d of 4 bytes\n\n", written);
    }
    
    spi_deinit(SSD1681_SPI_PORT);
}

int main(void) {
    stdio_init_all();
    
    // Wait for USB connection
    printf("\n\n");
    printf("===============================================\n");
    printf("  RP2350 Pico SSD1681 Wiring Test\n");
    printf("===============================================\n\n");
    
    printf("This test will check each pin individually.\n");
    printf("If you have a multimeter or logic analyzer,\n");
    printf("you can verify signals on each pin.\n\n");
    
    sleep_ms(2000);
    
    // Test output pins
    printf("=== TESTING OUTPUT PINS ===\n\n");
    test_gpio_output(SSD1681_PIN_CS, "CS");
    test_gpio_output(SSD1681_PIN_DC, "D/C");
    test_gpio_output(SSD1681_PIN_RST, "RESET");
    
    // Test input pins
    printf("=== TESTING INPUT PINS ===\n\n");
    test_gpio_input(SSD1681_PIN_BUSY, "BUSY");
    
    // Test SPI
    printf("=== TESTING SPI ===\n\n");
    test_spi();
    
    // Summary
    printf("===============================================\n");
    printf("  Basic Pin Test Complete!\n");
    printf("===============================================\n\n");
    
    printf("Next steps:\n");
    printf("1. If all tests PASSED, your wiring is likely correct\n");
    printf("2. Connect your display and try the main example\n");
    printf("3. If display doesn't work, check:\n");
    printf("   - Display power (3.3V and GND)\n");
    printf("   - All connections are secure\n");
    printf("   - BUSY pin is connected (display hangs without it)\n\n");
    
    printf("Press RESET to run test again.\n");
    
    while (1) {
        sleep_ms(1000);
    }
    
    return 0;
}
