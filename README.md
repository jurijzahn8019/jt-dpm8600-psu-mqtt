# jt-dpm8600-psu-mqtt

based on work of great people:

- <https://github.com/d4rken/jt-dpm8600-psu-mqtt>
- <https://github.com/Lotiq/DPM8600>

This is a firmware for ESP8266 Wemos D1 Mini to interface with Juntec DPM8600 DC/DC Buck converter
via RS485 serial interface.

## Features

This variant provides:

- Captive portal, for WiFi configuration.
- Web interface for configuring mqtt connection, device name etc.
- Simple Web api (used by web interface)
- Home Assistant discovery

## Missing Features

- More targeted devices
- OTA Update
- Control Device via Web interface
- Config option to use or not use mqtt
- Config option to use or not use home assistant discovery
- ...many, many more

## How to use

- Connect wemos d1 to a max485 to your dpm. See <https://github.com/d4rken/jt-dpm8600-psu-mqtt> for schematics.
- Install VSCode and PlatformIO plugin
- Clone repo
- Open repo in platformio
- run upload (or build, or upload and monitor)
