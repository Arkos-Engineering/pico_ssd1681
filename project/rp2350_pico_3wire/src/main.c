/**
 * RP2350 Pico SSD1681 E-Paper Display Example (3-WIRE SPI)
 * 
 * This example demonstrates how to use the SSD1681 driver with the RP2350 Pico SDK
 * using 3-wire SPI mode (no D/C pin - D/C bit sent as 9th bit over SPI).
 * 
 * Hardware Connections (3-wire SPI):
 * - MOSI: GPIO 19  (Data in/out)
 * - SCK:  GPIO 18  (Clock)
 * - CS:   GPIO 17  (Chip select)
 * - RST:  GPIO 21  (Reset)
 * - BUSY: GPIO 22  (Busy status)
 * 
 * Note: NO D/C pin needed! The D/C bit is sent as the 9th bit before each byte.
 * 
 * @author OpenCode
 * @date 2026-01-26
 */

#include <stdio.h>
#include <string.h>
#include "pico/stdlib.h"
#include "driver_ssd1681_basic.h"

int main(void)
{
    uint8_t res;
    
    /* Initialize stdio for USB output */
    stdio_init_all();
    
    /* Wait for USB connection (optional, comment out if not needed) */
    sleep_ms(2000);
    
    printf("\n=== RP2350 SSD1681 E-Paper Display Test (3-WIRE SPI) ===\n");
    printf("Note: Using 9-bit SPI frames (D/C + 8 data bits)\n");
    printf("Initializing display...\n");
    
    /* Initialize the display with basic driver */
    res = ssd1681_basic_init();
    if (res != 0) {
        printf("ERROR: Display initialization failed (code %d)\n", res);
        return 1;
    }
    printf("Display initialized successfully\n");
    
    /* Clear the display (both black and red planes) */
    printf("Clearing black plane...\n");
    res = ssd1681_basic_clear(SSD1681_COLOR_BLACK);
    if (res != 0) {
        printf("ERROR: Failed to clear black plane\n");
        goto error_cleanup;
    }
    
    printf("Clearing red plane...\n");
    res = ssd1681_basic_clear(SSD1681_COLOR_RED);
    if (res != 0) {
        printf("ERROR: Failed to clear red plane\n");
        goto error_cleanup;
    }
    printf("Display cleared\n");
    
    /* Display some text */
    printf("Drawing text...\n");
    res = ssd1681_basic_string(SSD1681_COLOR_BLACK, 0, 0, "3-Wire SPI!", 11, 1, SSD1681_FONT_16);
    if (res != 0) {
        printf("ERROR: Failed to draw text\n");
        goto error_cleanup;
    }
    
    res = ssd1681_basic_string(SSD1681_COLOR_BLACK, 0, 20, "9-bit frames", 12, 1, SSD1681_FONT_16);
    if (res != 0) {
        printf("ERROR: Failed to draw text\n");
        goto error_cleanup;
    }
    
    res = ssd1681_basic_string(SSD1681_COLOR_RED, 0, 50, "No D/C pin!", 11, 1, SSD1681_FONT_16);
    if (res != 0) {
        printf("ERROR: Failed to draw red text\n");
        goto error_cleanup;
    }
    
    /* Draw a rectangle */
    printf("Drawing rectangle...\n");
    res = ssd1681_basic_rect(SSD1681_COLOR_BLACK, 10, 80, 100, 130, 1);
    if (res != 0) {
        printf("ERROR: Failed to draw rectangle\n");
        goto error_cleanup;
    }
    
    /* Draw some points */
    printf("Drawing points...\n");
    for (uint8_t i = 0; i < 20; i++) {
        res = ssd1681_basic_write_point(SSD1681_COLOR_BLACK, 120 + i, 80 + i, 1);
        if (res != 0) {
            printf("ERROR: Failed to draw point at (%d, %d)\n", 120 + i, 80 + i);
            goto error_cleanup;
        }
    }
    
    printf("Drawing complete!\n");
    printf("Display will now refresh (this takes ~2-3 seconds)...\n");
    
    /* The display automatically refreshes during the clear/draw operations */
    /* Wait a moment to ensure refresh is complete */
    sleep_ms(3000);
    
    printf("\n=== Test Complete ===\n");
    printf("Display should now show:\n");
    printf("- '3-Wire SPI!' in black at top\n");
    printf("- '9-bit frames' in black below it\n");
    printf("- 'No D/C pin!' in red\n");
    printf("- A black rectangle\n");
    printf("- A diagonal line of points\n");
    printf("\nPin savings: 5 wires instead of 6!\n");
    printf("(MOSI, SCK, CS, RST, BUSY - no D/C needed)\n");
    
    /* Main loop - you can add your own code here */
    while (1) {
        /* Optional: Add periodic updates or sleep */
        sleep_ms(1000);
    }
    
error_cleanup:
    ssd1681_basic_deinit();
    return 1;
}
