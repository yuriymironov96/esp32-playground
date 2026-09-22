# esp32-playground

A bunch of learning projects using ESP32-S3-N16R8 developemt board.

- [User Guide](https://github.com/microrobotics/ESP32-S3-N16R8/blob/main/ESP32-S3-N16R8_User_Guide.pdf);
- [Schematic](https://99tech.com.au/mx-m/esp32/esp32-s3-yd_schematics.pdf);
- [ESP32-S3-WROOM-1 Datasheet](https://documentation.espressif.com/esp32-s3-wroom-1_wroom-1u_datasheet_en.pdf);
- [Pinout](https://lastminuteengineers.com/wp-content/uploads/iot/ESP32-S3-DevKitC-Pinout.png);

### Lessons 35-36: servos and encoders

Implemented low-level servo control using timer-based PWM. Implemented a controller element for servo using an encoder.

Encoder lets you rotate the knob in one direction indefinitely, but the servo stops when you reach boundary values (0 and 180).

### Demo

![demo.gif](demo.gif)