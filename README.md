# jt-dpm8600-psu-mqtt

based on work of great people:

- <https://github.com/d4rken/jt-dpm8600-psu-mqtt>
- <https://github.com/Lotiq/DPM8600>

This is a firmware for ESP8266 Wemos D1 Mini to interface with Juntec DPM8600 DC/DC Buck converter
via RS485 serial interface.

This variant provides:

- Captive portal, for WiFi configuration.
- Web interface for configuring mqtt connection, device name etc.
- Simple Web api (used by web interface)
- Home Assistant discovery

Missing Features:

- OTA Update
- Combine SPIFFS into Firmware
- Preserve Configuration on Update
