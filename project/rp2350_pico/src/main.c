/**
 * RP2350 Pico SSD1681 E-Paper Display Example
 * 
 * This example demonstrates how to use the SSD1681 driver with the RP2350 Pico SDK.
 * It initializes the display and shows basic drawing operations.
 * 
 * Hardware Connections (4-wire SPI):
 * - MOSI: GPIO 19
 * - SCK:  GPIO 18
 * - CS:   GPIO 17
 * - D/C:  GPIO 20
 * - RST:  GPIO 21
 * - BUSY: GPIO 22
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
    
    printf("\n=== RP2350 SSD1681 E-Paper Display Test ===\n");
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
    res = ssd1681_basic_string(SSD1681_COLOR_BLACK, 0, 0, "Hello RP2350!", 13, 1, SSD1681_FONT_16);
    if (res != 0) {
        printf("ERROR: Failed to draw text\n");
        goto error_cleanup;
    }
    
    res = ssd1681_basic_string(SSD1681_COLOR_RED, 0, 20, "E-Paper Test", 12, 1, SSD1681_FONT_16);
    if (res != 0) {
        printf("ERROR: Failed to draw red text\n");
        goto error_cleanup;
    }
    
    /* Draw a rectangle */
    printf("Drawing rectangle...\n");
    res = ssd1681_basic_rect(SSD1681_COLOR_BLACK, 10, 50, 100, 100, 1);
    if (res != 0) {
        printf("ERROR: Failed to draw rectangle\n");
        goto error_cleanup;
    }
    
    /* Draw some points */
    printf("Drawing points...\n");
    for (uint8_t i = 0; i < 20; i++) {
        res = ssd1681_basic_write_point(SSD1681_COLOR_BLACK, 120 + i, 50 + i, 1);
        if (res != 0) {
            printf("ERROR: Failed to draw point at (%d, %d)\n", 120 + i, 50 + i);
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
    printf("- 'Hello RP2350!' in black at top\n");
    printf("- 'E-Paper Test' in red below it\n");
    printf("- A black rectangle\n");
    printf("- A diagonal line of points\n");
    
    /* Main loop - you can add your own code here */
    while (1) {
        /* Optional: Add periodic updates or sleep */
        sleep_ms(1000);
    }
    
error_cleanup:
    ssd1681_basic_deinit();
    return 1;
}
