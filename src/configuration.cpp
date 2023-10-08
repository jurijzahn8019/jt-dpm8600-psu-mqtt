#include "configuration.h"

Configuration::Configuration(String dev_name)
    : data{
          .deviceName = dev_name,
          .mqttServer = "",
          .mqttPort = 1883,
          .mqttUser = "",
          .mqttPass = "",
          .mqttPrefix = "dcdc",
          .dpmBusId = 1,
          .restartInterval = 12,
      } {
}

bool Configuration::begin() {
  Serial.println("Initialize Configuration");

  if (!SPIFFS.begin()) {
    Serial.println("SPIFFS initialization failed");
    return false;
  }

  Serial.println("read config file");
  File configFile = SPIFFS.open(CONFIG_FILE_NAME, "r");

  if (!configFile) {
    Serial.println("failed to open config file, init default config");
    return false;
  }

  Serial.println("parse config file");
  StaticJsonDocument<1024> json_data;
  DeserializationError error = deserializeJson(json_data, configFile);
  if (error) {
    Serial.println("failed to load json config, init default config: " + String(error.c_str()));
    return false;
  }

  Serial.println("parsed json data, update configuration");
  fromJson(json_data.as<JsonObject>());
  serializeJson(getJson(), Serial);
  Serial.println();

  configFile.close();
  return true;
}

bool Configuration::save() {
  Serial.println("saving config");

  File configFile = SPIFFS.open(CONFIG_FILE_NAME, "w");
  if (!configFile) {
    Serial.println("failed to open config file for writing");
    return false;
  }

  StaticJsonDocument<1024> json = getJson();
  serializeJson(json, Serial);
  serializeJson(json, configFile);
  configFile.close();

  return true;
}

StaticJsonDocument<1024> Configuration::getJson() {
  StaticJsonDocument<1024> json;
  json["device_name"] = data.deviceName;
  json["mqtt_server"] = data.mqttServer;
  json["mqtt_port"] = data.mqttPort;
  json["mqtt_user"] = data.mqttUser;
  json["mqtt_pass"] = data.mqttPass;
  json["mqtt_prefix"] = data.mqttPrefix;
  json["dpm_bus_id"] = data.dpmBusId;
  json["restart_interval"] = data.restartInterval;
  return json;
}

String Configuration::serialize() {
  String ret;
  serializeJson(getJson(), ret);
  return ret;
}

void Configuration::fromJson(const JsonObject json) {
  data.deviceName = json["device_name"] | data.deviceName;
  data.mqttServer = json["mqtt_server"] | data.mqttServer;
  data.mqttPort = json["mqtt_port"] | data.mqttPort;
  data.mqttUser = json["mqtt_user"] | data.mqttUser;
  data.mqttPass = json["mqtt_pass"] | data.mqttPass;
  data.mqttPrefix = json["mqtt_prefix"] | data.mqttPrefix;
  data.dpmBusId = json["dpm_bus_id"] | data.dpmBusId;
  data.restartInterval = json["restart_interval"] | data.restartInterval;
}
