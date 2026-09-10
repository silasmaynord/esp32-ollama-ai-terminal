# Development Phases

This project is broken into 6 sequential phases to manage complexity and allow for incremental testing.

## Phase 1: Project Foundation ✅ COMPLETE

**Deliverables:**
- ✅ PlatformIO configuration (`platformio.ini`)
- ✅ Main application skeleton (`src/main.cpp`)
- ✅ Configuration header (`src/config.h`)
- ✅ Hardware documentation (`docs/HARDWARE.md`)
- ✅ Project README
- ✅ .gitignore

**Status:** Foundation complete. Ready to begin Phase 2.

---

## Phase 2: WiFi Connectivity (NEXT)

**Objective:** Establish stable WiFi connection to local network and verify Ollama host is reachable.

**Deliverables:**
- `src/wifi_manager.h` — WiFi interface
- `src/wifi_manager.cpp` — WiFi implementation
- SSID/password configuration from `config.h`
- WiFi status logging
- Automatic reconnection logic
- IP address acquisition and verification

**Implementation Strategy:**
1. Initialize WiFi in station (STA) mode
2. Connect with SSID/password from `config.h`
3. Wait for IP address with timeout
4. Test HTTP connectivity to Ollama host
5. Display WiFi status on serial console

**Testing:**
- Verify connection to 2.4 GHz WiFi network
- Confirm IP address is assigned
- Test DNS resolution and ping to Ollama host
- Measure WiFi signal strength (RSSI)

---

## Phase 3: Ollama Client (THEN)

**Objective:** Implement HTTP streaming client for Ollama API.

**Deliverables:**
- `src/ollama_client.h` — Ollama API interface
- `src/ollama_client.cpp` — Streaming HTTP implementation
- Token-by-token parsing from NDJSON response
- Error handling and retry logic
- Response buffering

**Implementation Strategy:**
1. Create HTTP POST request to `/api/generate`
2. Set `stream: true` in JSON payload
3. Read HTTP response stream byte-by-byte
4. Parse NDJSON tokens using ArduinoJson
5. Extract `response` field from each token
6. Handle `done: true` to mark completion

**Testing:**
- Send test prompt to Ollama
- Verify chunked HTTP response parsing
- Confirm token extraction and buffering
- Test timeout and error handling

---

## Phase 4: Display Driver (THEN)

**Objective:** Render text to ILI9341 display with proper formatting and scrolling.

**Deliverables:**
- `src/display_driver.h` — Display interface
- `src/display_driver.cpp` — TFT_eSPI wrapper
- Text rendering with word wrapping
- Scrollable response buffer
- Input line display
- Status bar (WiFi, model, etc.)

**Implementation Strategy:**
1. Initialize TFT_eSPI with correct SPI pins
2. Set display rotation and colors
3. Implement text wrapping at `DISPLAY_WRAP_MARGIN`
4. Create circular buffer for response history
5. Update display asynchronously to avoid blocking

**Testing:**
- Display splash screen on startup
- Render static text with proper formatting
- Test word wrapping at screen edges
- Verify scrolling with large responses

---

## Phase 5: Touch Input (THEN)

**Objective:** Read touchscreen and provide keyboard input interface.

**Deliverables:**
- `src/touch_input.h` — Touch interface
- `src/touch_input.cpp` — XPT2046 and keyboard handling
- On-screen QWERTY keyboard rendering
- Touch coordinate calibration
- Input buffering and echo
- Backspace/delete handling

**Implementation Strategy:**
1. Initialize XPT2046 touch controller
2. Read raw touch ADC values
3. Map raw coordinates to display pixels
4. Detect key presses on keyboard
5. Accumulate input in buffer
6. Display typed characters on screen

**Testing:**
- Run touch calibration utility
- Verify single and multi-touch
- Test keyboard key detection
- Verify input buffer accuracy

---

## Phase 6: UI Manager & Integration (FINALLY)

**Objective:** Orchestrate all components into a cohesive application.

**Deliverables:**
- `src/ui_manager.h` — State machine interface
- `src/ui_manager.cpp` — Application state manager
- Main event loop
- Response streaming to display
- Input validation and submission
- Error dialogs and status messages

**Implementation Strategy:**
1. Create state machine (IDLE → INPUT → WAITING → STREAMING → COMPLETE)
2. Wire input → Ollama client → display in sequence
3. Handle WiFi reconnection in background
4. Implement graceful error handling
5. Update status bar with state information

**Testing:**
- Full end-to-end prompt → response cycle
- Multiple sequential prompts
- WiFi disconnection recovery
- Memory usage monitoring
- Timeout handling

---

## Summary Timeline

| Phase | Component | Est. Time | Dependencies |
|-------|-----------|-----------|---------------|
| 1 | Foundation | ✅ Done | None |
| 2 | WiFi | Next | Phase 1 |
| 3 | Ollama | After 2 | Phase 1-2 |
| 4 | Display | After 3 | Phase 1 |
| 5 | Touch | After 4 | Phase 1, 4 |
| 6 | Integration | After 5 | Phase 1-5 |

**Total Estimated Duration:** 1-2 weeks for full implementation with testing.

---

## Testing Strategy

### Unit Testing
- Each phase is tested in isolation
- Serial console output for debugging
- Example sketches demonstrating each component

### Integration Testing
- Components integrated step-by-step
- Full system test at end of Phase 6
- Memory profiling after each phase

### System Testing
- Long-running stability tests
- WiFi reconnection scenarios
- Ollama timeout handling
- Memory leak detection
