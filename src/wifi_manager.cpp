/**
 * @file wifi_manager.cpp
 * @brief WiFi connectivity implementation using ESP32 WiFi API
 * 
 * Implements the WiFi manager interface with:
 * - Station (STA) mode connection
 * - Automatic reconnection with backoff
 * - Network status monitoring
 * - Ollama host connectivity testing
 */

#include "wifi_manager.h"
#include "config.h"

// ============================================================================
// Private State Variables
// ============================================================================

static WiFiStatus current_status = WIFI_DISCONNECTED;
static unsigned long last_reconnect_attempt = 0;
static unsigned int reconnect_attempts = 0;
static const unsigned long RECONNECT_BACKOFF_MS = 5000; // 5 second backoff

// ============================================================================
// Public Implementation
// ============================================================================

void wifi_init() {
  DEBUG_PRINTLN("Initializing WiFi in station (STA) mode...");
  
  // Set WiFi mode to station only (STA)
  WiFi.mode(WIFI_STA);
  
  // Disable power saving for faster response (we're always running)
  WiFi.setSleep(false);
  
  // Set hostname for network discovery
  WiFi.setHostname("esp32-ollama-terminal");
  
  // Enable DHCP (default, but explicit)
  WiFi.enableIpV6();
  
  current_status = WIFI_DISCONNECTED;
  reconnect_attempts = 0;
  
  DEBUG_PRINTLN("✓ WiFi initialization complete");
}

bool wifi_connect() {
  DEBUG_PRINTF("Connecting to WiFi network: %s\n", WIFI_SSID);
  current_status = WIFI_CONNECTING;
  
  // Disconnect first (clear any previous connection)
  WiFi.disconnect(true); // true = turn off radio
  delay(100);
  
  // Begin connection attempt
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  
  // Wait for IP address with timeout
  unsigned long start_time = millis();
  int dot_counter = 0;
  
  while (millis() - start_time < WIFI_CONNECT_TIMEOUT_MS) {
    if (WiFi.status() == WL_CONNECTED) {
      current_status = WIFI_CONNECTED;
      reconnect_attempts = 0; // Reset counter on successful connection
      
      DEBUG_PRINTLN("\n✓ WiFi connected!");
      wifi_print_status();
      return true;
    }
    
    // Print progress indicator
    if (dot_counter++ % 10 == 0) {
      DEBUG_PRINT(".");
    }
    delay(100);
  }
  
  // Timeout reached
  current_status = WIFI_ERROR;
  DEBUG_PRINTLN("\n✗ WiFi connection timeout");
  DEBUG_PRINTF("  Status: %d (expected 3 for WL_CONNECTED)\n", WiFi.status());
  
  return false;
}

WiFiStatus wifi_get_status() {
  return current_status;
}

bool wifi_is_connected() {
  return (WiFi.status() == WL_CONNECTED && WiFi.localIP() != INADDR_NONE);
}

String wifi_get_ip_address() {
  if (wifi_is_connected()) {
    return WiFi.localIP().toString();
  }
  return "0.0.0.0";
}

int wifi_get_signal_strength() {
  if (wifi_is_connected()) {
    return WiFi.RSSI();
  }
  return 0;
}

String wifi_get_ssid() {
  if (wifi_is_connected()) {
    return String(WiFi.SSID());
  }
  return "";
}

bool wifi_reconnect_if_needed() {
  // If already connected, no action needed
  if (wifi_is_connected()) {
    return true;
  }
  
  // Check if enough time has passed since last attempt (backoff)
  unsigned long now = millis();
  if (now - last_reconnect_attempt < RECONNECT_BACKOFF_MS) {
    return false;
  }
  
  last_reconnect_attempt = now;
  reconnect_attempts++;
  
  DEBUG_PRINTF("Reconnection attempt #%u...\n", reconnect_attempts);
  
  // Try to reconnect
  bool success = wifi_connect();
  
  if (!success && reconnect_attempts >= WIFI_MAX_ATTEMPTS) {
    DEBUG_PRINTF("Max reconnection attempts (%u) reached\n", WIFI_MAX_ATTEMPTS);
    current_status = WIFI_ERROR;
    return false;
  }
  
  return success;
}

void wifi_disconnect() {
  DEBUG_PRINTLN("Disconnecting from WiFi...");
  WiFi.disconnect(true); // true = turn off radio
  current_status = WIFI_DISCONNECTED;
  DEBUG_PRINTLN("✓ WiFi disconnected");
}

