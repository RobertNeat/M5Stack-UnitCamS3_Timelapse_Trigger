# M5Stack UnitCamS3 Timelapse Trigger Project

## Overview
IoT camera system for timelapse photography using M5Stack UnitCamS3 with web-based configuration interface and automatic WiFi connection with fallback to AP mode.

## Firmware (ESP32-S3)

### Critical Libraries (DO NOT CHANGE)
- **Platform**: `espressif32@6.4.0`
- **Board**: `esp32s3box` (compatible with UnitCamS3)
- **Framework**: Arduino
- **AsyncTCP**: Asynchronous TCP library for ESP32
- **ESPAsyncWebServer**: Async HTTP server for ESP32
- **ArduinoJson** `~6.21.3`: JSON serialization/deserialization
- **LittleFS**: Filesystem for serving web files and config
- **WiFi/ESPmDNS**: Network connectivity with mDNS support

### Architecture
- **HAL Layer** (`src/hal/`): Hardware abstraction for camera, SD card, microphone, config, WiFi
  - **WiFi Manager** (`hal/wifi/`): Smart WiFi connection with fallback logic
- **API Layer** (`src/apis/`): REST API endpoints for camera, system, mic, poster, shooter
- **Server Modes**:
  - AP Server: WiFi access point (fallback mode when connection fails)
  - Poster Server: Timelapse posting mode
  - Shooter Server: Interval shooting mode
- **Mooncake**: Custom logging framework with spdlog

### WiFi Connection Flow
1. **Boot Sequence**: On startup, device attempts WiFi connection with automatic fallback
2. **Connection Logic**:
   - If `wifiSsid` (user-configured) is set, use it
   - Else if `wifiSsidDefault` (from config.json) is set, use it
   - If connection fails or no credentials set, start as Access Point (`UnitCamS3-WiFi`)
3. **mDNS**: When connected in station mode, device advertises via mDNS at `{mdnsHostname}.local` (e.g., `a1camera.local`)
4. **Web Configuration**: Users can configure WiFi credentials through web interface without reflashing firmware

### Configuration System
- **Config File**: `firmware/data/config.json` in LittleFS
- **Key Properties**:
  - `wifiSsid`/`wifiPass`: User-configured WiFi credentials (set via web UI)
  - `wifiSsidDefault`/`wifiPassDefault`: Default WiFi credentials (set before flashing)
  - `mdnsHostname`: mDNS hostname for network discovery (default: `a1camera`)
  - `nickname`, `postInterval`, `timeZone`: Device settings
  - `startPoster`/`startShooter`: Operating mode flags
- **Priority**: User credentials override defaults; defaults provide backup connectivity

### Key Features
- **Smart WiFi Connection**: Automatic connection with multi-level fallback
- **Zero-Touch Setup**: Pre-configure default WiFi for deployment
- **Fallback AP Mode**: Always accessible for configuration if WiFi fails
- **mDNS Discovery**: Access device by hostname instead of IP
- **Persistent Configuration**: All settings stored in LittleFS config.json
- **RESTful API**: Endpoints served at `/api/v1/*`
  - `/api/v1/get_config`: Fetch current configuration
  - `/api/v1/set_config`: Update configuration (saves WiFi credentials)
  - `/api/v1/get_wifi_list`: Scan available WiFi networks

## Web Interface

### Technologies
- **Build Tool**: Vite 5.0
- **Framework**: React 18.2 with TypeScript 5.2
- **UI Library**: NextUI 2.2.9 (React component library)
- **Styling**: TailwindCSS 3.3.5
- **Routing**: React Router DOM 6.19
- **Animation**: Framer Motion 10.16
- **Build Output**: Single-file HTML (via `vite-plugin-singlefile`)
- **Compression**: Gzip compression (via `vite-plugin-compression`)

### Development Tools
- **JSON Server**: Mock API server for development (`json-server`)
- **Linting**: ESLint with TypeScript support
- **Dev Proxy**: Vite proxies `/api` to `localhost:3000`

### Interaction with Firmware
1. **Static File Serving**: Compiled web app (single HTML file) served from LittleFS at `/`
2. **REST API Communication**:
   - Web makes fetch requests to `/api/v1/*` endpoints
   - Firmware responds with JSON (using ArduinoJson)
   - Examples: `/api/v1/get_config`, `/api/v1/wifi_scan`, `/api/v1/capture`
3. **Configuration Flow**:
   - Web fetches device config on load
   - User modifies settings in React UI
   - POST requests update firmware config
   - Firmware saves to LittleFS config.json
4. **Real-time Features**: Async handlers enable non-blocking camera operations

### Build Process
- `npm run build` → Vite compiles React/TS → Single compressed HTML file
- Upload to firmware's `data/` folder → Flash to LittleFS → Served at device IP
