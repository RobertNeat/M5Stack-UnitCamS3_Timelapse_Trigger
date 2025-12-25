/**
 * @file hal_wifi.cpp
 * @author Modified for WiFi auto-connect
 * @brief WiFi connection management with fallback to AP mode
 * @version 0.2
 * @date 2025-12-22
 *
 */
#include <mooncake.h>
#include <WiFi.h>
#include <ESPmDNS.h>
#include "hal_wifi.h"
#include "../hal.h"

using namespace HAL;

WiFiManager::ConnectionResult WiFiManager::connectWithFallback(HAL::hal* hal, uint32_t timeout_ms)
{
    ConnectionResult result;
    result.success = false;
    result.mode = ConnectionMode::CONNECTING;

    auto config = hal->getConfig();

    // Determine which credentials to use (prioritize wifiSsid, fallback to wifiSsidDefault)
    String ssid_to_use;
    String pass_to_use;

    if (config.wifi_ssid.length() > 0)
    {
        // User has configured WiFi via web interface
        ssid_to_use = config.wifi_ssid;
        pass_to_use = config.wifi_password;
        spdlog::info("Using user-configured WiFi: {}", ssid_to_use.c_str());
    }
    else if (config.wifi_ssid_default.length() > 0)
    {
        // Use default WiFi from config.json
        ssid_to_use = config.wifi_ssid_default;
        pass_to_use = config.wifi_password_default;
        spdlog::info("Using default WiFi from config: {}", ssid_to_use.c_str());
    }

    // Try to connect if we have credentials
    if (ssid_to_use.length() > 0)
    {
        spdlog::info("Attempting WiFi connection...");

        WiFi.mode(WIFI_STA);
        WiFi.begin(ssid_to_use.c_str(), pass_to_use.c_str());

        uint32_t start_time = millis();
        while (WiFi.status() != WL_CONNECTED && (millis() - start_time) < timeout_ms)
        {
            delay(500);
            Serial.print(".");
        }
        Serial.println();

        if (WiFi.status() == WL_CONNECTED)
        {
            result.success = true;
            result.mode = ConnectionMode::STATION;
            result.ssid = ssid_to_use;
            result.ip = WiFi.localIP().toString();
            spdlog::info("Connected successfully!");
            spdlog::info("SSID: {}", result.ssid.c_str());
            spdlog::info("IP: {}", result.ip.c_str());
            return result;
        }

        spdlog::warn("Failed to connect to: {}", ssid_to_use.c_str());
        WiFi.disconnect();
    }
    else
    {
        spdlog::info("No WiFi credentials configured");
    }

    // Fallback to AP mode
    spdlog::info("Starting AP mode for configuration");
    return startAPMode(hal);
}

WiFiManager::ConnectionResult WiFiManager::startAPMode(HAL::hal* hal)
{
    ConnectionResult result;

    String ap_ssid = "UnitCamS3-WiFi";

    // Start AP without password (open network)
    bool success = WiFi.softAP(ap_ssid, emptyString, 1, 0, 1, false);

    if (success)
    {
        result.success = true;
        result.mode = ConnectionMode::ACCESS_POINT;
        result.ssid = ap_ssid;
        result.ip = WiFi.softAPIP().toString();

        spdlog::info("AP Mode started");
        spdlog::info("SSID: {}", result.ssid.c_str());
        spdlog::info("IP: {}", result.ip.c_str());
    }
    else
    {
        result.success = false;
        result.mode = ConnectionMode::ACCESS_POINT;
        spdlog::error("Failed to start AP mode");
    }

    return result;
}

bool WiFiManager::initMDNS(HAL::hal* hal)
{
    auto config = hal->getConfig();

    if (config.mdns_hostname.length() == 0)
    {
        spdlog::warn("mDNS hostname not configured");
        return false;
    }

    if (MDNS.begin(config.mdns_hostname.c_str()))
    {
        spdlog::info("mDNS started: {}.local", config.mdns_hostname.c_str());
        MDNS.addService("http", "tcp", 80);
        return true;
    }
    else
    {
        spdlog::error("Failed to start mDNS");
        return false;
    }
}
