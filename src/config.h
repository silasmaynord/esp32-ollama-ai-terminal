/**
 * @file config.h
 * @brief Configuration and build settings for ESP32 Ollama AI Terminal
 */

#ifndef CONFIG_H
#define CONFIG_H

// ============================================================================
// WIFI CONFIGURATION
// ============================================================================

#define WIFI_SSID "YOUR_WIFI_SSID"
#define WIFI_PASSWORD "YOUR_WIFI_PASSWORD"
#define WIFI_CONNECT_TIMEOUT_MS 15000
#define WIFI_MAX_ATTEMPTS 3

// ============================================================================
// OLLAMA SERVER CONFIGURATION
// ============================================================================

// IP address of the host running Ollama LXC
#define OLLAMA_HOST "192.168.1.100"
#define OLLAMA_PORT 11434
#define OLLAMA_ENDPOINT "/api/generate"

// Build the full URL: http://192.168.1.100:11434/api/generate
#define OLLAMA_URL "http://" OLLAMA_HOST ":" STRINGIFY(OLLAMA_PORT) OLLAMA_ENDPOINT

// Model to use (must be pulled in Ollama first)
// Options: qwen2.5:0.5b, qwen2.5:1.5b, llama3.2:1b, tinyllama, neural-chat:latest
#define OLLAMA_MODEL "qwen2.5:0.5b"

// HTTP request timeout
#define OLLAMA_HTTP_TIMEOUT_MS 30000

// ============================================================================
// DISPLAY CONFIGURATION (TFT_eSPI)
// ============================================================================

#define DISPLAY_WIDTH 320
#define DISPLAY_HEIGHT 240
#define DISPLAY_ROTATION 1  // 0=Portrait, 1=Landscape, 2=Reverse Portrait, 3=Reverse Landscape

// Text display parameters
#define DISPLAY_TEXT_COLOR 0xFFFF        // White
#define DISPLAY_BG_COLOR 0x0000          // Black
#define DISPLAY_INPUT_BG_COLOR 0x1082    // Dark gray
#define DISPLAY_CURSOR_COLOR 0xF800      // Red

// Font sizes (approximate character counts)
#define DISPLAY_FONT_SIZE_SMALL 1
#define DISPLAY_FONT_SIZE_MEDIUM 2
#define DISPLAY_FONT_SIZE_LARGE 4

// Line wrapping at pixel position (leaves margin for scrollbar)
#define DISPLAY_WRAP_MARGIN 310

// Maximum lines to keep in display buffer (scrolling history)
#define DISPLAY_MAX_BUFFER_LINES 100

// ============================================================================
// TOUCH INPUT CONFIGURATION (XPT2046)
// ============================================================================

// Touch controller SPI pins (shared with display on CYD)
#define TOUCH_CS_PIN 33      // Chip select
#define TOUCH_IRQ_PIN 36     // Interrupt (optional, not used in polling mode)

// Calibration values (adjust based on your specific board)
// Run the touch calibration example to get precise values
#define TOUCH_MIN_X 280
#define TOUCH_MAX_X 3870
#define TOUCH_MIN_Y 200
#define TOUCH_MAX_Y 3900

// Touch sampling
#define TOUCH_SAMPLES 20     // Number of samples for touch reading
#define TOUCH_THRESHOLD 400  // Pressure threshold (0-4095)
#define TOUCH_DEBOUNCE_MS 50 // Debounce time in milliseconds

// ============================================================================
// KEYBOARD CONFIGURATION
// ============================================================================

// On-screen keyboard dimensions
#define KEYBOARD_KEY_WIDTH 28
#define KEYBOARD_KEY_HEIGHT 24
#define KEYBOARD_KEY_GAP 2

// Display zones
#define RESPONSE_ZONE_HEIGHT 140
#define INPUT_ZONE_HEIGHT 50
#define KEYBOARD_ZONE_HEIGHT (DISPLAY_HEIGHT - RESPONSE_ZONE_HEIGHT - INPUT_ZONE_HEIGHT)

// ============================================================================
// MEMORY AND PERFORMANCE
// ============================================================================

// JSON document size (must fit a single token response)
// Ollama token JSON is typically < 512 bytes
#define JSON_DOC_SIZE 512

// HTTP stream buffer size (size of chunks to read at once)
#define HTTP_STREAM_BUFFER_SIZE 1024

// Text input buffer (max prompt length)
#define INPUT_BUFFER_SIZE 2048

// Response streaming buffer (per token chunk)
#define RESPONSE_BUFFER_SIZE 2048

// ============================================================================
// DEBUG AND LOGGING
// ============================================================================

// Enable serial debug output
#define DEBUG_ENABLED 1

#if DEBUG_ENABLED
  #define DEBUG_SERIAL Serial
  #define DEBUG_BAUD 115200
  #define DEBUG_PRINT(fmt, ...) DEBUG_SERIAL.printf("[DEBUG] " fmt "\n", ##__VA_ARGS__)
  #define DEBUG_PRINTLN(msg) DEBUG_SERIAL.println("[DEBUG] " msg)
#else
  #define DEBUG_PRINT(fmt, ...)
  #define DEBUG_PRINTLN(msg)
#endif

// ============================================================================
// MACRO HELPERS
// ============================================================================

#define STRINGIFY(x) #x

#endif // CONFIG_H