void wifi_print_status() {
  DEBUG_PRINTLN("\n╔═══════════════════════════════════════════════════════╗");
  
  if (wifi_is_connected()) {
    DEBUG_PRINTLN("║ WiFi Status: CONNECTED ✓                              ║");
    DEBUG_PRINTF("║ SSID: %-48s ║\n", WiFi.SSID().c_str());
    DEBUG_PRINTF("║ IP Address: %-43s ║\n", wifi_get_ip_address().c_str());
    DEBUG_PRINTF("║ Gateway: %-46s ║\n", WiFi.gatewayIP().toString().c_str());
    DEBUG_PRINTF("║ DNS: %-49s ║\n", WiFi.dnsIP().toString().c_str());
    DEBUG_PRINTF("║ Signal Strength: %d dBm (", wifi_get_signal_strength());
    
    int rssi = wifi_get_signal_strength();
    if (rssi > -50) {
      DEBUG_PRINT("Excellent");
    } else if (rssi > -60) {
      DEBUG_PRINT("Good");
    } else if (rssi > -70) {
      DEBUG_PRINT("Fair");
    } else {
      DEBUG_PRINT("Weak");
    }
    DEBUG_PRINTLN(") ║");
  } else {
    DEBUG_PRINTF("║ WiFi Status: %s\n", 
      (current_status == WIFI_CONNECTING) ? "CONNECTING..." :
      (current_status == WIFI_ERROR) ? "ERROR ✗" : "DISCONNECTED");
  }
  
  DEBUG_PRINTLN("╚═══════════════════════════════════════════════════════╝\n");
}

bool wifi_test_ollama_connectivity() {
  if (!wifi_is_connected()) {
    DEBUG_PRINTLN("✗ Cannot test Ollama connectivity: WiFi not connected");
    return false;
  }
  
  DEBUG_PRINTF("Testing connectivity to Ollama host: %s:%d\n", OLLAMA_HOST, OLLAMA_PORT);
  
  WiFiClient client;
  client.setTimeout(3000); // 3 second timeout
  
  // Attempt to connect to Ollama host
  if (!client.connect(OLLAMA_HOST, OLLAMA_PORT)) {
    DEBUG_PRINTLN("✗ Cannot connect to Ollama host (connection failed)");
    return false;
  }
  
  // Successfully connected - verify we can send HTTP request
  DEBUG_PRINTLN("✓ Connected to Ollama host");
  
  // Send a simple HTTP GET to /api/tags to verify API is responding
  client.print("GET /api/tags HTTP/1.1\r\n");
  client.print("Host: ");
  client.print(OLLAMA_HOST);
  client.print(":");
  client.print(OLLAMA_PORT);
  client.print("\r\n");
  client.print("Connection: close\r\n");
  client.print("\r\n");
  client.flush();
  
  // Wait for response
  delay(100);
  
  String response = "";
  while (client.available()) {
    response += (char)client.read();
  }
  client.stop();
  
  // Check for HTTP 200 OK response
  if (response.indexOf("200 OK") >= 0 || response.indexOf("HTTP/1.") >= 0) {
    DEBUG_PRINTLN("✓ Ollama API is responding correctly");
    return true;
  } else {
    DEBUG_PRINTLN("✗ Ollama API not responding correctly");
    return false;
  }
}

// ============================================================================
// Integration with main.cpp
// ============================================================================

/**
 * @brief Called from main setup() - Phase 2 initialization hook
 * 
 * This function is called from main.cpp during setup sequence
 */
void setup_wifi_phase2() {
  // Initialize WiFi hardware
  wifi_init();
  
  // Attempt initial connection
  if (!wifi_connect()) {
    DEBUG_PRINTLN("⚠ Initial WiFi connection failed - will retry in loop");
  }
  
  // If connected, test Ollama connectivity
  if (wifi_is_connected()) {
    delay(500); // Brief delay to ensure network is stable
    if (!wifi_test_ollama_connectivity()) {
      DEBUG_PRINTLN("⚠ Warning: Ollama host may not be reachable");
    }
  }
}

/**
 * @brief Called from main loop() - Periodic WiFi maintenance
 * 
 * Call this regularly (every loop iteration) to maintain WiFi connection
 * and handle reconnection if needed
 */
void loop_wifi_phase2() {
  // Check if we need to reconnect
  if (!wifi_is_connected()) {
    wifi_reconnect_if_needed();
  }
}
