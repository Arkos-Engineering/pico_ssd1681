# Quick Start Guide - RP2350 Pico SSD1681 Driver

Get your SSD1681 e-paper display running with RP2350 Pico in 5 minutes!

## Prerequisites

- RP2350 Pico board
- SSD1681 e-paper display (1.54" 200×200, red/black/white)
- 8 jumper wires
- USB cable
- Computer with Pico SDK installed

## Step 1: Hardware Setup (2 minutes)

Connect your display to the Pico following this wiring:

```
Display Pin    →    Pico Pin
─────────────────────────────
VCC (3.3V)     →    3V3 (OUT)
GND            →    GND
DIN/MOSI       →    GPIO 19
CLK/SCK        →    GPIO 18
CS             →    GPIO 17
DC/D/C         →    GPIO 20
RST/RESET      →    GPIO 21
BUSY           →    GPIO 22
```

**⚠️ IMPORTANT:** Use 3.3V, NOT 5V! The display is NOT 5V tolerant.

See `WIRING.txt` for a detailed diagram.

## Step 2: Build the Firmware (2 minutes)

### Option A: Using the Build Script (Linux/macOS/WSL)

```bash
cd /mnt/c/Users/ben/Downloads/pico_ssd1681/project/rp2350_pico
./build.sh
```

### Option B: Manual Build

```bash
cd /mnt/c/Users/ben/Downloads/pico_ssd1681/project/rp2350_pico
mkdir build
cd build
cmake ..
make -j4
```

## Step 3: Flash to Pico (1 minute)

1. **Hold down the BOOTSEL button** on your Pico
2. **Connect USB cable** while holding BOOTSEL
3. **Release BOOTSEL** - Pico appears as USB drive "RPI-RP2"
4. **Copy `build/pico_ssd1681.uf2`** to the RPI-RP2 drive
5. Pico will reboot automatically and run the program

## Step 4: View the Result

Your display should now show:
- "Hello RP2350!" in black text
- "E-Paper Test" in red text
- A black rectangle
- A diagonal line of dots

The display refresh takes ~2-3 seconds (this is normal for e-paper).

## Step 5: View Debug Output (Optional)

Connect to the Pico's USB serial port to see debug messages:

```bash
# Linux/macOS
screen /dev/ttyACM0 115200

# Or use minicom
minicom -D /dev/ttyACM0 -b 115200
```

On Windows, use PuTTY or TeraTerm to connect to the Pico's COM port.

## Troubleshooting

### Display shows nothing

1. Check all wire connections
2. Verify display VCC is connected to 3.3V (not 5V)
3. Check that BUSY pin is connected
4. View debug output to see error messages

### Build fails

1. Verify PICO_SDK_PATH is set:
   ```bash
   echo $PICO_SDK_PATH
   ```
   
2. If not set:
   ```bash
   export PICO_SDK_PATH=/path/to/pico-sdk
   ```

3. Try clean rebuild:
   ```bash
   ./build.sh clean
   ```

### Can't see debug output

- Check USB cable supports data (not power-only)
- On Linux, you may need permissions:
  ```bash
  sudo usermod -a -G dialout $USER
  # Then log out and back in
  ```

## Next Steps

- Edit `src/main.c` to create your own display content
- See `README.md` for full API documentation
- Check examples in `../../example/` directory

## Pin Customization

To use different pins, edit `driver/inc/rp2350_pico_driver_ssd1681_interface.h`:

```c
#define SSD1681_PIN_MOSI        19    // Change to your pin
#define SSD1681_PIN_SCK         18    // Change to your pin
// ... etc
```

Then rebuild the project.

## Common Issues

| Problem | Solution |
|---------|----------|
| "PICO_SDK_PATH not found" | Set environment variable: `export PICO_SDK_PATH=/path/to/pico-sdk` |
| Display stays white | Check wiring, verify 3.3V power |
| Build errors | Install ARM GCC: `sudo apt install gcc-arm-none-eabi` |
| No USB serial | Switch to UART in CMakeLists.txt |
| Slow updates | Normal! E-paper takes 2-3 seconds to refresh |

## Quick Test Code

Minimal test program to verify everything works:

```c
#include "driver_ssd1681_basic.h"

int main(void) {
    stdio_init_all();
    ssd1681_basic_init();
    ssd1681_basic_clear(SSD1681_COLOR_BLACK);
    ssd1681_basic_string(SSD1681_COLOR_BLACK, 50, 50, 
                        "WORKS!", 6, 1, SSD1681_FONT_24);
    while(1) tight_loop_contents();
}
```

## Support

- **Full Documentation:** See `README.md`
- **Wiring Diagram:** See `WIRING.txt`
- **API Reference:** See `../../doc/` directory
- **Original Driver:** https://github.com/libdriver/ssd1681

---

**Ready to customize?** Open `src/main.c` in your favorite editor and start coding!
