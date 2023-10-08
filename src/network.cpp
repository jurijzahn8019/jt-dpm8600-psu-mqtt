#include "network.h"

Network::Network(Configuration* config, AsyncWebServer* server, DNSServer* dns) : _wifiManager(server, dns) {
  _config = config;
  _mrd = new MultiResetDetector(MRD_TIMEOUT, MRD_ADDRESS);
  _server = server;
}

// flag for saving data
bool shouldSaveConfig = false;

// callback notifying us of the need to save config
void saveConfigCallback() {
  Serial.println("Should save config");
  shouldSaveConfig = true;
}

void Network::begin() {
  Configuration::Data config = _config->data;

  //                 millis sec  min  hours
  _restartInterval = (1000 * 60 * 60 * config.restartInterval);
  _startupTimestamp = millis();

  WiFi.hostname(config.deviceName.c_str());

  if (_mrd->detectMultiReset()) {
    Serial.println("Reset Wifi Config");
    _wifiManager.setDebugOutput(false);
    _wifiManager.resetSettings();
    _wifiManager.setDebugOutput(true);
  } else {
    Serial.println("Start Wifi");
  }

  _wifiManager.setSaveConfigCallback(saveConfigCallback);

  AsyncWiFiManagerParameter device_name("device_name", "Device Name", config.deviceName.c_str(), 40);
  _wifiManager.addParameter(&device_name);

  // fetches ssid and pass and tries to connect
  // if it does not connect it starts an access point with the specified name
  // here  "AutoConnectAP"
  // and goes into a blocking loop awaiting configuration
  if (!_wifiManager.autoConnect(config.deviceName.c_str())) {
    Serial.println("failed to connect and hit timeout");
    delay(3000);
    // reset and try again, or maybe put it to deep sleep
    ESP.reset();
    delay(5000);
  }

  if (shouldSaveConfig) {
    config.deviceName = device_name.getValue();
    if (!_config->save()) {
      Serial.println("Could not save configuration");
    }
  }

  Serial.println("Device Name: " + config.deviceName);
  Serial.println("IP Address: " + String(WiFi.localIP().toString()));
  Serial.println("MAC Address: " + String(WiFi.macAddress()));
}

void Network::loop() {
  _mrd->loop();

  // Process periodic restart, to maintain dhcp stuff
  if ((millis() - _startupTimestamp) >= _restartInterval) {
    Serial.println(
        "Restart interval reached, reset device (" +
        String(_restartInterval) + ":" + String(_startupTimestamp) +
        ")");
    delay(500);
    ESP.reset();
  }
}
