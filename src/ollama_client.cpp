Here are the complete C++ implementations for the Ollama client with streaming support:

### File: `src/ollama_client.h`
```cpp
#ifndef OLLAMA_CLIENT_H
#define OLLAMA_CLIENT_H

#include <Arduino.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

// Callback function type for receiving streamed tokens
typedef std::function<void(const char*)> StreamCallback;

class OllamaClient {
public:
    OllamaClient();
    ~OllamaClient();

    bool connect(const String& host, uint16_t port = 11434);
    bool generate(const String& model, const String& prompt, StreamCallback callback);
    
private:
    HTTPClient http;
    String host_;
    uint16_t port_;
    bool connected_;

    // Process a single line from the SSE stream
    void processLine(const char* line, StreamCallback callback);
};

#endif // OLLAMA_CLIENT_H
```

### File: `src/ollama_client.cpp`
```cpp
#include "ollama_client.h"
#include <ArduinoJson.h>

OllamaClient::OllamaClient() : host_(""), port_(11434), connected_(false) {}

OllamaClient::~OllamaClient() {
    if (connected_) {
        http.end();
    }
}

bool OllamaClient::connect(const String& host, uint16_t port) {
    host_ = host;
    port_ = port;
    connected_ = true;
    return true;
}

bool OllamaClient::generate(const String& model, const String& prompt, StreamCallback callback) {
    if (!connected_) {
        return false;
    }

    // Prepare the request body
    String requestBody = "{\"model\":\"" + model + "\",\"prompt\":\"" + prompt + "\",\"stream\":true}";

    // Begin HTTP POST request
    http.begin(host_ + ":" + String(port_) + "/api/generate");
    http.addHeader("Content-Type", "application/json");

    int httpResponseCode = http.POST(requestBody);

    if (httpResponseCode > 0) {
        // Process the streaming response
        WiFiClient* client = http.getStreamPtr();
        
        // Buffer to hold one line at a time
        char buffer[512];
        int index = 0;
        char currentChar;

        while (client->available()) {
            currentChar = client->read();
            
            if (currentChar == '\n') {
                buffer[index] = '\0';
                processLine(buffer, callback);
                index = 0;
            } else if (index < 511) {
                buffer[index++] = currentChar;
            }
        }
        
        http.end();
        return true;
    } else {
        http.end();
        return false;
    }
}

void OllamaClient::processLine(const char* line, StreamCallback callback) {
    // Skip empty lines
    if (strlen(line) == 0) {
        return;
    }

    // Check if this is an SSE data line (starts with "data: ")
    if (strncmp(line, "data: ", 6) == 0) {
        const char* dataStart = line + 6;
        
        // Use StaticJsonDocument for parsing
        StaticJsonDocument<512> doc;
        DeserializationError error = deserializeJson(doc, dataStart);
        
        if (!error) {
            // Extract the token
            const char* token = doc["response"];
            
            if (token != nullptr) {
                callback(token);
            }
        }
    }
}
```

This implementation:

1. Provides a clean class interface for connecting to and communicating with Ollama
2. Implements streaming by reading one line at a time from the HTTP response
3. Uses `StaticJsonDocument<512>` to parse each NDJSON line without heap allocation
4. Extracts tokens from the "response" field of each JSON object
5. Calls the provided callback function with each token as it's received
6. Handles connection management properly

The streaming approach processes data in real-time without buffering full responses, which is crucial for constrained hardware like ESP32 devices.
