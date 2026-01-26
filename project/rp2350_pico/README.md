# SSD1681 E-Paper Display Driver for RP2350 Pico

This is a complete Raspberry Pi Pico SDK implementation for the libdriver/ssd1681 e-paper display driver, targeting the RP2350 microcontroller.

## Hardware Requirements

- Raspberry Pi Pico 2 (RP2350) or compatible board
- SSD1681-based e-paper display module (typically 1.54" 200×200, red/black/white)
- 6 wire connections (4-wire SPI + reset + busy)

## Pin Connections

### Default Pin Assignment

| Function | RP2350 GPIO | Display Module Pin |
|----------|-------------|-------------------|
| SPI MOSI | GPIO 19     | DIN / MOSI        |
| SPI SCK  | GPIO 18     | CLK / SCLK        |
| SPI CS   | GPIO 17     | CS                |
| D/C      | GPIO 20     | DC / D/C          |
| Reset    | GPIO 21     | RST / RESET       |
| Busy     | GPIO 22     | BUSY              |
| 3.3V     | 3V3 (OUT)   | VCC               |
| GND      | GND         | GND               |

**Note:** Pin assignments can be customized in `driver/inc/rp2350_pico_driver_ssd1681_interface.h`

### Pin Change Instructions

To change pins, edit the following defines in `rp2350_pico_driver_ssd1681_interface.h`:

```c
#define SSD1681_SPI_PORT        spi0        // or spi1
#define SSD1681_PIN_MOSI        19          // Your MOSI pin
#define SSD1681_PIN_SCK         18          // Your SCK pin
#define SSD1681_PIN_CS          17          // Your CS pin
#define SSD1681_PIN_DC          20          // Your D/C pin
#define SSD1681_PIN_RST         21          // Your RST pin
#define SSD1681_PIN_BUSY        22          // Your BUSY pin
```

## Software Requirements

### Pico SDK Setup

1. **Install the Pico SDK** (version 2.0.0 or later for RP2350 support):
   ```bash
   cd ~/
   git clone https://github.com/raspberrypi/pico-sdk.git
   cd pico-sdk
   git submodule update --init
   ```

2. **Set the PICO_SDK_PATH environment variable:**
   ```bash
   export PICO_SDK_PATH=~/pico-sdk
   ```
   
   Add this to your `~/.bashrc` or `~/.zshrc` to make it permanent:
   ```bash
   echo 'export PICO_SDK_PATH=~/pico-sdk' >> ~/.bashrc
   source ~/.bashrc
   ```

3. **Install required tools:**
   - **Linux/WSL:**
     ```bash
     sudo apt update
     sudo apt install cmake gcc-arm-none-eabi libnewlib-arm-none-eabi \
                      build-essential libstdc++-arm-none-eabi-newlib
     ```
   
   - **macOS:**
     ```bash
     brew install cmake
     brew tap ArmMbed/homebrew-formulae
     brew install arm-none-eabi-gcc
     ```
   
   - **Windows:** Install [ARM GCC toolchain](https://developer.arm.com/tools-and-software/open-source-software/developer-tools/gnu-toolchain/gnu-rm/downloads) and [CMake](https://cmake.org/download/)

## Building the Project

1. **Navigate to the project directory:**
   ```bash
   cd /mnt/c/Users/ben/Downloads/pico_ssd1681/project/rp2350_pico
   ```

2. **Create build directory:**
   ```bash
   mkdir build
   cd build
   ```

3. **Configure with CMake:**
   ```bash
   cmake ..
   ```

4. **Build the project:**
   ```bash
   make -j4
   ```

5. **The output files will be generated:**
   - `pico_ssd1681.uf2` - Ready to upload to Pico
   - `pico_ssd1681.elf` - For debugging
   - `pico_ssd1681.bin` - Binary firmware
   - `pico_ssd1681.hex` - Hex firmware

## Flashing to RP2350 Pico

### Method 1: UF2 Bootloader (Easiest)

1. Hold down the **BOOTSEL** button on your Pico
2. While holding BOOTSEL, connect the Pico to your computer via USB
3. Release BOOTSEL - the Pico will appear as a USB mass storage device
4. Copy `pico_ssd1681.uf2` to the mounted drive
5. The Pico will automatically reboot and run your program

### Method 2: Using picotool

```bash
# Install picotool (one-time setup)
cd ~/
git clone https://github.com/raspberrypi/picotool.git
cd picotool
mkdir build && cd build
cmake ..
make

# Flash the firmware
sudo ./picotool load -f pico_ssd1681.uf2
sudo ./picotool reboot
```

### Method 3: Using OpenOCD + SWD Debugger

If you have a debug probe connected to the SWD pins:

```bash
openocd -f interface/cmsis-dap.cfg -f target/rp2040.cfg \
        -c "adapter speed 5000" \
        -c "program pico_ssd1681.elf verify reset exit"
```

## Viewing Debug Output

The example uses **USB CDC** for printf output.

### Linux/macOS

```bash
# Find the device (usually /dev/ttyACM0)
ls /dev/tty*

# Connect with screen
screen /dev/ttyACM0 115200

# Or use minicom
minicom -D /dev/ttyACM0 -b 115200
```

### Windows

Use **PuTTY** or **TeraTerm** and connect to the Pico's COM port at 115200 baud.

### Switching to UART Output

To use UART instead of USB for debug output:

1. Edit `CMakeLists.txt` and change:
   ```cmake
   pico_enable_stdio_usb(pico_ssd1681 0)
   pico_enable_stdio_uart(pico_ssd1681 1)
   ```

2. Connect UART:
   - GPIO 0 (UART0 TX) → Your USB-Serial RX
   - GPIO 1 (UART0 RX) → Your USB-Serial TX
   - GND → GND

3. Rebuild the project

## Usage Examples

### Basic Example (Included)

The included `main.c` demonstrates:
- Display initialization
- Clearing the display
- Drawing text in black and red
- Drawing rectangles
- Drawing individual points

### Custom Application Template

```c
#include "driver_ssd1681_basic.h"

int main(void) {
    stdio_init_all();
    
    /* Initialize display */
    if (ssd1681_basic_init() != 0) {
        printf("Display init failed\n");
        return 1;
    }
    
    /* Clear display */
    ssd1681_basic_clear(SSD1681_COLOR_BLACK);
    ssd1681_basic_clear(SSD1681_COLOR_RED);
    
    /* Draw text */
    ssd1681_basic_string(SSD1681_COLOR_BLACK, 10, 10, 
                        "Hello World!", 12, 1, SSD1681_FONT_16);
    
    /* Draw rectangle */
    ssd1681_basic_rect(SSD1681_COLOR_RED, 20, 40, 180, 120, 1);
    
    /* Your main loop */
    while (1) {
        sleep_ms(1000);
    }
    
    return 0;
}
```

### Drawing an Image

```c
/* Image must be 200x200 pixels, 1 bit per pixel, stored row-major */
uint8_t my_image[5000];  // 200*200/8 = 5000 bytes

/* Load your image data into my_image[] */
// ... (convert image to 1bpp format)

/* Display the image */
ssd1681_basic_picture(SSD1681_COLOR_BLACK, 0, 0, 199, 199, my_image);
```

## API Reference

### Initialization

```c
uint8_t ssd1681_basic_init(void);
uint8_t ssd1681_basic_deinit(void);
```

### Display Control

```c
uint8_t ssd1681_basic_clear(ssd1681_color_t color);
```
- `color`: `SSD1681_COLOR_BLACK` or `SSD1681_COLOR_RED`

### Drawing Functions

```c
/* Draw a single point */
uint8_t ssd1681_basic_write_point(ssd1681_color_t color, uint8_t x, uint8_t y, uint8_t data);

/* Read a point */
uint8_t ssd1681_basic_read_point(ssd1681_color_t color, uint8_t x, uint8_t y, uint8_t *data);

/* Draw text string */
uint8_t ssd1681_basic_string(ssd1681_color_t color, uint8_t x, uint8_t y, 
                             char *str, uint16_t len, uint8_t data, 
                             ssd1681_font_t font);

/* Draw filled rectangle */
uint8_t ssd1681_basic_rect(ssd1681_color_t color, uint8_t left, uint8_t top, 
                           uint8_t right, uint8_t bottom, uint8_t data);

/* Draw bitmap image */
uint8_t ssd1681_basic_picture(ssd1681_color_t color, uint8_t left, uint8_t top, 
                              uint8_t right, uint8_t bottom, uint8_t *img);
```

### Font Sizes

- `SSD1681_FONT_12` - 12pt font
- `SSD1681_FONT_16` - 16pt font
- `SSD1681_FONT_24` - 24pt font

## Performance

- **SPI Transfer Rate:** ~4 MHz (configurable up to 10 MHz)
- **Full Screen Update:** ~20 ms (SPI transfer) + ~2500 ms (e-paper refresh)
- **Partial Updates:** Not supported by basic driver (full refresh only)

## Troubleshooting

### Display Not Working

1. **Check connections:**
   ```bash
   # Verify pins are correct in the header file
   cat driver/inc/rp2350_pico_driver_ssd1681_interface.h
   ```

2. **Check power:**
   - Ensure display VCC is connected to 3.3V (not 5V!)
   - Check GND connection

3. **Enable debug output:**
   ```c
   // The driver will print debug info via printf
   // Make sure USB/UART output is working
   ```

4. **Verify BUSY signal:**
   - The display may hang if BUSY pin is not connected
   - Check continuity between BUSY pin and GPIO 22

### Build Errors

- **"PICO_SDK_PATH not found":**
  ```bash
  export PICO_SDK_PATH=/path/to/pico-sdk
  ```

- **"arm-none-eabi-gcc not found":**
  Install the ARM GCC toolchain (see Software Requirements)

- **CMake errors:**
  Delete `build` directory and try again:
  ```bash
  rm -rf build
  mkdir build
  cd build
  cmake ..
  ```

### Display Shows Nothing

- Check if initialization succeeded (check return values)
- Verify that both color planes are cleared
- Try increasing SPI delay (reduce `SSD1681_SPI_BAUDRATE`)
- Some displays require specific initialization sequences (check your display datasheet)

## Advanced Configuration

### Adjusting SPI Speed

Edit `rp2350_pico_driver_ssd1681_interface.h`:

```c
#define SSD1681_SPI_BAUDRATE    4000000     // 4 MHz (default)
// #define SSD1681_SPI_BAUDRATE    10000000     // 10 MHz (maximum)
// #define SSD1681_SPI_BAUDRATE    2000000      // 2 MHz (if having issues)
```

### Using Different SPI Peripheral

```c
// To use SPI1 instead of SPI0:
#define SSD1681_SPI_PORT        spi1
#define SSD1681_PIN_MOSI        11  // SPI1 MOSI
#define SSD1681_PIN_SCK         10  // SPI1 SCK
// ... update other pins as needed
```

### Custom Display Settings

The driver uses defaults from `driver_ssd1681_basic.h`. To customize:

1. Copy `../../example/driver_ssd1681_basic.c` to your project
2. Modify the initialization parameters
3. Rebuild

## References

- **RP2350 Datasheet:** https://datasheets.raspberrypi.com/rp2350/rp2350-datasheet.pdf
- **Pico SDK Documentation:** https://www.raspberrypi.com/documentation/pico-sdk/
- **SSD1681 Datasheet:** See `../../datasheet/` directory
- **LibDriver Documentation:** https://www.libdriver.com/docs/ssd1681/index.html

## License

This implementation follows the MIT License of the libdriver/ssd1681 project.
See LICENSE file in the root directory.

## Support

For issues specific to:
- **RP2350 Pico implementation:** Check this README and troubleshooting section
- **SSD1681 driver functions:** Refer to libdriver documentation
- **Pico SDK issues:** See Raspberry Pi Pico SDK documentation
