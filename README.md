# esp32-playground

A bunch of learning projects using ESP32-S3-N16R8 developemt board.

- [User Guide](https://github.com/microrobotics/ESP32-S3-N16R8/blob/main/ESP32-S3-N16R8_User_Guide.pdf);
- [Schematic](https://99tech.com.au/mx-m/esp32/esp32-s3-yd_schematics.pdf);
- [ESP32-S3-WROOM-1 Datasheet](https://documentation.espressif.com/esp32-s3-wroom-1_wroom-1u_datasheet_en.pdf);
- [Pinout](https://lastminuteengineers.com/wp-content/uploads/iot/ESP32-S3-DevKitC-Pinout.png);

## Lesson 21 - OOP

The goal was to rewrite a classic "blink" firmware using OOP.

### General rules:

- use `volatile`, `const`/`constexpr`, `static`;
- superloop-architecture;
- GPIO periphery usage;

### Task 1: Embedded C++ "Arduino blink"

- Use `enum` for Led state (on/off);
- Use `constexpr` for GPIO mapping and timings;
- Avvoid global variables, apart from LED state;
- Avoid using `delay` - use `millis` for time management;
- Create `Led` class with `init` and `set` methods;

### Task 2. Use `static const` for params

- Create a config class with `static const` fields. Put all magic numbers here or in `constexpr`;

### Task 3. Measure superloop exec time

- Measure execution time for one loop iteration with no delays;
- Print into serial monitor once per 1000 iterations;

### Task 4. Add interrupt-driven button

- Connect a button to GPIO, set up interrupts;
- Create `volatile bool buttonPressed` to track pressed state;
- Change LED mode on button press;
- Minimal ISR, no `Serial.print` inside;
- Handle switch logic in `loop()`;


## Implementation

- `button_controller` encapsulats button processing logic. `init` sets up pin mode and subscribes to interrupt event. ISR is a static method that accepts `this` reference as an argument, calling custom `onInterrupt` method - it listens to debounced press. `consumePress` retruns `true` once and only once on button press, resetting value to `false` until the next physical btn press;
- `blink_controller` is a tiny state machine that controls LED. It has three states: "Constatnly on", "Constantly off" and "blink", that switch from one to another on `setNextState`. `refresh` is a method called in superloop, allowing to update led state (if that is blink, for example);
- These two controllers don't know about one another and interact in inside a superloop;
- There is also a `monitor` module, but it just tracks iterationcount and prints every nth iteration;

## Demo

![demo.gif](demo.gif)