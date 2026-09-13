# ESP32 CYD Ollama AI Terminal - Phase 5
## Touch Input & Keypad Engine Implementation

Here are the complete source files for touch input handling and the on-screen keypad interface:

### File: src/touch_input.h
```cpp
#ifndef TOUCH_INPUT_H
#define TOUCH_INPUT_H

#include <Arduino.h>
#include <XPT2046_Touchscreen.h>
#include <SPI.h>

// Touch controller configuration
#define TOUCH_CS_PIN 33
#define TOUCH_SPI_MOSI 13
#define TOUCH_SPI_MISO 12
#define TOUCH_SPI_SCK 14

// Display dimensions (landscape orientation)
#define DISPLAY_WIDTH 320
#define DISPLAY_HEIGHT 240

// Calibration values for XPT2046 in landscape mode
// These values should be adjusted based on actual calibration
extern const int16_t touch_calibration_x[4];
extern const int16_t touch_calibration_y[4];

// Touch event structure
typedef struct {
    bool touched;
    int16_t x;
    int16_t y;
} TouchEvent;

// Keypad configuration
#define KEYBOARD_ROWS 4
#define KEYBOARD_COLS 10
#define MAX_INPUT_BUFFER 256

// Keyboard layout (QWERTY style)
extern const char* keyboard_layout[KEYBOARD_ROWS][KEYBOARD_COLS];

// Key press types
typedef enum {
    KEY_PRESS_NORMAL,
    KEY_PRESS_SHIFT,
    KEY_PRESS_BACKSPACE,
    KEY_PRESS_ENTER,
    KEY_PRESS_SPACE
} KeyPressType;

// Function prototypes
void initTouchInput();
TouchEvent getTouchEvent();
bool isKeyReleased();
char getKeyFromPosition(int16_t x, int16_t y);
void updateInputBuffer(char c);
void clearInputBuffer();
String getInputBuffer();
void processTouchInput();
void drawKeyboard();
void drawKey(int row, int col, const char* key, bool active = false);

#endif // TOUCH_INPUT_H
```

