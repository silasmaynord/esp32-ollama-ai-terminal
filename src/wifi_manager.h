# src/wifi_manager.h

```cpp
#ifndef WIFI_MANAGER_H
#define WIFI_MANAGER_H

#include <WiFi.h>
#include <Arduino.h>
#include "config.h"

// WiFi connection states
typedef enum {
    WIFI_DISCONNECTED = 0,
    WIFI_CONNECTING,
    WIFI_CONNECTED,
    WIFI_CONNECTION_FAILED
} wifi_state_t;

// WiFi credentials structure
typedef struct {
    const char* ssid;
    const char* password;
} wifi_credentials_t;

// Callback function type for WiFi state changes
typedef void (*wifi_state_callback_t)(wifi_state_t state, const char* message);

/**
 * @brief WiFi Manager class for handling ESP32 WiFi connections
 * 
 * This class manages WiFi connection lifecycle including:
 * - Automatic reconnection attempts
 * - Connection status monitoring
 * - Configuration of multiple networks
 * - Event callbacks for connection state changes
 */
class WiFiManager {
public:
    /**
     * @brief Construct a new WiFi Manager object
     * 
     * @param callback Optional callback function for WiFi state changes
     */
    WiFiManager(wifi_state_callback_t callback = nullptr);
    
    /**
     * @brief Initialize the WiFi manager
     * 
     * This method should be called once during system initialization.
     * It sets up the WiFi interface and begins connection attempts.
     */
    void begin();
    
    /**
     * @brief Update WiFi connection status
     * 
     * This method should be called regularly in the main loop to
     * handle connection state changes and reconnection attempts.
     */
    void update();
    
    /**
     * @brief Connect to a specific WiFi network
     * 
     * @param ssid WiFi network SSID
     * @param password WiFi network password
     * @return true if connection started successfully, false otherwise
     */
    bool connect(const char* ssid, const char* password);
    
    /**
     * @brief Connect to the default network (from config.h)
     * 
     * @return true if connection started successfully, false otherwise
     */
    bool connectDefault();
    
    /**
     * @brief Disconnect from current WiFi network
     */
    void disconnect();
    
    /**
     * @brief Get current WiFi connection state
     * 
     * @return Current WiFi state
     */
    wifi_state_t getState() const;
    
    /**
     * @brief Get the IP address of the connected network
     * 
     * @return IP address as string, empty string if not connected
     */
    String getIP() const;
    
    /**
     * @brief Check if currently connected to WiFi
     * 
     * @return true if connected, false otherwise
     */
    bool isConnected() const;
    
    /**
     * @brief Set callback function for WiFi state changes
     * 
     * @param callback Callback function to be called on state changes
     */
    void setStateCallback(wifi_state_callback_t callback);
    
    /**
     * @brief Get the currently connected network SSID
     * 
     * @return SSID string, empty if not connected
     */
    String getSSID() const;
    
    /**
     * @brief Get signal strength of current connection
     * 
     * @return RSSI value in dBm, or 0 if not connected
     */
    int8_t getSignalStrength() const;

private:
    // WiFi state tracking
    wifi_state_t _state;
    
    // Connection parameters
    wifi_credentials_t _credentials;
    
    // Callback function for state changes
    wifi_state_callback_t _callback;
    
    // Connection attempt tracking
    unsigned long _lastConnectionAttempt;
    uint8_t _connectionAttempts;
    const uint8_t _maxConnectionAttempts;
    
    // Network configuration
    const uint32_t _connectionTimeoutMs;
    const uint32_t _reconnectIntervalMs;
    
    // Internal helper methods
    void _handleStateChange(wifi_state_t newState, const char* message = nullptr);
    bool _isConnected();
    void _attemptConnection();
    void _printWiFiInfo();
};

// Global instance of WiFi manager
extern WiFiManager wifiManager;

#endif // WIFI_MANAGER_H
```
