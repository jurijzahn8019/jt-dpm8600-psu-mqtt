#ifndef configuration_h
#define configuration_h

#include <FS.h>  //this needs to be first, or it all crashes and burns...
//
#include <ArduinoJson.h>  //https://github.com/bblanchon/ArduinoJson

#define CONFIG_FILE_NAME "dpm_config.json"

class Configuration {
 public:
  struct Data {
    String deviceName;
    String mqttServer;
    uint16_t mqttPort;
    String mqttUser;
    String mqttPass;
    String mqttPrefix;
    int8_t dpmBusId;
    // Restart interval in hours
    int8_t restartInterval;
  };

  Configuration(String dev_name);
  Data data;
  bool begin();
  bool save();

  void fromJson(const JsonObject json);
  StaticJsonDocument<1024> getJson();
  String serialize();

 private:
};

#endif