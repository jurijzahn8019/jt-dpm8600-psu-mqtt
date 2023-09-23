#include "mqtt.h"

MqttClient::MqttClient(Configuration *config) : _client(_wifiClient) {
  _config = config;
}

bool MqttClient::begin(Psu *psu) {
  _psu = psu;
  Configuration::Data conf = _config->data;

  _clientId = conf.deviceName.substring(0);
  _clientId.toLowerCase();
  _clientId.replace('-', '_');
  _clientId.replace(' ', '_');
  _stateTopic = conf.mqttPrefix + "/" + conf.deviceName + "/" + conf.dpmBusId;
  _willTopic = conf.mqttPrefix + "/" + conf.deviceName + "/status";

  Serial.println("Configure mqtt broker: " + conf.mqttServer + ":" + String(conf.mqttPort));
  // Serial.println("Will connect to MQTT Broker as User: " + conf.mqttUser + ":" + conf.mqttPass);
  _client.setServer(conf.mqttServer.c_str(), conf.mqttPort);
  _client.setBufferSize(1024);
  return true;
}

StaticJsonDocument<1024> MqttClient::buildDicoMessage(String name, String node, String icon) {
  Configuration::Data conf = _config->data;

  StaticJsonDocument<1024> disco;
  JsonObject deviceInfo = disco.createNestedObject("dev");
  JsonArray identifiers = deviceInfo.createNestedArray("ids");
  identifiers.add("dpm8600_" + _clientId);
  deviceInfo["mf"] = "Juntec";
  deviceInfo["mdl"] = "DPM8600";
  deviceInfo["name"] = conf.deviceName;

  JsonArray connections = deviceInfo.createNestedArray("cns");
  JsonArray mac = connections.createNestedArray();
  mac.add("mac");
  mac.add(WiFi.macAddress());

  JsonObject availability = disco.createNestedObject("avty");
  availability["topic"] = _willTopic;
  // availability["payload_available"] = _onlineMessage;
  // availability["payload_not_available"] = _willMessage;

  JsonObject origin = disco.createNestedObject("o");
  origin["name"] = "jt-dpm8600-mqtt";
  origin["sw"] = "0.0.1";
  // origin["url"] = "https://github.com/jurijzahn8019/jt-dpm8600-mqtt";

  disco["name"] = name;
  disco["obj_id"] = (_clientId + "_" + node);
  disco["uniq_id"] = ("dpm8600_" + _clientId + "_" + node);
  disco["stat_t"] = _stateTopic;
  disco["json_attr_t"] = _stateTopic;
  disco["val_tpl"] = ("{{ value_json." + node + " }}");
  disco["icon"] = ("mdi:" + icon);

  return disco;
}

bool MqttClient::discoPublishSwitch(String name, String node, String icon) {
  Configuration::Data conf = _config->data;
  StaticJsonDocument<1024> disco = buildDicoMessage(name, node, icon);
  String commandTopic = (_stateTopic + "/set/" + node);

  disco["ent_cat"] = "config";
  disco["pl_off"] = "OFF";
  disco["pl_on"] = "ON";
  disco["cmd_t"] = commandTopic;
  _client.subscribe(commandTopic.c_str());

  return sendDiscoMessage(disco.as<JsonObject>(), "switch", node);
}

bool MqttClient::discoPublishSensor(String name, String node, String icon, String deviceClass, boolean enabledDefault, String unit) {
  StaticJsonDocument<1024> disco = buildDicoMessage(name, node, icon);

  disco["dev_cla"] = deviceClass;
  disco["en"] = enabledDefault;
  disco["unit_of_meas"] = unit;
  disco["sug_dsp_prc"] = 2;
  disco["stat_cla"] = "measurement";

  return sendDiscoMessage(disco.as<JsonObject>(), "sensor", node);
}

bool MqttClient::discoPublishNumber(String name, String node, String icon, int32_t min, int32_t max) {
  StaticJsonDocument<1024> disco = buildDicoMessage(name, node, icon);
  String commandTopic = (_stateTopic + "/set/" + node);

  disco["ent_cat"] = "config";
  disco["min"] = min;
  disco["max"] = max;
  disco["cmd_t"] = commandTopic;
  _client.subscribe(commandTopic.c_str());

  return sendDiscoMessage(disco.as<JsonObject>(), "number", node);
}

