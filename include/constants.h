#ifndef constants_h
#define constants_h

/** rerun stuff every x milliseconds */
extern const unsigned long SYSTEM_STAT_INTERVAL;

struct DpmDeviceData {
  long timestamp = 0;
  char power[10];
  float voltage = 0;
  float current = 0;
  char cccv_status[10];
  float temperature = 0;
  float max_current = 0;
  float max_voltage = 0;
  bool connected = false;
};

#endif  // constants_h