#include "webApi.h"

#include "ArduinoJson.h"
#include "AsyncJson.h"

WebApi::WebApi(AsyncWebServer* server, Configuration* config) : _events("/api/events") {
  _server = server;
  _config = config;
  state = {};
  state.shouldReset = false;
}

void WebApi::begin() {
  Configuration* config = _config;
  State* _state = &state;

  _server->on("/api/config", HTTP_GET, [config](AsyncWebServerRequest* request) {
    AsyncResponseStream* response = request->beginResponseStream("application/json");

    serializeJson(config->getJson(), Serial);
    serializeJson(config->getJson(), *response);
    request->send(response);
  });

  _server->addHandler(new AsyncCallbackJsonWebHandler("/api/config", [config, _state](AsyncWebServerRequest* request, JsonVariant& json) {
    Serial.println("Update Config");
    JsonObject jsonObj = json.as<JsonObject>();

    Serial.println("Set config from JSON");
    serializeJson(jsonObj, Serial);
    config->fromJson(jsonObj);
    config->save();

    Serial.println("Return updated config");
    AsyncResponseStream* response = request->beginResponseStream("application/json");
    serializeJson(config->getJson(), *response);
    request->send(response);

    Serial.println("Config changed, mark reset device");
    _state->shouldReset = true;
  }));

  _server->on("/api/restart", HTTP_POST, [_state](AsyncWebServerRequest* request) {
    StaticJsonDocument<256> doc;

    doc["command"] = "restart";
    doc["status"] = "success";

    AsyncResponseStream* response = request->beginResponseStream("application/json");
    serializeJson(doc, *response);
    request->send(response);

    Serial.println("Restart requested, mark reset device");
    _state->shouldReset = true;
  });

  _events.onConnect([config](AsyncEventSourceClient* client) {
    if (client->lastId()) {
      Serial.printf("Client reconnected! Last message ID that it gat is: %u\n", client->lastId());
    }

    // send event with message "hello!", id current millis
    // and set reconnect delay to 1 second
    StaticJsonDocument<1024> doc;
    doc["Name"] = config->data.deviceName;
    doc["RSSI"] = WiFi.RSSI();
    doc["IP"] = WiFi.localIP();
    doc["MAC"] = WiFi.macAddress();
    String payload;
    serializeJson(doc, payload);

    client->send(payload.c_str(), NULL, millis(), 1000);
  });
  _server->addHandler(&_events);
}

void WebApi::loop() {
  if (state.shouldReset) {
    Serial.println("Device reset requested, reset device");
    delay(1000);
    ESP.reset();
  }
}

void WebApi::dash(DpmDeviceData data) {
  this->loop();

  StaticJsonDocument<1024> doc;
  doc["Name"] = _config->data.deviceName;
  doc["RSSI"] = WiFi.RSSI();
  doc["IP"] = WiFi.localIP();
  doc["MAC"] = WiFi.macAddress();
  doc["power"] = data.power;
  doc["voltage"] = data.voltage;
  doc["current"] = data.current;
  doc["max_current"] = data.max_current;
  doc["max_voltage"] = data.max_voltage;
  doc["cccv_status"] = data.cccv_status;
  doc["temperature"] = data.temperature;
  doc["timestamp"] = data.timestamp;
  String payload;
  serializeJson(doc, payload);

  _events.send(payload.c_str(), "dashboard", millis());
}
