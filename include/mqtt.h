#ifndef mqtt_h
#define mqtt_h

#include <ESP8266WiFi.h>
#include <PubSubClient.h>

#include <functional>

#include "configuration.h"
#include "constants.h"
#include "psu.h"

class MqttClient {
 public:
  MqttClient(Configuration* config);

  bool begin(Psu* psu);
  bool loop();
  bool publish(DpmDeviceData data);

 private:
  const char* _willMessage = "offline";
  const char* _onlineMessage = "online";
  String _willTopic;
  String _stateTopic;
  String _clientId;
  Configuration* _config;
  PubSubClient _client;
  WiFiClient _wifiClient;
  std::function<bool(String command)> onCommand;
  Psu* _psu;

  StaticJsonDocument<1024> buildDicoMessage(String name, String node, String icon);
  bool discoPublishSwitch(String name, String node, String icon);
  bool discoPublishSensor(String name, String node, String icon, String deviceClass, boolean enabledDefault, String unit);
  bool discoPublishNumber(String name, String node, String icon, int32_t min, int32_t max);
  bool sendDiscoMessage(JsonObject disco, String type, String node);
  void publishDiscovery();

  bool processCommand(char* _topic, byte* payload, unsigned int length);
};

#endif