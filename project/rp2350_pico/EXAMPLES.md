# Code Examples - RP2350 Pico SSD1681 Driver

## Table of Contents

1. [Basic Display Test](#basic-display-test)
2. [Custom Pin Configuration](#custom-pin-configuration)
3. [Drawing Shapes](#drawing-shapes)
4. [Displaying Text](#displaying-text)
5. [Showing Images](#showing-images)
6. [Using Both Color Planes](#using-both-color-planes)
7. [Periodic Updates](#periodic-updates)

---

## Basic Display Test

Minimal code to verify your setup works:

```c
#include <stdio.h>
#include "pico/stdlib.h"
#include "driver_ssd1681_basic.h"

int main(void) {
    stdio_init_all();
    
    // Initialize display
    if (ssd1681_basic_init() != 0) {
        printf("Init failed!\n");
        return 1;
    }
    
    // Clear display
    ssd1681_basic_clear(SSD1681_COLOR_BLACK);
    
    // Draw text
    ssd1681_basic_string(SSD1681_COLOR_BLACK, 10, 10, 
                        "It works!", 9, 1, SSD1681_FONT_24);
    
    while (1) {
        sleep_ms(1000);
    }
}
```

---

## Custom Pin Configuration

### Using SPI1 Instead of SPI0

Edit `driver/inc/rp2350_pico_driver_ssd1681_interface.h`:

```c
// Change from:
#define SSD1681_SPI_PORT        spi0
#define SSD1681_PIN_MOSI        19
#define SSD1681_PIN_SCK         18

// To:
#define SSD1681_SPI_PORT        spi1
#define SSD1681_PIN_MOSI        11    // SPI1 MOSI
#define SSD1681_PIN_SCK         10    // SPI1 SCK
```

### Using Different GPIO Pins

```c
// Custom pin assignment
#define SSD1681_SPI_PORT        spi0
#define SSD1681_PIN_MOSI        3     // Any SPI0 MOSI-capable pin
#define SSD1681_PIN_SCK         2     // Any SPI0 SCK-capable pin
#define SSD1681_PIN_CS          5     // Any GPIO
#define SSD1681_PIN_DC          6     // Any GPIO
#define SSD1681_PIN_RST         7     // Any GPIO
#define SSD1681_PIN_BUSY        8     // Any GPIO
```

**Note:** After changing pins, rebuild the project:
```bash
cd build
make clean
make
```

---

## Drawing Shapes

### Draw Rectangle

```c
#include "driver_ssd1681_basic.h"

int main(void) {
    stdio_init_all();
    ssd1681_basic_init();
    ssd1681_basic_clear(SSD1681_COLOR_BLACK);
    
    // Draw filled rectangle in black
    // Parameters: color, left, top, right, bottom, fill (1=filled)
    ssd1681_basic_rect(SSD1681_COLOR_BLACK, 20, 20, 180, 100, 1);
    
    // Draw unfilled rectangle (draw border only)
    // Draw 4 thin rectangles to form a border
    ssd1681_basic_rect(SSD1681_COLOR_RED, 30, 110, 170, 112, 1);  // Top
    ssd1681_basic_rect(SSD1681_COLOR_RED, 30, 180, 170, 182, 1);  // Bottom
    ssd1681_basic_rect(SSD1681_COLOR_RED, 30, 110, 32, 182, 1);   // Left
    ssd1681_basic_rect(SSD1681_COLOR_RED, 168, 110, 170, 182, 1); // Right
    
    while (1) sleep_ms(1000);
}
```

### Draw Lines

```c
// Horizontal line
void draw_hline(uint8_t x, uint8_t y, uint8_t length) {
    for (uint8_t i = 0; i < length; i++) {
        ssd1681_basic_write_point(SSD1681_COLOR_BLACK, x + i, y, 1);
    }
}

// Vertical line
void draw_vline(uint8_t x, uint8_t y, uint8_t length) {
    for (uint8_t i = 0; i < length; i++) {
        ssd1681_basic_write_point(SSD1681_COLOR_BLACK, x, y + i, 1);
    }
}

// Diagonal line
void draw_diagonal(uint8_t x0, uint8_t y0, uint8_t length) {
    for (uint8_t i = 0; i < length; i++) {
        ssd1681_basic_write_point(SSD1681_COLOR_BLACK, x0 + i, y0 + i, 1);
    }
}

int main(void) {
    stdio_init_all();
    ssd1681_basic_init();
    ssd1681_basic_clear(SSD1681_COLOR_BLACK);
    
    draw_hline(10, 50, 100);      // Horizontal
    draw_vline(50, 10, 100);      // Vertical
    draw_diagonal(100, 100, 80);  // Diagonal
    
    while (1) sleep_ms(1000);
}
```

---

## Displaying Text

### Multiple Font Sizes

```c
#include "driver_ssd1681_basic.h"

int main(void) {
    stdio_init_all();
    ssd1681_basic_init();
    ssd1681_basic_clear(SSD1681_COLOR_BLACK);
    
    // Small font (12pt)
    ssd1681_basic_string(SSD1681_COLOR_BLACK, 10, 10, 
                        "Small", 5, 1, SSD1681_FONT_12);
    
    // Medium font (16pt)
    ssd1681_basic_string(SSD1681_COLOR_BLACK, 10, 30, 
                        "Medium", 6, 1, SSD1681_FONT_16);
    
    // Large font (24pt)
    ssd1681_basic_string(SSD1681_COLOR_BLACK, 10, 60, 
                        "Large", 5, 1, SSD1681_FONT_24);
    
    while (1) sleep_ms(1000);
}
```

### Centered Text

```c
void draw_centered_text(char *text, uint8_t y, ssd1681_font_t font) {
    uint8_t len = strlen(text);
    uint8_t font_width = (font == SSD1681_FONT_12) ? 6 : 
                         (font == SSD1681_FONT_16) ? 8 : 12;
    uint8_t text_width = len * font_width;
    uint8_t x = (200 - text_width) / 2;  // Center on 200px display
    
    ssd1681_basic_string(SSD1681_COLOR_BLACK, x, y, text, len, 1, font);
}

int main(void) {
    stdio_init_all();
    ssd1681_basic_init();
    ssd1681_basic_clear(SSD1681_COLOR_BLACK);
    
    draw_centered_text("CENTERED", 50, SSD1681_FONT_24);
    draw_centered_text("Text Example", 90, SSD1681_FONT_16);
    
    while (1) sleep_ms(1000);
}
```

### Dynamic Text (Temperature Display Example)

```c
#include <stdio.h>
#include "driver_ssd1681_basic.h"

void display_temperature(float temp) {
    char buffer[32];
    
    // Clear previous content
    ssd1681_basic_clear(SSD1681_COLOR_BLACK);
    
    // Format temperature string
    snprintf(buffer, sizeof(buffer), "Temp: %.1f C", temp);
    
    // Display
    ssd1681_basic_string(SSD1681_COLOR_BLACK, 20, 50, 
                        buffer, strlen(buffer), 1, SSD1681_FONT_24);
}

int main(void) {
    stdio_init_all();
    ssd1681_basic_init();
    
    float temp = 23.5;
    
    while (1) {
        display_temperature(temp);
        temp += 0.1;  // Simulate temperature change
        sleep_ms(5000);  // Update every 5 seconds
    }
}
```

---

## Showing Images

### Image Format

The display expects 1-bit-per-pixel bitmap data, stored in bytes row-by-row:
- Each byte represents 8 horizontal pixels
- Bit 7 = leftmost pixel, Bit 0 = rightmost pixel
- 1 = pixel ON (black or red), 0 = pixel OFF (white)

### Display a 200×200 Image

```c
#include "driver_ssd1681_basic.h"

// Example: Create a checkerboard pattern
void create_checkerboard(uint8_t *img, uint16_t width, uint16_t height) {
    uint16_t bytes_per_row = width / 8;
    
    for (uint16_t y = 0; y < height; y++) {
        for (uint16_t x_byte = 0; x_byte < bytes_per_row; x_byte++) {
            // Create 8x8 checkerboard blocks
            uint8_t pattern = ((y / 8) + (x_byte)) % 2 ? 0xAA : 0x55;
            img[y * bytes_per_row + x_byte] = pattern;
        }
    }
}

int main(void) {
    stdio_init_all();
    ssd1681_basic_init();
    
    // Allocate image buffer (200 * 200 / 8 = 5000 bytes)
    uint8_t image[5000];
    
    // Create checkerboard pattern
    create_checkerboard(image, 200, 200);
    
    // Display image
    ssd1681_basic_picture(SSD1681_COLOR_BLACK, 0, 0, 199, 199, image);
    
    while (1) sleep_ms(1000);
}
```

### Display a Partial Image

```c
// Display a 100x100 image at position (50, 50)
uint8_t small_image[1250];  // 100 * 100 / 8

// ... fill small_image with your data ...

ssd1681_basic_picture(SSD1681_COLOR_BLACK, 50, 50, 149, 149, small_image);
```

### Converting Images for Display

To convert a regular image (PNG, JPG) to the required format:

**Using ImageMagick (Linux/macOS):**
```bash
# Convert to 1-bit bitmap
convert input.png -resize 200x200 -monochrome output.bmp

# Then use a tool like xxd to create a C array:
xxd -i output.bmp > image_data.h
```

**Using Python (Pillow):**
```python
from PIL import Image
import numpy as np

def image_to_display_format(filename):
    # Load and convert to 1-bit
    img = Image.open(filename).convert('1')
    img = img.resize((200, 200))
    
    # Convert to numpy array
    arr = np.array(img)
    
    # Pack bits into bytes
    output = []
    for row in arr:
        for byte_idx in range(0, len(row), 8):
            byte = 0
            for bit in range(8):
                if row[byte_idx + bit]:
                    byte |= (1 << (7 - bit))
            output.append(byte)
    
    return bytes(output)

# Save as C array
data = image_to_display_format("input.png")
with open("image_data.h", "w") as f:
    f.write("const uint8_t image_data[] = {\n")
    for i, b in enumerate(data):
        if i % 16 == 0:
            f.write("\n    ")
        f.write(f"0x{b:02X}, ")
    f.write("\n};\n")
```

---

## Using Both Color Planes

The SSD1681 supports two color planes: BLACK and RED.

### Black and Red Text

```c
#include "driver_ssd1681_basic.h"

int main(void) {
    stdio_init_all();
    ssd1681_basic_init();
    
    // Clear both planes
    ssd1681_basic_clear(SSD1681_COLOR_BLACK);
    ssd1681_basic_clear(SSD1681_COLOR_RED);
    
    // Draw on black plane
    ssd1681_basic_string(SSD1681_COLOR_BLACK, 10, 20, 
                        "Black Text", 10, 1, SSD1681_FONT_16);
    
    // Draw on red plane
    ssd1681_basic_string(SSD1681_COLOR_RED, 10, 50, 
                        "Red Text", 8, 1, SSD1681_FONT_16);
    
    // Mix both colors
    ssd1681_basic_string(SSD1681_COLOR_BLACK, 10, 80, 
                        "Black", 5, 1, SSD1681_FONT_16);
    ssd1681_basic_string(SSD1681_COLOR_RED, 60, 80, 
                        "Red", 3, 1, SSD1681_FONT_16);
    
    while (1) sleep_ms(1000);
}
```

### Overlapping Colors

```c
// When black and red overlap, the display shows red
// (red has priority over black)

int main(void) {
    stdio_init_all();
    ssd1681_basic_init();
    ssd1681_basic_clear(SSD1681_COLOR_BLACK);
    ssd1681_basic_clear(SSD1681_COLOR_RED);
    
    // Draw black rectangle
    ssd1681_basic_rect(SSD1681_COLOR_BLACK, 40, 40, 160, 100, 1);
    
    // Draw overlapping red rectangle - red will show where they overlap
    ssd1681_basic_rect(SSD1681_COLOR_RED, 80, 70, 200, 130, 1);
    
    while (1) sleep_ms(1000);
}
```

---

## Periodic Updates

### Simple Clock Display

```c
#include <stdio.h>
#include "pico/stdlib.h"
#include "driver_ssd1681_basic.h"

void display_time(uint32_t seconds) {
    char buffer[32];
    uint32_t hours = (seconds / 3600) % 24;
    uint32_t minutes = (seconds / 60) % 60;
    uint32_t secs = seconds % 60;
    
    // Clear display
    ssd1681_basic_clear(SSD1681_COLOR_BLACK);
    
    // Format time
    snprintf(buffer, sizeof(buffer), "%02lu:%02lu:%02lu", hours, minutes, secs);
    
    // Display centered
    ssd1681_basic_string(SSD1681_COLOR_BLACK, 40, 90, 
                        buffer, strlen(buffer), 1, SSD1681_FONT_24);
}

int main(void) {
    stdio_init_all();
    ssd1681_basic_init();
    
    uint32_t seconds = 0;
    
    while (1) {
        display_time(seconds);
        seconds++;
        sleep_ms(1000);  // Update every second
        // Note: Each update takes ~2.5s due to e-paper refresh time
    }
}
```

### Warning: E-Paper Refresh Limitations

**Important:** E-paper displays are NOT suitable for high-frequency updates!

- Refresh time: ~2-3 seconds per update
- Lifetime: ~100,000 full refreshes (varies by manufacturer)
- Ghost images: Frequent updates may cause image retention

**Best practices:**
- Update only when data changes
- Minimum 180 seconds between updates for production use
- Use partial refresh if supported (not in basic driver)
- Consider LCD/OLED for >1 update/minute applications

---

## Complete Application Example

### Environmental Monitor

```c
#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/adc.h"
#include "driver_ssd1681_basic.h"

// Read temperature from RP2350's internal sensor
float read_temperature(void) {
    adc_select_input(4);  // Temperature sensor is on ADC4
    uint16_t raw = adc_read();
    float voltage = raw * 3.3f / 4096.0f;
    return 27.0f - (voltage - 0.706f) / 0.001721f;
}

void display_dashboard(float temp, uint32_t uptime) {
    char buffer[64];
    
    // Clear display
    ssd1681_basic_clear(SSD1681_COLOR_BLACK);
    ssd1681_basic_clear(SSD1681_COLOR_RED);
    
    // Title
    ssd1681_basic_string(SSD1681_COLOR_BLACK, 30, 10, 
                        "SYSTEM STATUS", 13, 1, SSD1681_FONT_16);
    
    // Temperature
    snprintf(buffer, sizeof(buffer), "Temp: %.1fC", temp);
    ssd1681_basic_string(SSD1681_COLOR_BLACK, 10, 50, 
                        buffer, strlen(buffer), 1, SSD1681_FONT_16);
    
    // Uptime
    uint32_t hours = uptime / 3600;
    uint32_t mins = (uptime % 3600) / 60;
    snprintf(buffer, sizeof(buffer), "Uptime: %luh %lum", hours, mins);
    ssd1681_basic_string(SSD1681_COLOR_BLACK, 10, 80, 
                        buffer, strlen(buffer), 1, SSD1681_FONT_12);
    
    // Status indicator
    ssd1681_basic_string(SSD1681_COLOR_RED, 10, 150, 
                        "ONLINE", 6, 1, SSD1681_FONT_24);
}

int main(void) {
    stdio_init_all();
    
    // Initialize ADC for temperature sensor
    adc_init();
    adc_set_temp_sensor_enabled(true);
    
    // Initialize display
    ssd1681_basic_init();
    
    uint32_t uptime = 0;
    
    while (1) {
        float temp = read_temperature();
        display_dashboard(temp, uptime);
        
        // Update every 5 minutes
        sleep_ms(300000);
        uptime += 300;
    }
}
```

---

## Tips and Best Practices

1. **Always clear both color planes** when starting a new frame
2. **Check return values** - all functions return 0 on success
3. **Limit update frequency** - e-paper has limited refresh cycles
4. **Use appropriate fonts** - larger fonts are more readable
5. **Test with simple patterns first** before complex graphics
6. **Keep wires short** - long wires can cause SPI communication issues

For more examples, see the libdriver documentation at:
https://www.libdriver.com/docs/ssd1681/index.html
