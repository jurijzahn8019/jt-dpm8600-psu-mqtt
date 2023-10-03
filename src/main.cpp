// #include <Arduino.h>
#include <FS.h>  //this needs to be first, or it all crashes and burns...
//
#include <ESP8266WiFi.h>
#include <ESPAsyncWebServer.h>    //Local WebServer used to serve the configuration portal
#include <ESPAsyncWiFiManager.h>  //https://github.com/alanswx/ESPAsyncWiFiManager WiFi Configuration
#include <SPI.h>
#include <SoftwareSerial.h>

#include "ArduinoJson.h"
#include "AsyncJson.h"
//
#include "configuration.h"
#include "constants.h"
#include "ota_update.h"

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
MultiResetDetector* mrd;

DNSServer dns;
AsyncWebServer server(80);
AsyncWiFiManager wifiManager(&server, &dns);

long last_execution = 0;
// flag for saving data
bool shouldSaveConfig = false;

Configuration config("DPM8600-" + String(ESP.getChipId()));

#include "webApi.h"
#include "webApp.h"
WebApi webApi(&server, &config);

#include "psu.h"
Psu* dpm;
SoftwareSerial uart(D2, D1);

#include "mqtt.h"
MqttClient mqtt(&config);

#include <Ticker.h>
Ticker ticker;

bool isConnected = false;
bool isReading = false;

long last_blink = 0;
void led_reading() {
  if (!isConnected && (millis() - last_blink) >= 500L) {
    // Blinking
    int state = digitalRead(LED_BUILTIN);
    digitalWrite(LED_BUILTIN, !state);
    last_blink = millis();
    return;
  }

  if (isReading && (millis() - last_blink) >= 500L) {
    digitalWrite(LED_BUILTIN, LOW);
    isReading = false;
    last_blink = millis();
    return;
  }

  if (!isReading) {
    digitalWrite(LED_BUILTIN, HIGH);
  }
}

// callback notifying us of the need to save config
void saveConfigCallback() {
  Serial.println("Should save config");
  shouldSaveConfig = true;
}

void setup() {
  Serial.begin(74880);
  while (!Serial) continue;
  Serial.println("Startup...");
  mrd = new MultiResetDetector(MRD_TIMEOUT, MRD_ADDRESS);

  if (!config.begin()) {
    Serial.println("Failed to initialize config manager");
  }

  Serial.println("instantiate led controller");
  pinMode(LED_BUILTIN, OUTPUT);
  ticker.attach(0.1, led_reading);

  Serial.println("Starting " + config.data.deviceName + ", HW ID: " + WiFi.macAddress());
  WiFi.hostname(config.data.deviceName.c_str());

  if (mrd->detectMultiReset()) {
    Serial.println("Reset Wifi Config");
    wifiManager.setDebugOutput(false);
    wifiManager.resetSettings();
    wifiManager.setDebugOutput(true);
  } else {
    Serial.println("Start Wifi");
  }

  wifiManager.setSaveConfigCallback(saveConfigCallback);

  AsyncWiFiManagerParameter device_name("device_name", "Device Name", config.data.deviceName.c_str(), 40);
  wifiManager.addParameter(&device_name);

  // fetches ssid and pass and tries to connect
  // if it does not connect it starts an access point with the specified name
  // here  "AutoConnectAP"
  // and goes into a blocking loop awaiting configuration
  if (!wifiManager.autoConnect(config.data.deviceName.c_str())) {
    Serial.println("failed to connect and hit timeout");
    delay(3000);
    // reset and try again, or maybe put it to deep sleep
    ESP.reset();
    delay(5000);
  }

  if (shouldSaveConfig) {
    config.data.deviceName = device_name.getValue();
    if (!config.save()) {
      Serial.println("Could not save configuration");
    }
  }

  Serial.println("Device Name: " + config.data.deviceName);
  Serial.println("IP Address: " + String(WiFi.localIP().toString()));
  Serial.println("MAC Address: " + String(WiFi.macAddress()));

  Serial.println("Setup Ota Module");
  ota_begin(&server);

  Serial.println("Setup WebApi");
  webApi.begin();

  Serial.println("Setup WebApp");
  webAppBegin(&server);

  Serial.println("Start Web Server");
  server.begin();

  dpm = new Psu(&config);
  Serial.println("Connected to DCDC: " + String(dpm->begin(&uart)));
  Serial.println("configure Mqtt: " + String(mqtt.begin(dpm)));
}

void loop() {
  // put your main code here, to run repeatedly:
  mrd->loop();
  webApi.loop();
  mqtt.loop();
  ota_loop();

  if ((millis() - last_execution) >= SYSTEM_STAT_INTERVAL) {
    // Serial.println("Executing Stuff: " + String(last_execution));
    last_execution = millis();
    isReading = true;
    DpmDeviceData dpmData = dpm->read();
    isConnected = dpmData.connected;

    webApi.dash(dpmData);
    mqtt.publish(dpmData);

    Serial.println(
        "Is connected: " + String(dpmData.connected) + ", Power: " + String(dpmData.power) +
        ", V: " + dpmData.voltage + " (" + dpmData.max_voltage +
        "), A: " + dpmData.current + " (" + dpmData.max_current +
        "), CC/CV: " + dpmData.cccv_status + ", Temp: " + dpmData.temperature);
  }
}