// src/touch_input.cpp

``cpp
#ifndef DISPLAY_DRIVER_H
#define DISPLAY_DRIVER_H

#include <TFT_eSPI.h>
#include <Arduino.h>

// Terminal configuration
#define TERMINAL_WIDTH 310      // Max width for text wrapping (pixels)
#define TERMINAL_HEIGHT 240     // Screen height in pixels
#define MAX_LINES 50            // Maximum lines in terminal buffer
#define MAX_CHARS_PER_LINE 100  // Max characters per line

// Scroll constants
#define SCROLL_THRESHOLD 220    // When to start scrolling (pixels from bottom)
#define SCROLL_MARGIN 10        // Margin before actual scroll begins

class DisplayDriver {
private:
    TFT_eSPI* tft;
    
    // Terminal state
    uint16_t cursor_x;
    uint16_t cursor_y;
    uint16_t line_count;
    uint16_t current_line;
    
    // Scroll state
    bool needs_scroll;
    uint16_t scroll_offset;
    
    // Line buffer for text storage
    char* line_buffer[MAX_LINES];
    uint16_t line_lengths[MAX_LINES];
    
    // Character streaming parameters
    unsigned long last_char_time;
    uint32_t char_delay_ms;
    
    // Internal helper functions
    void wrap_text(const char* text, uint16_t max_width);
    void draw_line(uint16_t y_pos, const char* line_text, uint16_t length);
    void scroll_display();
    bool is_char_ready();
    
public:
    DisplayDriver(TFT_eSPI* display);
    ~DisplayDriver();
    
    // Terminal operations
    void begin();
    void clear_screen();
    void write_char(char c);
    void write_string(const char* str);
    void newline();
    void set_cursor_pos(uint16_t x, uint16_t y);
    void set_char_delay(uint32_t delay_ms);
    
    // Scroll handling
    void check_scroll();
    void force_scroll();
    
    // Utility functions
    uint16_t get_cursor_x() const { return cursor_x; }
    uint16_t get_cursor_y() const { return cursor_y; }
    uint16_t get_line_count() const { return line_count; }
};

#endif // DISPLAY_DRIVER_H
```

# src/display_driver.cpp

```cpp
#include "display_driver.h"

DisplayDriver::DisplayDriver(TFT_eSPI* display) : tft(display) {
    cursor_x = 0;
    cursor_y = 0;
    line_count = 0;
    current_line = 0;
    needs_scroll = false;
    scroll_offset = 0;
    last_char_time = 0;
    char_delay_ms = 0; // No delay by default
    
    // Initialize line buffers
    for (int i = 0; i < MAX_LINES; i++) {
        line_buffer[i] = nullptr;
        line_lengths[i] = 0;
    }
}

DisplayDriver::~DisplayDriver() {
    // Free allocated memory
    for (int i = 0; i < MAX_LINES; i++) {
        if (line_buffer[i]) {
            free(line_buffer[i]);
            line_buffer[i] = nullptr;
        }
    }
}

void DisplayDriver::begin() {
    tft->fillScreen(TFT_BLACK);
    tft->setTextColor(TFT_YELLOW, TFT_BLACK);
    tft->setTextSize(1);
    tft->setFont(&FreeSans9pt7b); // Use a fixed-width font for consistent character widths
    cursor_x = 0;
    cursor_y = 0;
}

void DisplayDriver::clear_screen() {
    tft->fillScreen(TFT_BLACK);
    cursor_x = 0;
    cursor_y = 0;
    line_count = 0;
    current_line = 0;
    scroll_offset = 0;
    
    // Clear line buffer
    for (int i = 0; i < MAX_LINES; i++) {
        if (line_buffer[i]) {
            free(line_buffer[i]);
            line_buffer[i] = nullptr;
        }
        line_lengths[i] = 0;
    }
}

void DisplayDriver::write_char(char c) {
    // Check if character delay is set
    if (char_delay_ms > 0 && !is_char_ready()) {
        return;
    }
    
    last_char_time = millis();
    
    switch (c) {
        case '\n':
            newline();
            break;
            
        case '\r':
            cursor_x = 0;
            break;
            
        case '\b': // Backspace
            if (cursor_x > 0) {
                cursor_x -= 6; // Approximate width of a character
                tft->drawFastHLine(cursor_x, cursor_y, 6, TFT_BLACK);
                tft->drawChar(cursor_x, cursor_y, ' ', TFT_YELLOW, TFT_BLACK, 1);
            }
            break;
            
        default:
            // Draw character at current position
            tft->drawChar(cursor_x, cursor_y, c, TFT_YELLOW, TFT_BLACK, 1);
            
            // Update cursor position
            cursor_x += 6; // Approximate width of a character
            
            // Check for word wrap or line break
            if (cursor_x >= TERMINAL_WIDTH) {
                newline();
            }
            break;
    }
}

void DisplayDriver::write_string(const char* str) {
    while (*str) {
        write_char(*str++);
    }
}

void DisplayDriver::newline() {
    cursor_x = 0;
    cursor_y += 12; // Height of font used
    
    // Check if we need to scroll
    if (cursor_y >= TERMINAL_HEIGHT - SCROLL_MARGIN) {
        check_scroll();
    }
    
    // Handle line overflow
    if (line_count >= MAX_LINES) {
        // Shift lines up
        for (int i = 0; i < MAX_LINES - 1; i++) {
            line_buffer[i] = line_buffer[i + 1];
            line_lengths[i] = line_lengths[i + 1];
        }
        line_count--;
        
        // Clear the last line in display
        tft->fillRect(0, TERMINAL_HEIGHT - 12, TERMINAL_WIDTH, 12, TFT_BLACK);
    }
    
    line_count++;
}

void DisplayDriver::set_cursor_pos(uint16_t x, uint16_t y) {
    cursor_x = x;
    cursor_y = y;
}

void DisplayDriver::set_char_delay(uint32_t delay_ms) {
    char_delay_ms = delay_ms;
}

void DisplayDriver::check_scroll() {
    if (cursor_y >= TERMINAL_HEIGHT - SCROLL_MARGIN) {
        force_scroll();
    }
}

void DisplayDriver::force_scroll() {
    // Move all existing text up by one line
    tft->pushRect(0, 12, TERMINAL_WIDTH, TERMINAL_HEIGHT - 12, 
                  (uint16_t*)tft->getBuffer());
    
    // Clear the last line
    tft->fillRect(0, TERMINAL_HEIGHT - 12, TERMINAL_WIDTH, 12, TFT_BLACK);
    
    // Reset cursor position
    cursor_y = TERMINAL_HEIGHT - 12;
    cursor_x = 0;
}

bool DisplayDriver::is_char_ready() {
    return (millis() - last_char_time) >= char_delay_ms;
}
```
