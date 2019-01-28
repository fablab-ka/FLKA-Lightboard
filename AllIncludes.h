#pragma once
  // Verwendete Bibliotheken   =========================================================================//
  
  #include "ESP8266WiFi.h"        // http://esp8266.github.io/Arduino/versions/2.3.0/doc/installing.html 
  #include "ESP8266WiFiGeneric.h" // http://esp8266.github.io/Arduino/versions/2.3.0/doc/installing.html 
  #include <ESP8266mDNS.h>        // https://github.com/esp8266/Arduino/tree/master/libraries/ESP8266mDNS
  #include <Dns.h>                // http://esp8266.github.io/Arduino/versions/2.3.0/doc/installing.html 
  
  #include "ESPAsyncUDP.h"        // http://github.com/me-no-dev/ESPAsyncUDP
  #include <ESPAsyncTCP.h>        // http://github.com/me-no-dev/ESPAsyncTCP
  #include <ESPAsyncWebServer.h>  // http://github.com/me-no-dev/ESPAsyncWebServer
  #include <SPIFFSEditor.h>       // http://github.com/me-no-dev/ESPAsyncWebServer
 

  #include <ArduinoJson.h>        // https://arduinojson.org
  #include <Adafruit_NeoPixel.h>  // https://github.com/adafruit/Adafruit_NeoPixel

  
  #include <EEPROM.h>
  #include <memory>
  #include "FS.h"

  extern "C" {
    #include "user_interface.h"
  }
  
  
  // Basiskonfigurationen ==============================================================================//
  
  // Webserver und WiFi Konfigurationen
  #define CONF_VERSION 2    // 1..255 incremented, each time the configuration struct changes
  #define CONF_MAGIC "flka" // unique identifier for project. Please change for forks of this code
  #define CONF_MDNS_NAME "FLKA-LIGHTBOARD"
  #define CONF_DEFAULT_SSID "FLKA-LIGHTBOARD"
  #define CONF_DEFAULT_PWD "ABCdef123456"
  #define CONF_HOSTNAME "ESPTEST"
  #define CONF_ADMIN_USER "admin"
  #define CONF_ADMIN_PWD "admin"
  #define CONF_DEFAULT_CHANNEL 4

  
  #define STEPS_PER_PIXEL 26
  #define PIXEL_OFFSET    0
  
  #define LED_DATA   D0
  #define LED_CLK    D6
  #define LED_ENABLE D8
  #define LED_LATCH  D7

  #define MOTOR_A1 D1
  #define MOTOR_A2 D2
  #define MOTOR_B1 D3
  #define MOTOR_B2 D4

  #define SPACER 1


  // Zeitserver etc.
  #define TimeServerName "de.pool.ntp.org"
  #define TimeServerPort 123


  
  uint8_t speedval  = 100;    // Speed at Startup

  // Konfiguration für Flash
  struct T_ConfigStruct { 
    char    magic[5] = CONF_MAGIC;
    uint8_t version  = CONF_VERSION;
    char    mySSID[32] = CONF_DEFAULT_SSID;
    char    myPWD[64]  = CONF_DEFAULT_PWD;
    char    myHostname[20] = CONF_HOSTNAME;
    char    myAdminUser[20] = CONF_ADMIN_USER;
    char    myAdminPWD[20] = CONF_ADMIN_PWD;
  
    uint8_t myChannel  = CONF_DEFAULT_CHANNEL;
  
  };
