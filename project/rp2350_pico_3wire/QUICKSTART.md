# Quick Start Guide - RP2350 Pico SSD1681 Driver (3-Wire SPI)

Get your SSD1681 e-paper display running with RP2350 Pico using **3-wire SPI** in 5 minutes!

## What's Special About This?

**You save one GPIO pin!** Instead of 6 wires (4-wire SPI), you only need 5 wires.

The D/C (Data/Command) signal is sent as the **9th bit** in each SPI frame instead of using a separate pin.

## Prerequisites

- RP2350 Pico board
- SSD1681 e-paper display that **supports 3-wire SPI mode** (check datasheet!)
- 7 jumper wires (5 signal + power + ground)
- USB cable
- Computer with Pico SDK installed

## Step 1: Verify Display Compatibility (IMPORTANT!)

**⚠️ Not all SSD1681 displays support 3-wire mode!**

Check your display's datasheet for:
- ✅ "3-wire SPI" or "9-bit SPI" support mentioned
- ✅ BS1 pin (mode select - should be set HIGH for 3-wire)
- ⚠️ If unsure, use the 4-wire implementation instead

## Step 2: Hardware Setup (2 minutes)

Connect your display to the Pico:

```
Display Pin    →    Pico Pin         Notes
─────────────────────────────────────────────
VCC (3.3V)     →    3V3 (OUT)        Power
GND            →    GND               Ground
DIN/MOSI       →    GPIO 19          Data (9-bit frames)
CLK/SCK        →    GPIO 18          Clock
CS             →    GPIO 17          Chip select
DC/D/C         →    NOT CONNECTED    ← Leave unconnected!
RST/RESET      →    GPIO 21          Reset
BUSY           →    GPIO 22          Busy status
BS1 (if present) → 3V3 (OUT)        Select 3-wire mode
```

**Key Points:**
- ⚠️ Use 3.3V, NOT 5V!
- ⚠️ Leave D/C pin unconnected (this is the pin you're saving!)
- ⚠️ If your display has a BS1 pin, connect it to 3.3V to enable 3-wire mode

See `WIRING.txt` for a detailed diagram.

## Step 3: Build the Firmware (2 minutes)

```bash
cd /mnt/c/Users/ben/Downloads/pico_ssd1681/project/rp2350_pico_3wire

# Set Pico SDK path (if not already set)
export PICO_SDK_PATH=/path/to/pico-sdk

# Build
mkdir build
cd build
cmake ..
make -j4
```

## Step 4: Flash to Pico (1 minute)

1. **Hold down the BOOTSEL button** on your Pico
2. **Connect USB cable** while holding BOOTSEL
3. **Release BOOTSEL** - Pico appears as USB drive "RPI-RP2"
4. **Copy `build/pico_ssd1681_3wire.uf2`** to the RPI-RP2 drive
5. Pico will reboot automatically

## Step 5: View the Result

Your display should now show:
- "3-Wire SPI!" in black text
- "9-bit frames" in black text
- "No D/C pin!" in red text
- A black rectangle
- A diagonal line of dots

The display refresh takes ~2-3 seconds (normal for e-paper).

## Step 6: View Debug Output (Optional)

```bash
# Linux/macOS
screen /dev/ttyACM0 115200
```

You should see:
```
=== RP2350 SSD1681 E-Paper Display Test (3-WIRE SPI) ===
Note: Using 9-bit SPI frames (D/C + 8 data bits)
Initializing display...
Display initialized successfully
...
```

## Troubleshooting

### Display shows nothing

**Most likely cause:** Display doesn't support 3-wire mode

**Solutions:**
1. Check display datasheet for "3-wire SPI" support
2. If display has BS1 pin, ensure it's connected to 3.3V
3. Try the **4-wire implementation** instead:
   ```bash
   cd ../rp2350_pico  # 4-wire version
   ```
4. Verify wiring (especially check that D/C is NOT connected)

### Display shows garbage/partial image

**Cause:** Display in wrong mode (4-wire mode but receiving 9-bit frames)

**Solution:** 
- Check BS1 configuration
- Try 4-wire implementation

### Build fails

Same troubleshooting as 4-wire version:
```bash
# Verify PICO_SDK_PATH
echo $PICO_SDK_PATH

# If not set:
export PICO_SDK_PATH=/path/to/pico-sdk
```

## Comparison: Should I Use 3-Wire or 4-Wire?

| Factor | 3-Wire (This) | 4-Wire |
|--------|---------------|--------|
| **GPIO Pins** | 5 (saves 1!) | 6 |
| **Wiring** | 5 wires | 6 wires |
| **Speed** | ~0.1% slower (negligible) | Baseline |
| **Compatibility** | Some displays only | Universal |
| **Complexity** | Moderate | Simple |
| **Best for** | Pin-constrained projects | Maximum compatibility |

**Recommendation:**
- **Use 3-wire if:** You need to save a GPIO pin AND your display supports it
- **Use 4-wire if:** You want maximum compatibility or are unsure

## Next Steps

- **Edit code:** `src/main.c` to customize display content
- **Change pins:** Edit `driver/inc/rp2350_pico_driver_ssd1681_interface.h`
- **Full docs:** See `README.md`

## Quick Test Code

Minimal program to test 3-wire mode:

```c
#include "driver_ssd1681_basic.h"

int main(void) {
    stdio_init_all();
    
    // Initialize (uses 9-bit SPI automatically)
    ssd1681_basic_init();
    
    // Clear and draw
    ssd1681_basic_clear(SSD1681_COLOR_BLACK);
    ssd1681_basic_string(SSD1681_COLOR_BLACK, 50, 50, 
                        "3-WIRE!", 7, 1, SSD1681_FONT_24);
    
    while(1) tight_loop_contents();
}
```

## Still Not Working?

1. **Check display compatibility** - This is the #1 issue with 3-wire mode
2. **Try 4-wire implementation** - Located in `../rp2350_pico/`
3. **Check wiring** - Verify D/C pin is NOT connected
4. **Verify power** - Must be 3.3V, not 5V

## Key Differences from 4-Wire

```
4-Wire Mode:                    3-Wire Mode:
D/C pin (GPIO 20) → High/Low   No D/C pin needed!
SPI sends 8 bits               SPI sends 9 bits (D/C + 8 data)
6 wires total                  5 wires total ✓
```

---

**Success?** You're now using 3-wire SPI and saved yourself a GPIO pin! The API is identical to 4-wire mode, so all examples work the same way.

**Not working?** Don't worry - use the 4-wire implementation instead. It's more universally compatible and the extra pin won't hurt your project.
