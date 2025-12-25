# Project Changes Summary

## Overview
Redesigned WiFi connectivity system to support automatic connection with fallback to AP mode, eliminating the need to reflash firmware for WiFi configuration changes.

## What Changed

### 1. Configuration System
**File: `firmware/data/config.json`**
- Added `wifiSsidDefault` and `wifiPassDefault` for pre-configured default WiFi
- Added `mdnsHostname` for mDNS network discovery
- User-configured credentials (`wifiSsid`/`wifiPass`) now override defaults

### 2. HAL Layer Updates
**Files Modified:**
- `firmware/src/hal/hal.h` - Added new config fields to `Config_t` struct
- `firmware/src/hal/config/hal_config.cpp` - Updated serialization/deserialization for new fields

**Files Created:**
- `firmware/src/hal/wifi/hal_wifi.h` - WiFi manager interface
- `firmware/src/hal/wifi/hal_wifi.cpp` - WiFi connection logic with fallback

### 3. WiFi Connection Logic
**New Features:**
- **Priority 1**: Attempt connection to user-configured WiFi (`wifiSsid`/`wifiPass`)
- **Priority 2**: Fallback to default WiFi (`wifiSsidDefault`/`wifiPassDefault`)
- **Priority 3**: Start Access Point mode (`UnitCamS3-WiFi`) if both fail
- **mDNS Support**: Device advertises as `{mdnsHostname}.local` when in station mode

### 4. Server Updates
**File: `firmware/src/servers/server_ap.cpp`**
- Integrated WiFiManager for intelligent connection handling
- Automatic mDNS initialization in station mode
- Improved logging for connection status

### 5. API Updates
**File: `firmware/src/apis/system/api_system.cpp`**
- Updated `setConfig()` to preserve default WiFi credentials
- Now uses incremental updates instead of full replacement
- Supports setting user WiFi without affecting defaults

### 6. Web Interface
**File: `web/src/components/shooter/card-shooter-config.tsx`**
- ✅ Added WiFi configuration UI section
- WiFi SSID selection dropdown (populated by scan)
- Manual WiFi SSID text input field
- WiFi password input field
- "Scan WiFi" button with loading state
- WiFi credentials displayed in confirmation modal
- Integrates with existing `/api/v1/get_wifi_list` and `/api/v1/set_config` endpoints

### 7. Documentation
**Files Created:**
- `WIFI_SETUP.md` - Comprehensive WiFi setup guide
- `CHANGES.md` - This file
- Updated `claude.md` - Project architecture documentation

## How It Works

### Boot Sequence
```
1. Device boots
2. HAL initializes, loads config.json
3. Server starts (AP/Poster/Shooter mode)
4. WiFi Manager attempts connection:
   ├─ Try wifiSsid/wifiPass (user-configured)
   ├─ Try wifiSsidDefault/wifiPassDefault (default)
   └─ Fallback to AP mode (UnitCamS3-WiFi)
5. If station mode: Initialize mDNS
6. Web server starts serving on port 80
```

### Configuration Priority
```
User WiFi (via web UI)
    ↓ (if fails)
Default WiFi (pre-configured)
    ↓ (if fails)
AP Mode (always works)
```

## Migration Guide

### For Existing Projects
1. Update `firmware/data/config.json` with new fields:
   ```json
   {
     "wifiSsidDefault": "YourDefaultWiFi",
     "wifiPassDefault": "YourDefaultPassword",
     "mdnsHostname": "a1camera"
   }
   ```
2. Reflash filesystem (config.json) to device
3. No code changes needed - existing web UI continues to work

### For New Deployments
1. Edit `firmware/data/config.json` before first flash
2. Set `wifiSsidDefault` to target network
3. Set unique `mdnsHostname` per device
4. Flash firmware + filesystem
5. Devices auto-connect on boot

## Benefits

### Before
- WiFi credentials hardcoded or required reflashing
- No fallback if WiFi unavailable
- Manual IP address tracking required
- Configuration changes needed firmware rebuild

### After
- ✅ Zero-touch deployment with default WiFi
- ✅ Web-based WiFi configuration (no reflashing)
- ✅ Automatic fallback to AP mode
- ✅ mDNS for easy device discovery
- ✅ Two-tier WiFi credentials (user + default)
- ✅ Always accessible for reconfiguration

## Breaking Changes
**None** - Fully backward compatible. Existing config.json files work without new fields (will use empty defaults).

## Testing Checklist
- [ ] Device connects to user-configured WiFi
- [ ] Device falls back to default WiFi if user WiFi unavailable
- [ ] Device creates AP mode if both WiFi fail
- [ ] mDNS resolves correctly (`a1camera.local`)
- [ ] Web UI can scan and configure WiFi
- [ ] WiFi credentials persist across reboots
- [ ] API endpoints accept and save WiFi configuration
- [ ] Default credentials are preserved when user updates config

## Files Changed Summary

### Created
- `firmware/src/hal/wifi/hal_wifi.h`
- `firmware/src/hal/wifi/hal_wifi.cpp`
- `WIFI_SETUP.md`
- `CHANGES.md`

### Modified
- `firmware/data/config.json`
- `firmware/src/hal/hal.h`
- `firmware/src/hal/config/hal_config.cpp`
- `firmware/src/servers/server_ap.cpp`
- `firmware/src/apis/system/api_system.cpp`
- `claude.md`

### Unchanged
- Web interface (fully compatible)
- Camera/microphone/SD APIs
- Poster/shooter logic
- Main application flow
