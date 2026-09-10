# ESP32 Ollama AI Terminal

A thin-client AI terminal running on the **ESP32-2432S028** ("Cheap Yellow Display") that streams responses from a local **Ollama LXC** container in real-time.

## Features

- **Chunked Streaming:** Token-by-token response rendering directly to the 320x240 TFT display
- **Memory Efficient:** Optimized for 520 KB SRAM constraints using buffered streaming
- **Local Inference:** Delegates heavy LLM computation to Ollama on a host machine
- **Touchscreen Input:** Resistive XPT2046 touch controller with on-screen keyboard
- **Multiple Input Methods:** On-screen keyboard, serial input, or optional CardKB I2C keyboard

## Hardware

- **Microcontroller:** ESP32-WROOM-32E (Dual-core, 240 MHz, 520 KB SRAM)
- **Display:** 2.8" TFT (ILI9341, 320x240 SPI)
- **Touch:** XPT2046 Resistive Touch Controller
- **Cost:** ~$10-15 USD

## System Architecture

```
┌─────────────────────────┐         ┌──────────────────────┐
│  ESP32 CYD (Thin Client)│         │  Ollama LXC (Host)   │
│  - Display Rendering    │◄───────►│  - LLM Inference     │
│  - Touch Input          │ HTTP    │  - Token Streaming   │
│  - WiFi Communication   │ REST    │  - Model Loading     │
└─────────────────────────┘         └──────────────────────┘
```

## Quick Start

### Prerequisites

- **PlatformIO** (recommended) or Arduino IDE
- **ESP32 Board Support** installed
- **Ollama** running on a local LXC/Docker container with port 11434 exposed

### Configuration

1. Clone this repository
2. Edit `src/config.h` with your WiFi credentials and Ollama host IP
3. Flash firmware to ESP32-2432S028
4. Power on and connect to your Ollama instance

### Required Libraries

- `WiFi` (ESP-IDF built-in)
- `HTTPClient` (ESP-IDF built-in)
- `ArduinoJson` (for JSON parsing)
- `TFT_eSPI` (for ILI9341 display driver)
- `XPT2046_Touchscreen` (for touch input)

## Project Structure

```
esp32-ollama-ai-terminal/
├── src/
│   ├── main.cpp              # Application entry point
│   ├── config.h              # WiFi & Ollama configuration
│   ├── wifi_manager.h        # WiFi connection handling
│   ├── wifi_manager.cpp
│   ├── ollama_client.h       # Ollama API integration
│   ├── ollama_client.cpp
│   ├── display_driver.h      # TFT display wrapper
│   ├── display_driver.cpp
│   ├── touch_input.h         # Touch and keyboard interface
│   ├── touch_input.cpp
│   ├── ui_manager.h          # UI orchestration
│   └── ui_manager.cpp
├── include/
│   └── fonts/                # Font files for display
├── platformio.ini            # PlatformIO configuration
├── README.md                 # This file
└── docs/
    ├── HARDWARE.md           # Hardware pinout and wiring
    ├── MEMORY_OPTIMIZATION.md # Performance tuning guide
    └── TROUBLESHOOTING.md    # Common issues and solutions
```

## Development Workflow

Each phase of development includes:
- Source code implementation
- Header definitions
- Configuration settings
- Example usage and tests

Phases are executed sequentially to allow for incremental integration and testing.

## License

MIT License - See LICENSE file for details

## Troubleshooting

See [TROUBLESHOOTING.md](docs/TROUBLESHOOTING.md) for common issues and solutions.

## Contributing

Contributions welcome! Please create a feature branch and submit a pull request.