bool MqttClient::sendDiscoMessage(JsonObject disco, String type, String node) {
  String payload;
  serializeJson(disco, payload);
  String discoTopic = ("homeassistant/" + type + "/dpm_" + _clientId + "/" + node + "/config");

  Serial.println("Publish to: " + discoTopic);
  if (!_client.publish(discoTopic.c_str(), payload.c_str(), true)) {
    Serial.println("Could not publish discovery mesage: " + discoTopic);
    return false;
  }

  delay(500);
  return true;
}

void MqttClient::publishDiscovery() {
  Configuration::Data conf = _config->data;
  StaticJsonDocument<1024> disco;

  discoPublishSwitch("Power", "power", "power");
  discoPublishSensor("Voltage", "voltage", "sine-wave", "voltage", true, "V");
  discoPublishSensor("Curent", "current", "current-dc", "current", true, "A");
  discoPublishSensor("Temperature", "temp", "thermometer-lines", "temp", true, "°C");
  discoPublishSensor("", "temp", "thermometer-lines", "temp", true, "°C");

  discoPublishNumber("Max Voltage", "max_voltage", "sine-wave", 0, 60);
  discoPublishNumber("Max Curent", "max_current", "current-dc", 0, 24);
}

bool MqttClient::processCommand(char *_topic, byte *payload, unsigned int length) {
  String topic = String(_topic);
  char data_str[length + 1];
  os_memcpy(data_str, payload, length);
  data_str[length] = '\0';

  Serial.println("Process Mqtt Command: " + topic + ": " + String(data_str));
  if (topic == _stateTopic + "/set/power") {
    if (String(data_str) == "ON") {
      return _psu->power(true);
    } else {
      return _psu->power(false);
    }
  } else if (topic == _stateTopic + "/set/max_voltage") {
    return _psu->maxVoltage(atof(data_str));
  } else if (topic == _stateTopic + "/set/max_current") {
    return _psu->maxCurent(atof(data_str));
  }

  return false;
}

bool MqttClient::loop() {
  Configuration::Data conf = _config->data;

  int16_t connCount = 0;
  while (connCount < 3 && !_client.connected()) {
    Serial.println("connect to MQTT Broker as: " + _clientId + ", User: " + conf.mqttUser + ":" + conf.mqttPass);

    if (!_client.connect(
            _clientId.c_str(), conf.mqttUser.c_str(),
            conf.mqttPass.c_str(), _willTopic.c_str(), 0, true, _willMessage)) {
      Serial.println("connection failed: " + String(_client.state()));
      connCount++;
      delay(1000);

      return false;
    } else {
      Serial.println("Connected to MQTT Broker, set online message");
      _client.publish(_willTopic.c_str(), _onlineMessage, true);

      Serial.println("Publish home assistant discovery");
      publishDiscovery();

      _client.setCallback([this](char *_topic, byte *payload, unsigned int length) {
        processCommand(_topic, payload, length);
      });
    }
  }

  _client.loop();
  return true;
}

bool MqttClient::publish(DpmDeviceData data) {
  // Serial.println("Publish DPM Data");

  if (!_client.connected()) {
    return false;
  }

  StaticJsonDocument<1024> doc;
  doc["rssi"] = WiFi.RSSI();
  doc["ip"] = WiFi.localIP();
  doc["timestamp"] = data.timestamp;
  doc["power"] = data.power;
  doc["voltage"] = data.voltage;
  doc["current"] = data.current;
  doc["temperature"] = data.temperature;
  doc["cccv_status"] = data.cccv_status;
  doc["max_voltage"] = data.max_voltage;
  doc["max_current"] = data.max_current;
  doc["connected"] = data.connected;

  String payload;
  serializeJson(doc, payload);
  if (!_client.publish(_stateTopic.c_str(), payload.c_str())) {
    Serial.println("Could not publish topic: " + _stateTopic);
    return false;
  }

  return true;
}
