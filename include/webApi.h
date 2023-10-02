#ifndef webApi_h
#define webApi_h

#include <ESPAsyncWebServer.h>

#include "configuration.h"
#include "constants.h"

class WebApi {
 public:
  struct State {
    bool shouldReset;
  };

  WebApi(AsyncWebServer* server, Configuration* config);
  void begin();
  void loop();
  void dash(DpmDeviceData data);
  State state;

 private:
  AsyncWebServer* _server;
  Configuration* _config;
  AsyncEventSource _events;
  StaticJsonDocument<1024> _dashboard;

  void publishDashboard();
};

#endif