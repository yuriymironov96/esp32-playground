# esp32-playground

A bunch of learning projects using ESP32-S3-N16R8 developemt board.

- [User Guide](https://github.com/microrobotics/ESP32-S3-N16R8/blob/main/ESP32-S3-N16R8_User_Guide.pdf);
- [Schematic](https://99tech.com.au/mx-m/esp32/esp32-s3-yd_schematics.pdf);
- [ESP32-S3-WROOM-1 Datasheet](https://documentation.espressif.com/esp32-s3-wroom-1_wroom-1u_datasheet_en.pdf);
- [Pinout](https://lastminuteengineers.com/wp-content/uploads/iot/ESP32-S3-DevKitC-Pinout.png);

### Lesson 25: MCU timers

- Implemented a timer-driven relay controller;
- Empty `loop`: everything works on times, no `delay` function as well;
- Added watchdog to monitor program health;
- Added usage of `Preferences` API to renew MCU state on reload;
- Serial monitor logging;
- Led state indicator;

### Demo

![demo.gif](demo.gif)

### Circuit

![circuit.png](circuit.png)