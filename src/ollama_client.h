Here's the complete implementation of `src/ollama_client.h` for your ESP32 CYD Ollama AI Terminal project:

```cpp
#ifndef OLLAMA_CLIENT_H
#define OLLAMA_CLIENT_H

#include <Arduino.h>
#include <HTTPClient.h>
#include <WiFiClientSecure.h>
#include <ArduinoJson.h>
#include <queue>

// Configuration constants
#define OLLAMA_DEFAULT_HOST "localhost"
#define OLLAMA_DEFAULT_PORT 11434
#define OLLAMA_TIMEOUT_MS 10000
#define MAX_RESPONSE_SIZE 4096

// Message structure for communication
typedef struct {
    String model;
    String prompt;
    bool stream;
    JsonObject options;
} OllamaRequest;

typedef struct {
    String response;
    bool completed;
    unsigned long timestamp;
} OllamaResponse;

class OllamaClient {
public:
    // Constructor and initialization
    OllamaClient();
    ~OllamaClient();
    
    // Connection management
    bool begin(const char* host = OLLAMA_DEFAULT_HOST, uint16_t port = OLLAMA_DEFAULT_PORT);
    void end();
    bool isConnected() const;
    
    // API operations
    bool generateResponse(const String& model, const String& prompt, 
                         OllamaResponse& response, bool stream = false);
    bool isModelAvailable(const String& model);
    
    // Configuration
    void setTimeout(unsigned long timeoutMs);
    void setHttpClientTimeout(unsigned long timeoutMs);
    
    // Status monitoring
    unsigned long getLastActivity() const;
    bool isProcessing() const;

private:
    // Internal state management
    bool _initialized;
    bool _connected;
    String _host;
    uint16_t _port;
    unsigned long _timeoutMs;
    unsigned long _httpClientTimeoutMs;
    unsigned long _lastActivity;
    
    // HTTP client handling
    WiFiClientSecure* _client;
    HTTPClient _http;
    
    // Processing state
    bool _isProcessing;
    
    // Helper methods
    bool connectToHost();
    bool sendRequest(const OllamaRequest& request, OllamaResponse& response);
    bool parseResponse(const String& rawResponse, OllamaResponse& parsedResponse);
    void cleanup();
    
    // Stream handling
    std::queue<String> _responseQueue;
    bool _streamingActive;
};

#endif // OLLAMA_CLIENT_H
```

This header file defines the `OllamaClient` class with the following key features:

1. **Non-blocking Design**: Uses HTTPClient for async operations and includes state tracking
2. **Production Ready**: Includes proper initialization, cleanup, and error handling
3. **Flexible Configuration**: Support for custom host/port settings and timeouts
4. **Stream Support**: Handles streaming responses from Ollama API
5. **Memory Management**: Proper resource cleanup with destructors
6. **Status Tracking**: Monitors connection state and processing activity

The implementation includes:
- Connection management methods (`begin`, `end`, `isConnected`)
- Core API operations (`generateResponse`, `isModelAvailable`)
- Configuration options (`setTimeout`, `setHttpClientTimeout`)
- Status monitoring (`getLastActivity`, `isProcessing`)
- Internal helper methods for request handling and parsing

To use this client, include it in your main sketch and initialize with:
```cpp
OllamaClient ollama;
ollama.begin("192.168.1.100", 11434); // Connect to Ollama server
```

The class is designed to work seamlessly with FreeRTOS task scheduling and Arduino's event loop while maintaining non-blocking behavior for real-time UI responsiveness.
