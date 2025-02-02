// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#ifdef ARDUINO_ARCH_SAMD

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <time.h>
#include <sys/time.h>
#include <SPI.h>

#if WIO_TERMINAL
#include <WiFi.h>
#include "WiFiClientSecure.h"
static WiFiClientSecure sslClient; // for Wio Terminal variant of SAMD
#else
#include <WiFi101.h>
static WiFiSSLClient sslClient;
#endif

#include <WiFiUdp.h>
#include "NTPClientAz.h"

#ifdef ARDUINO_SAMD_FEATHER_M0

// Optional LIPO battery monitoring
#define VBAT_ENABLED 1
#define VBAT_PIN    A7

#define WINC_CS   8
#define WINC_IRQ  7
#define WINC_RST  4
#define WINC_EN   2

#endif // ARDUINO_SAMD_FEATHER_M0


static WiFiSSLClient sslClient;


static void initWifi(const char* ssid, const char* pass) 
{
    #ifdef ARDUINO_SAMD_FEATHER_M0
    //Configure pins for Adafruit ATWINC1500 Feather
    Serial.println(F("WINC1500 on FeatherM0 detected."));
    Serial.println(F("Setting pins for WiFi101 library (WINC1500 on FeatherM0)"));
    WiFi.setPins(WINC_CS, WINC_IRQ, WINC_RST, WINC_EN);
    // for the Adafruit WINC1500 we need to enable the chip
    pinMode(WINC_EN, OUTPUT);
    digitalWrite(WINC_EN, HIGH);
    Serial.println(F("Enabled WINC1500 interface for FeatherM0"));
    #endif
    
  // Attempt to connect to Wifi network:
    int status = WL_IDLE_STATUS;
    Serial.print("Attempting to connect to WPA SSID: ");
    Serial.println(ssid);

    // Connect to WPA/WPA2 network. Change this line if using open or WEP network:
    while ( status != WL_CONNECTED) 
    {
        // Connect to WPA/WPA2 network:
        status = WiFi.begin(ssid, pass);
        Serial.print("    WiFi status: ");
        Serial.println(status);
        // wait 2 seconds to try again
        delay(2000);
      }

    Serial.println("\r\nConnected to wifi");
}

static void initTime() {
    WiFiUDP     _udp;

    time_t epochTime = (time_t)-1;

    NTPClientAz ntpClient;
    Serial.println("Fetching NTP epoch time");
    ntpClient.begin();

    while (true) {
        epochTime = ntpClient.getEpochTime("0.pool.ntp.org");

        if (epochTime == (time_t)-1) {
            Serial.println("Fetching NTP epoch time failed! Waiting 2 seconds to retry.");
            delay(2000);
        } else {
            Serial.print("Fetched NTP epoch time is: ");

#if WIO_TERMINAL
            char buff[32];
            sprintf(buff, "%.f", difftime(epochTime, (time_t) 0));
            Serial.println(buff);
#else
            Serial.println(epochTime);
#endif

            break;
        }
    }
    
    ntpClient.end();

    struct timeval tv;
    tv.tv_sec = epochTime;
    tv.tv_usec = 0;

    settimeofday(&tv, NULL);
}

void m0_sample_init(const char* ssid, const char* password)
{
    // The Feather M0 loses it's COMn connection with every reset.
    // This 10 s delay allows you to reselect the COM port and open the serial monitor window.
    delay(10000);
    Serial.begin(115200);
    initWifi(ssid, password);
    initTime();
}

#endif // ARDUINO_ARCH_SAMD
