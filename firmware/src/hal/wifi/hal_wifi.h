/**
 * @file hal_wifi.h
 * @author Modified for WiFi auto-connect
 * @brief WiFi connection management with fallback to AP mode
 * @version 0.2
 * @date 2025-12-22
 *
 */
#pragma once
#include <WString.h>

namespace HAL
{
    class hal;

    namespace WiFiManager
    {
        enum class ConnectionMode
        {
            STATION,        // Connected to WiFi network
            ACCESS_POINT,   // Running as AP (fallback mode)
            CONNECTING      // Attempting connection
        };

        struct ConnectionResult
        {
            ConnectionMode mode;
            bool success;
            String ssid;
            String ip;
        };

        /**
         * @brief Attempt to connect to WiFi with fallback to AP mode
         * @param hal Reference to HAL instance for config access
         * @param timeout_ms Maximum time to wait for connection (default: 10000ms)
         * @return ConnectionResult with connection status and info
         */
        ConnectionResult connectWithFallback(HAL::hal* hal, uint32_t timeout_ms = 10000);

        /**
         * @brief Start AP mode with default settings
         * @param hal Reference to HAL instance for config access
         * @return ConnectionResult with AP info
         */
        ConnectionResult startAPMode(HAL::hal* hal);

        /**
         * @brief Initialize mDNS with hostname from config
         * @param hal Reference to HAL instance for config access
         * @return true if mDNS started successfully
         */
        bool initMDNS(HAL::hal* hal);
    }
}
