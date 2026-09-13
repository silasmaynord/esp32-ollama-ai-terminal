# src/ui_manager.h

```cpp
#ifndef UI_MANAGER_H
#define UI_MANAGER_H

#include <Arduino.h>
#include "display_driver.h"
#include "touch_input.h"
#include "ollama_client.h"
#include "wifi_manager.h"

// UI States
typedef enum {
    UI_STATE_IDLE,
    UI_STATE_CONNECTING,
    UI_STATE_CHAT_INPUT,
    UI_STATE_PROCESSING,
    UI_STATE_RESPONSE,
    UI_STATE_ERROR
} ui_state_t;

// Chat message structure
typedef struct {
    String text;
    bool is_user;
    uint32_t timestamp;
} chat_message_t;

// UI Manager class
class UIManager {
public:
    // Constructor
    UIManager(DisplayDriver* display, TouchInput* touch, OllamaClient* ollama, WiFiManager* wifi);
    
    // Initialize UI manager
    void begin();
    
    // Main update loop - should be called in main loop
    void update();
    
    // Handle touch events
    void handleTouch(int x, int y);
    
    // Handle keyboard input
    void handleKeyboard(char key);
    
    // Set current state
    void setState(ui_state_t state);
    
    // Get current state
    ui_state_t getState() const;
    
    // Add message to chat history
    void addMessage(const String& text, bool is_user);
    
    // Clear chat history
    void clearChat();
    
    // Show connection status
    void showConnectionStatus(bool connected);
    
    // Show error message
    void showError(const String& error);
    
    // Show processing indicator
    void showProcessing(const String& message = "Thinking...");
    
    // Show response from Ollama
    void showResponse(const String& response);
    
    // Draw chat input area
    void drawChatInput();
    
    // Draw main UI elements
    void drawMainUI();
    
    // Draw welcome screen
    void drawWelcomeScreen();
    
    // Draw loading spinner
    void drawSpinner(int x, int y, int radius, int angle);
    
private:
    DisplayDriver* display_;
    TouchInput* touch_;
    OllamaClient* ollama_;
    WiFiManager* wifi_;
    
    ui_state_t current_state_;
    uint32_t last_update_time_;
    
    // Chat history
    chat_message_t chat_history_[20];  // Max 20 messages
    int chat_count_;
    
    // UI elements
    bool show_spinner_;
    uint32_t spinner_start_time_;
    int spinner_angle_;
    
    // Input handling
    String input_buffer_;
    bool input_dirty_;
    
    // Drawing helper functions
    void drawHeader();
    void drawChatHistory();
    void drawInputArea();
    void drawButton(int x, int y, int width, int height, const String& label, bool active = false);
    void drawProgressBar(int x, int y, int width, int height, float progress);
    
    // State handling
    void handleIdleState();
    void handleConnectingState();
    void handleChatInputState();
    void handleProcessingState();
    void handleResponseState();
    void handleErrorState();
    
    // Utility functions
    String truncateString(const String& str, int max_chars);
    void resetInputBuffer();
};

#endif // UI_MANAGER_H
```

This header file defines the `UIManager` class for the ESP32 CYD Ollama AI Terminal. It includes:

1. **UI State Management** - Enum defining different UI states (idle, connecting, chat input, processing, response, error)
2. **Data Structures** - Chat message structure and UI manager class
3. **Core Functions**:
   - Initialization and update loop
   - Touch and keyboard event handling
   - State management
   - Chat history management
   - UI rendering functions
4. **Dependencies** - Includes required headers for display, touch, Ollama client, and WiFi manager
5. **Non-blocking Design** - Uses time-based updates and state management for smooth operation
6. **Production Ready Features** - Error handling, input validation, and resource management

The implementation allows for a responsive UI that works well with the ESP32's limited resources while maintaining a clean separation of concerns between UI rendering, input handling, and system integration.
