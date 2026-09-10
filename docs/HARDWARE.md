# Hardware Setup Guide

## ESP32-2432S028 Pinout (CYD - Cheap Yellow Display)

### Display (ILI9341) - SPI Pins

| Signal | GPIO | Note |
|--------|------|------|
| CLK    | 18   | SPI Clock |
| MOSI   | 23   | SPI Data Out (Master Out) |
| MISO   | 19   | SPI Data In (Master In) - Not used for display |
| CS     | 5    | Display Chip Select |
| DC     | 2    | Data/Command selector |
| RST    | 4    | Display Reset |
| LED    | 15   | Backlight (PWM capable) |
| GND    | GND  | Ground |
| 3V3    | 3V3  | Power (3.3V) |

### Touch Controller (XPT2046) - SPI Pins (Shared)

| Signal | GPIO | Note |
|--------|------|------|
| CLK    | 18   | **Shared with display** |
| MOSI   | 23   | **Shared with display** |
| MISO   | 19   | **Shared with display** |
| CS     | 33   | Touch Chip Select (**different from display**) |
| IRQ    | 36   | Interrupt pin (optional) |
| GND    | GND  | Ground |
| 3V3    | 3V3  | Power (3.3V) |

### Additional GPIO (Expansion/Future Use)

| GPIO | Use | Note |
|------|-----|------|
| 26   | Available | Can be used for external modules |
| 25   | Available | Can be used for external modules |
| 27   | Available | Can be used for external modules |
| 14   | Available | Can be used for external modules |
| 12   | Available | Can be used for external modules |
| 13   | Available | Can be used for external modules |

### I2C for Optional CardKB Keyboard

| Signal | GPIO | Note |
|--------|------|------|
| SDA    | 21   | I2C Data |
| SCL    | 22   | I2C Clock |
| GND    | GND  | Ground |
| 3V3    | 3V3  | Power (3.3V) |

## Wiring Diagram

```
ESP32-2432S028 (CYD)
┌─────────────────────────────┐
│                             │
│  Display (ILI9341):         │  Touch (XPT2046):
│  ├─ GPIO18 (CLK)  ─────────────┼── GPIO18 (CLK)
│  ├─ GPIO23 (MOSI) ─────────────┼── GPIO23 (MOSI)
│  ├─ GPIO19 (MISO) ─────────────┼── GPIO19 (MISO)
│  ├─ GPIO5  (CS)               │
│  ├─ GPIO2  (DC)               │
│  ├─ GPIO4  (RST)              │
│  ├─ GPIO15 (LED/Backlight)    │
│  └─ 3V3, GND                  │  ├─ GPIO33 (CS)
│                               │  ├─ GPIO36 (IRQ)
│                               │  └─ 3V3, GND
│                             │
│  Optional CardKB Keyboard:  │
│  ├─ GPIO21 (SDA/I2C)        │
│  ├─ GPIO22 (SCL/I2C)        │
│  └─ 3V3, GND                │
│                             │
└─────────────────────────────┘
         ║
         ║ USB-C (Programming & Power)
         ║
    Host Computer
```

## Power Specifications

- **Operating Voltage:** 3.3V (regulated via onboard LDO from USB)
- **Peak Current:** ~500 mA (WiFi + display active)
- **USB Power:** 5V @ 2A minimum (via USB-C)
- **Battery Option:** 3.7V LiPo with charging circuit (not included)

## Display Specifications

### ILI9341 TFT Controller
- **Resolution:** 320 × 240 pixels
- **Color Depth:** 16-bit (RGB565)
- **Interface:** 4-wire SPI
- **Max SPI Speed:** 40 MHz (use 27 MHz for stability)
- **Refresh Rate:** Depends on SPI speed and update size
- **Viewing Angle:** 85° typical

### Physical Dimensions
- **Panel Size:** 2.8 inches diagonal
- **Active Area:** ~56 mm × 42 mm
- **Bezel:** ~5 mm on all sides

## Touch Controller Specifications

### XPT2046 Touchscreen Controller
- **Type:** Resistive 4-wire touchscreen
- **Resolution:** Analog (up to 12-bit)
- **Interface:** SPI
- **Pressure Sensitivity:** 0-4095 (raw ADC counts)
- **Accuracy:** ±2% typical

### Calibration

The ESP32 needs to know the raw ADC coordinates that map to display pixels. Default values are provided in `config.h`:

```cpp
#define TOUCH_MIN_X 280
#define TOUCH_MAX_X 3870
#define TOUCH_MIN_Y 200
#define TOUCH_MAX_Y 3900
```

**To calibrate your specific board:**
1. Use the TFT_eSPI `Calibration_Touch` example sketch
2. Follow on-screen instructions
3. Record the printed calibration values
4. Update `config.h` with your values

## Network Connectivity

### WiFi (ESP32 Onboard)
- **Standards:** 802.11 b/g/n (2.4 GHz only)
- **Antenna:** PCB trace (built-in)
- **Range:** ~100 meters line-of-sight
- **Max Throughput:** ~20 Mbps

### Ollama Host Connection
- **Protocol:** HTTP REST (TCP port 11434)
- **Network:** Local LAN (LXC/Docker on same network)
- **Latency Target:** <100 ms (should be <50 ms on LAN)

## Thermal Considerations

- **Operating Temperature:** 0°C to 40°C
- **Storage Temperature:** -20°C to 70°C
- **ESP32 Junction Temp:** Typically 45-70°C under load
- **Note:** No active cooling required for typical use

## Optional Expansions

### CardKB I2C Mini Keyboard
- **Interface:** I2C (GPIO21/GPIO22)
- **I2C Address:** 0x5F (default)
- **Voltage:** 3.3V
- **Cost:** ~$5-8 USD
- **Benefit:** Full physical keyboard input instead of on-screen only

### External Battery
- **Type:** 3.7V 18650 LiPo or similar
- **Requires:** TP4056 charging module (not included)
- **Wiring:** To ESP32's 3V3 rail via buck converter if needed
- **Benefit:** Portable operation without USB tethering

## Troubleshooting Hardware

### Display Not Showing Anything
1. Verify GPIO18/23/19/5 connections (SPI bus)
2. Check GPIO2 (DC), GPIO4 (RST) connections
3. Ensure 3.3V power is stable
4. Try the TFT_eSPI example sketches first

### Touch Not Responsive
1. Verify GPIO33 (CS) connection
2. Run touch calibration (see Calibration section)
3. Check pressure threshold in `config.h`
4. Use I2C scanner to verify no I2C conflicts

### WiFi Connection Issues
1. Verify SSID and password in `config.h`
2. Check router is 2.4 GHz compatible (802.11 b/g/n)
3. Ensure Ollama host IP is reachable on network
4. Check firewall port 11434 is open

### Memory Issues / Crashes
1. Reduce buffer sizes in `config.h`
2. Enable PSRAM mode in `platformio.ini`
3. Simplify font rendering
4. Monitor serial output for heap allocation errors
