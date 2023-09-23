#ifndef psu_h
#define psu_h
#include <SoftwareSerial.h>

#include "DPM8600.h"
#include "configuration.h"
#include "constants.h"

class Psu {
 public:
  Psu(Configuration* config);

  bool begin(SoftwareSerial* serial);
  DpmDeviceData read();
  bool maxVoltage(float max);
  bool maxCurent(float max);
  bool power(bool state);
  bool isConnected = false;

 private:
  Configuration* _config;
  SoftwareSerial* _serial;
  DPM8600* _client;
  float _maxVoltage = 0;
  float _maxCurrent = 0;
};

#endif