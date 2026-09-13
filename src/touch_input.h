# src/touch_input.h

```cpp
#ifndef TOUCH_INPUT_H
#define TOUCH_INPUT_H

#include <Arduino.h>
#include <XPT2046_Touchscreen.h>
#include <SPI.h>

// Touch configuration
#define TOUCH_CS_PIN    33
#define TOUCH_IRQ_PIN   32
#define TOUCH_SPI_FREQ  2500000

// Touch calibration values (adjust for your specific display)
#define TOUCH_MIN_X     200
#define TOUCH_MAX_X     3800
#define TOUCH_MIN_Y     200
#define TOUCH_MAX_Y     3800

// Key definitions
#define KEY_NONE        0
#define KEY_UP          1
#define KEY_DOWN        2
#define KEY_LEFT        3
#define KEY_RIGHT       4
#define KEY_SELECT      5
#define KEY_BACKSPACE   6
#define KEY_ENTER       7
#define KEY_SPACE       8

// Touch event types
typedef enum {
    TOUCH_EVENT_NONE = 0,
    TOUCH_EVENT_PRESSED,
    TOUCH_EVENT_RELEASED,
    TOUCH_EVENT_MOVED
} touch_event_t;

// Touch point structure
typedef struct {
    int16_t x;
    int16_t y;
    bool valid;
} touch_point_t;

// Key press structure
typedef struct {
    uint8_t key;
    bool pressed;
} key_press_t;

/**
 * @brief Touch input manager class
 * 
 * Handles touch screen input and keyboard emulation for ESP32 CYD
 */
class TouchInput {
public:
    /**
     * @brief Initialize the touch input system
     * 
     * @return true if initialization successful, false otherwise
     */
    bool begin();
    
    /**
     * @brief Update touch input state
     * 
     * Should be called regularly in the main loop
     * 
     * @return true if new touch data is available, false otherwise
     */
    bool update();
    
    /**
     * @brief Check for key press events
     * 
     * @param key Pointer to store the detected key (0 = no key)
     * @return true if a key was pressed, false otherwise
     */
    bool getKeyPress(uint8_t* key);
    
    /**
     * @brief Get current touch point
     * 
     * @param point Pointer to touch_point_t structure to fill
     * @return true if valid touch point available, false otherwise
     */
    bool getTouchPoint(touch_point_t* point);
    
    /**
     * @brief Check if touch is currently pressed
     * 
     * @return true if screen is being touched, false otherwise
     */
    bool isPressed();
    
    /**
     * @brief Get touch event type
     * 
     * @return Current touch event type
     */
    touch_event_t getTouchEvent();
    
    /**
     * @brief Set touch calibration values
     * 
     * @param min_x Minimum X value
     * @param max_x Maximum X value
     * @param min_y Minimum Y value
     * @param max_y Maximum Y value
     */
    void setCalibration(int16_t min_x, int16_t max_x, int16_t min_y, int16_t max_y);
    
    /**
     * @brief Calibrate touch screen
     * 
     * This function should be called once during setup to calibrate the touch screen
     */
    void calibrate();
    
private:
    XPT2046_Touchscreen* ts;           // Touch screen object
    touch_point_t lastPoint;           // Last known touch point
    touch_point_t currentPoint;        // Current touch point
    touch_event_t currentEvent;        // Current touch event
    bool isCurrentlyPressed;           // Flag for current press state
    
    // Calibration values
    int16_t cal_min_x, cal_max_x;
    int16_t cal_min_y, cal_max_y;
    
    // Key mapping and detection
    uint8_t lastKey;                   // Last detected key
    unsigned long lastKeyTime;         // Time of last key press
    
    /**
     * @brief Process touch data and detect key presses
     */
    void processTouchData();
    
    /**
     * @brief Map raw touch coordinates to display coordinates
     * 
     * @param raw_x Raw X coordinate from touch controller
     * @param raw_y Raw Y coordinate from touch controller
     * @return Mapped touch point structure
     */
    touch_point_t mapTouchCoordinates(int16_t raw_x, int16_t raw_y);
    
    /**
     * @brief Check if a key is pressed at given coordinates
     * 
     * @param x X coordinate to check
     * @param y Y coordinate to check
     * @return Key code if pressed, KEY_NONE otherwise
     */
    uint8_t checkForKeyPress(int16_t x, int16_t y);
};

// Global instance
extern TouchInput touchInput;

#endif // TOUCH_INPUT_H
```
