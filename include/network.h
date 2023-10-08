#ifndef network_h
#define network_h

#include <ESP8266WiFi.h>
#include <ESPAsyncWebServer.h>    //Local WebServer used to serve the configuration portal
#include <ESPAsyncWiFiManager.h>  //https://github.com/alanswx/ESPAsyncWiFiManager WiFi Configuration

#include "configuration.h"

// These definitions must be placed before #include <ESP_MultiResetDetector.h>
// to be used Otherwise, default values (MRD_TIMES = 3, MRD_TIMEOUT = 10 seconds
// and MRD_ADDRESS = 0) will be used Number of subsequent resets during
#define ESP8266_MRD_USE_RTC true
#define ESP_MRD_USE_SPIFFS false
#define MULTIRESETDETECTOR_DEBUG false

// MRD_TIMEOUT to activate
#define MRD_TIMES 2
// Number of seconds after reset during which a
// subsequent reset will be considered a multi reset.
#define MRD_TIMEOUT 5
// RTC/EEPROM Memory Address for the MultiResetDetector to use
#define MRD_ADDRESS 0

#include <ESP_MultiResetDetector.h>  //https://github.com/khoih-prog/ESP_MultiResetDetector

class Network {
 public:
  Network(Configuration* config, AsyncWebServer* server, DNSServer* dns);
  void begin();
  void loop();

 private:
  Configuration* _config;
  MultiResetDetector* _mrd;
  AsyncWebServer* _server;
  AsyncWiFiManager _wifiManager;
  long _startupTimestamp;
  long _restartInterval;
};

#endif