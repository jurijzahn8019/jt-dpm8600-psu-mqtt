#include "psu.h"

Psu::Psu(Configuration* config) {
  _config = config;
}

bool Psu::begin(SoftwareSerial* serial) {
  _serial = serial;
  _client = new DPM8600(_config->data.dpmBusId);
  return _client->begin(_serial);
}

DpmDeviceData Psu::read() {
  DpmDeviceData res;
  isConnected = true;

  float power = _client->read('p');
  if (power == -12) {
    strcpy(res.power, "OFF");
    isConnected = false;
  } else if (power == 1) {
    strcpy(res.power, "ON");
  } else {
    strcpy(res.power, "OFF");
  }

  res.voltage = _client->read('v');
  if (res.voltage == -10) {
    res.voltage = 0;
    isConnected = false;
  }

  res.current = _client->read('c');
  if (res.current == -11) {
    res.current = 0;
    isConnected = false;
  }

  float cccv_status = _client->read('s');
  if (cccv_status == -13) {
    strcpy(res.cccv_status, "");
    isConnected = false;
  } else if (cccv_status == 1) {
    strcpy(res.cccv_status, "CC");
  } else {
    strcpy(res.cccv_status, "CV");
  }

  res.max_current = _client->read('l');
  if (res.max_current == -15) {
    res.max_current = 0;
    isConnected = false;
  }

  res.temperature = _client->read('t');
  if (res.temperature == -16) {
    res.temperature = 0;
    isConnected = false;
  }

  if (isConnected && _maxVoltage == 0) {
    _maxVoltage = res.voltage;
  }

  if (isConnected && _maxCurrent == 0) {
    _maxCurrent = res.current;
  }

  if (!isConnected) {
    res.max_voltage = 0;
  } else {
    res.max_voltage = _maxVoltage;
  }

  return res;
}

bool Psu::maxVoltage(float max) {
  _maxVoltage = max;
  return _client->write('v', max);
}

bool Psu::maxCurent(float max) {
  return _client->write('c', max);
}

bool Psu::power(bool state) {
  return _client->power(state);
}
