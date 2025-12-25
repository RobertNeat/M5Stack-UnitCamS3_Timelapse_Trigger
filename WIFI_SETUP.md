# WiFi Setup Guide

## Overview
The M5Stack UnitCamS3 now features intelligent WiFi connectivity with automatic fallback to AP mode, eliminating the need to reflash firmware for WiFi changes.

## Quick Start

### Method 1: Pre-configure Default WiFi (Recommended for Deployment)
1. Before flashing firmware, edit `firmware/data/config.json`:
   ```json
   {
     "wifiSsidDefault": "YourWiFiSSID",
     "wifiPassDefault": "YourWiFiPassword",
     "mdnsHostname": "a1camera"
   }
   ```
2. Flash firmware and filesystem to device
3. Device will automatically connect on boot
4. Access via `http://a1camera.local` (or device IP)

### Method 2: Configure via Web Interface (No Reflashing Required)
1. If device cannot connect to any WiFi, it automatically creates an Access Point: `UnitCamS3-WiFi`
2. Connect to `UnitCamS3-WiFi` from your phone/computer
3. Navigate to `http://192.168.4.1`
4. Use the web interface to:
   - Scan for available WiFi networks
   - Enter SSID and password
   - Save configuration
5. Device will restart and connect to your WiFi
6. Find device IP in your router, or use mDNS: `http://a1camera.local`

## WiFi Connection Priority

The device connects to WiFi using this logic:

1. **Check for Credentials**:
   - If `wifiSsid` (user-configured via web) is set, use it
   - Otherwise, if `wifiSsidDefault` (from config.json) is set, use it
   - If neither is set, skip to AP mode

2. **Attempt Connection**:
   - Try to connect with the selected credentials
   - Wait up to 10 seconds for connection
   - If successful, initialize mDNS and continue

3. **Fallback AP Mode**:
   - If connection fails or no credentials exist
   - Creates open network: `UnitCamS3-WiFi`
   - IP Address: `192.168.4.1`
   - Always accessible for configuration

## mDNS Discovery

When connected to WiFi (station mode), the device advertises itself via mDNS:
- Default hostname: `a1camera.local`
- Customizable in `config.json`: `"mdnsHostname": "your-custom-name"`
- Access via browser: `http://a1camera.local` instead of remembering IP addresses

## Configuration File Reference

Location: `firmware/data/config.json`

```json
{
  "wifiSsid": "",                    // User-configured SSID (via web UI)
  "wifiPass": "",                    // User-configured password (via web UI)
  "wifiSsidDefault": "YourWiFiSSID", // Default SSID (pre-configured)
  "wifiPassDefault": "YourPassword", // Default password (pre-configured)
  "mdnsHostname": "a1camera",        // mDNS hostname
  "startPoster": "no",               // Poster mode flag
  "waitApFirst": "no",               // AP wait flag
  "nickname": "UnitCamS3",           // Device nickname
  "postInterval": 5,                 // Timelapse interval (minutes)
  "timeZone": "GMT+0",               // Timezone
  "startShooter": "no"               // Shooter mode flag
}
```

## Deployment Scenarios

### Scenario 1: Home User (Single Network)
- Leave `wifiSsidDefault` empty
- Device starts in AP mode on first boot
- Configure WiFi through web interface
- Device remembers settings across reboots

### Scenario 2: Multiple Devices (Same Network)
- Set `wifiSsidDefault` to your WiFi before flashing
- Set unique `mdnsHostname` for each device: `camera1`, `camera2`, etc.
- All devices auto-connect on boot
- Access each at `http://camera1.local`, `http://camera2.local`

### Scenario 3: Field Deployment
- Pre-configure `wifiSsidDefault` for site WiFi
- If user changes WiFi via web UI, their setting takes priority
- If user WiFi unavailable, device falls back to default WiFi
- Zero-touch deployment possible

## Troubleshooting

### Device won't connect to WiFi
1. Check SSID and password are correct
2. Ensure WiFi network is 2.4GHz (ESP32 doesn't support 5GHz)
3. Device will auto-fallback to AP mode after ~10 seconds
4. Connect to `UnitCamS3-WiFi` and reconfigure

### Can't find device on network
1. Check your router's DHCP client list for device IP
2. Try mDNS: `http://a1camera.local`
3. Ensure device and computer are on same network
4. Some routers block mDNS - use IP address directly

### Lost WiFi credentials
1. Device creates AP mode on boot if connection fails
2. Connect to `UnitCamS3-WiFi` (192.168.4.1)
3. Reconfigure WiFi settings
4. Or reflash `config.json` with default credentials

## API Endpoints

Programmatic configuration via REST API:

- `GET /api/v1/get_config` - Retrieve current configuration
- `POST /api/v1/set_config` - Update configuration (include `wifiSsid`/`wifiPass`)
- `GET /api/v1/get_wifi_list` - Scan and list available WiFi networks
- `GET /api/v1/reset_config` - Reset to default configuration

Example: Update WiFi via curl
```bash
curl -X POST http://192.168.4.1/api/v1/set_config \
  -H "Content-Type: application/json" \
  -d '{
    "wifiSsid": "MyNetwork",
    "wifiPass": "MyPassword",
    "nickname": "UnitCamS3",
    "postInterval": 5,
    "timeZone": "GMT+0",
    "startPoster": "no"
  }'
```

## Security Notes

- AP mode is open (no password) for easy first-time configuration
- Consider setting a password for production deployments
- WiFi credentials stored in plain text in LittleFS
- mDNS hostnames must be unique on the network
