/**
 * @file server_ap.cpp
 * @author Forairaaaaa
 * @brief
 * @version 0.2
 * @date 2023-12-06
 *
 * @copyright Copyright (c) 2023
 *
 */
#include "servers.h"
#include <mooncake.h>
#include <Arduino.h>
#include "hal/hal.h"
#include "hal/wifi/hal_wifi.h"

#include <FS.h>
#include <LittleFS.h>
#include <ESPmDNS.h>
#include <WiFi.h>

#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>

#include <SPIFFSEditor.h>

#include "apis/camera/api_cam.h"
#include "apis/system/api_system.h"
#include "apis/mic/api_mic.h"
#include "apis/poster/api_poster.h"
#include "apis/shooter/api_shooter.h"


static AsyncWebServer* _server = nullptr;

void UserDemoServers::start_ap_server()
{
    delay(200);
    Serial.begin(115200);
    Serial.printf("start ap server\n");


    // Create web server
    _server = new AsyncWebServer(80);


    // Attempt WiFi connection with fallback to AP mode
    auto connection = HAL::WiFiManager::connectWithFallback(HAL::hal::GetHal());

    if (connection.success)
    {
        spdlog::info("WiFi ready - Mode: {}, SSID: {}, IP: {}",
            connection.mode == HAL::WiFiManager::ConnectionMode::STATION ? "STATION" : "AP",
            connection.ssid.c_str(),
            connection.ip.c_str());

        // Initialize mDNS if in station mode
        if (connection.mode == HAL::WiFiManager::ConnectionMode::STATION)
        {
            HAL::WiFiManager::initMDNS(HAL::hal::GetHal());
        }
    }
    else
    {
        spdlog::error("WiFi setup failed");
    }


    // Load apis 
    load_cam_apis(*_server);
    load_system_apis(*_server);
    load_mic_apis(*_server);
    load_poster_apis(*_server);
    load_shooter_apis(*_server);

    // Set file system support 
    _server->serveStatic("/", LittleFS, "/").setDefaultFile("index.html");
    _server->onNotFound([](AsyncWebServerRequest* request) {
        request->send(LittleFS, "/404.html");
    });


    // Start server
    _server->begin();

    // Light up 
    HAL::hal::GetHal()->setLed(true);
}


void UserDemoServers::stop_ap_server()
{
    spdlog::info("stop ap server");

    delete _server;
    delay(200);

    WiFi.softAPdisconnect();
    WiFi.disconnect();
}