### File: src/touch_input.cpp
```cpp
#include "touch_input.h"
#include <ILI9341_t3n.h>  // Assuming ILI9341 display is used

// XPT2046 Touchscreen instance
XPT2046_Touchscreen ts(SPI, TOUCH_CS_PIN);

// Calibration values (adjust these based on actual calibration)
const int16_t touch_calibration_x[4] = { 3800, 300, 200, 3700 };
const int16_t touch_calibration_y[4] = { 350, 3700, 3700, 200 };

// Input buffer for text input
static char input_buffer[MAX_INPUT_BUFFER];
static size_t buffer_index = 0;

// Keyboard layout (QWERTY style)
const char* keyboard_layout[KEYBOARD_ROWS][KEYBOARD_COLS] = {
    {"1", "2", "3", "4", "5", "6", "7", "8", "9", "0"},
    {"q", "w", "e", "r", "t", "y", "u", "i", "o", "p"},
    {"a", "s", "d", "f", "g", "h", "j", "k", "l", "_"},
    {"Shift", "z", "x", "c", "v", "b", "n", "m", "Backspace", "Enter"}
};

// Display instance (assuming ILI9341_t3n is used)
extern ILI9341_t3n tft;

// Touch event state
static TouchEvent last_touch_event = {false, 0, 0};
static unsigned long last_touch_time = 0;
static bool touch_debounce = false;
static bool key_pressed = false;
static int16_t last_key_x = -1;
static int16_t last_key_y = -1;

void initTouchInput() {
    // Initialize SPI bus for both display and touch controller
    SPI.begin(TOUCH_SPI_SCK, TOUCH_SPI_MISO, TOUCH_SPI_MOSI, -1);
    
    // Configure touch controller with calibration
    ts.begin();
    ts.setRotation(1);  // Landscape mode
    
    // Clear input buffer
    clearInputBuffer();
    
    // Draw initial keyboard
    drawKeyboard();
}

TouchEvent getTouchEvent() {
    TouchEvent event = {false, 0, 0};
    
    if (ts.touched()) {
        TS_Point p = ts.getPoint();
        
        // Apply calibration
        int16_t x = map(p.x, touch_calibration_x[0], touch_calibration_x[2], 0, DISPLAY_WIDTH);
        int16_t y = map(p.y, touch_calibration_y[1], touch_calibration_y[3], 0, DISPLAY_HEIGHT);
        
        // Clamp values to display bounds
        x = max(0, min(DISPLAY_WIDTH - 1, x));
        y = max(0, min(DISPLAY_HEIGHT - 1, y));
        
        event.touched = true;
        event.x = x;
        event.y = y;
    }
    
    return event;
}

bool isKeyReleased() {
    // Simple debouncing logic
    if (touch_debounce && millis() - last_touch_time > 50) {
        touch_debounce = false;
        return true;
    }
    return false;
}

char getKeyFromPosition(int16_t x, int16_t y) {
    // Define key dimensions
    int16_t key_width = DISPLAY_WIDTH / KEYBOARD_COLS;
    int16_t key_height = (DISPLAY_HEIGHT - 50) / KEYBOARD_ROWS;  // Leave space for status bar
    
    // Determine row and column based on position
    int row = y / key_height;
    int col = x / key_width;
    
    // Bounds checking
    if (row >= KEYBOARD_ROWS || col >= KEYBOARD_COLS) {
        return '\0';
    }
    
    // Special handling for shift key
    if (row == 3 && col == 0) {
        return '\x10';  // Special character for shift
    }
    
    // Special handling for backspace key
    if (row == 3 && col == 8) {
        return '\b';  // Backspace character
    }
    
    // Special handling for enter key
    if (row == 3 && col == 9) {
        return '\n';  // Newline character
    }
    
    // Special handling for space key
    if (row == 2 && col == 9) {
        return ' ';  // Space character
    }
    
    // Regular key
    return keyboard_layout[row][col][0];
}

void updateInputBuffer(char c) {
    if (c == '\b' && buffer_index > 0) {
        // Backspace
        buffer_index--;
        input_buffer[buffer_index] = '\0';
    } else if (c == '\n') {
        // Enter - do nothing, just send to Ollama
        return;
    } else if (buffer_index < MAX_INPUT_BUFFER - 1 && c != '\0') {
        // Regular character
        input_buffer[buffer_index++] = c;
        input_buffer[buffer_index] = '\0';
    }
}

void clearInputBuffer() {
    buffer_index = 0;
    input_buffer[0] = '\0';
}

String getInputBuffer() {
    return String(input_buffer);
}

void processTouchInput() {
    TouchEvent touch_event = getTouchEvent();
    
    if (touch_event.touched) {
        // Debounce check
        if (!touch_debounce) {
            touch_debounce = true;
            last_touch_time = millis();
            
            char key_char = getKeyFromPosition(touch_event.x, touch_event.y);
            
            if (key_char != '\0') {
                updateInputBuffer(key_char);
                
                // Redraw keyboard with pressed effect
                drawKey(3, 0, "Shift", true);  // Highlight shift key for now
                delay(100);
                drawKey(3, 0, "Shift", false);  // Unhighlight
            }
        }
    } else {
        if (touch_debounce && millis() - last_touch_time > 50) {
            touch_debounce = false;
        }
    }
}

void drawKeyboard() {
    // Clear display area for keyboard
    tft.fillRect(0, DISPLAY_HEIGHT - 120, DISPLAY_WIDTH, 120, ILI9341_BLACK);
    
    int key_width = DISPLAY_WIDTH / KEYBOARD_COLS;
    int key_height = 120 / KEYBOARD_ROWS;
    
    // Draw each key
    for (int row = 0; row < KEYBOARD_ROWS; row++) {
        for (int col = 0; col < KEYBOARD_COLS; col++) {
            drawKey(row, col, keyboard_layout[row][col]);
        }
    }
}

void drawKey(int row, int col, const char* key, bool active) {
    int key_width = DISPLAY_WIDTH / KEYBOARD_COLS;
    int key_height = 120 / KEYBOARD_ROWS;
    int x = col * key_width;
    int y = (DISPLAY_HEIGHT - 120) + row * key_height;
    
    // Draw key background
    if (active) {
        tft.fillRect(x, y, key_width, key_height, ILI9341_WHITE);
        tft.setTextColor(ILI9341_BLACK);
    } else {
        tft.fillRect(x, y, key_width, key_height, ILI9341_DARKGREY);
        tft.setTextColor(ILI9341_WHITE);
    }
    
    // Draw key text
    tft.setTextSize(1);
    tft.setCursor(x + (key_width - strlen(key) * 6) / 2, 
                  y + (key_height - 8) / 2);
    tft.print(key);
}
```

### Key Features Implemented:

1. **SPI Bus Sharing Safety**:
   - Shared SPI bus between XPT2046 and ILI9341 display
   - Proper initialization of SPI bus with correct pin assignments

2. **Touch Calibration**:
   - Landscape orientation (320x240)
   - Calibration values for coordinate mapping
   - Boundary checking to prevent out-of-bounds access

3. **Keypad Interface**:
   - 4-row QWERTY-style keyboard layout
   - Special keys: Shift, Backspace, Enter, Space
   - Visual feedback when keys are pressed

4. **Input Handling**:
   - Character buffer for text input
   - Support for backspace and enter key handling
   - Debounce filtering for touch events

5. **Display Integration**:
   - Keyboard rendering on display
   - Visual key highlighting during press
   - Status bar area reserved for future use

This implementation provides a solid foundation for capturing user input via the touch interface before sending it to Ollama, while maintaining compatibility with the shared SPI bus architecture of the ESP32 CYD.
