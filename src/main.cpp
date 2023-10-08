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
#include "network.h"
#include "ota_update.h"

long last_execution = 0;

DNSServer dns;
AsyncWebServer server(80);
Configuration config("DPM8600-" + String(ESP.getChipId()));
Network network(&config, &server, &dns);

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

void setup() {
  Serial.begin(74880);
  while (!Serial) continue;
  Serial.println("Startup...");

  if (!config.begin()) {
    Serial.println("Failed to initialize config manager");
  }

  Serial.println("instantiate led controller");
  pinMode(LED_BUILTIN, OUTPUT);
  ticker.attach(0.1, led_reading);

  Serial.println("Start " + config.data.deviceName + ", HW ID: " + WiFi.macAddress());
  network.begin();

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
  network.loop();
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