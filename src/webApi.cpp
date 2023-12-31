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

    JsonObject res = config->getJson().as<JsonObject>();
    res["IP"] = WiFi.localIP();
    res["MAC"] = WiFi.macAddress();

    // serializeJson(res, Serial);
    serializeJson(res, *response);
    request->send(response);
  });

  _server->addHandler(new AsyncCallbackJsonWebHandler("/api/config", [config, _state](AsyncWebServerRequest* request, JsonVariant& json) {
    Serial.println("Update Config");
    JsonObject jsonObj = json.as<JsonObject>();

    // Serial.println("Set config from JSON");
    // serializeJson(jsonObj, Serial);
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

  _events.onConnect([this](AsyncEventSourceClient* client) {
    if (client->lastId()) {
      Serial.printf("Client reconnected! Last message ID that it gat is: %u\n", client->lastId());
    }

    // send event with name: "message" ,id current millis
    // and set reconnect delay to 1 second
    client->send("connected", NULL, millis(), 5500);

    publishDashboard();
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
  StaticJsonDocument<1024> doc;
  _dashboard["RSSI"] = WiFi.RSSI();
  _dashboard["power"] = data.power;
  _dashboard["voltage"] = data.voltage;
  _dashboard["current"] = data.current;
  _dashboard["max_current"] = data.max_current;
  _dashboard["max_voltage"] = data.max_voltage;
  _dashboard["cccv_status"] = data.cccv_status;
  _dashboard["temperature"] = data.temperature;
  _dashboard["connected"] = data.connected;

  publishDashboard();
}

void WebApi::publishDashboard() {
  String payload;
  serializeJson(_dashboard, payload);
  _events.send(payload.c_str(), "dashboard", millis());
}
