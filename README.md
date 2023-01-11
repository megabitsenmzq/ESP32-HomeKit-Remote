# ESP32-HomeKit-Remote

This allows you to use the Home app on iOS to control your Panasonic air conditioner.

## Hardware

I'm using an ESP32 board made by M5Stack called [M5Atom Lite](https://shop.m5stack.com/products/atom-lite-esp32-development-kit). Which is cheap and small. You can use other boards, and the code is very easy to modify.

<img src="https://cdn.shopify.com/s/files/1/0056/7689/2250/products/1_2_c215b5d9-d41a-4ab8-ad54-b2531930f075_1200x1200.jpg?v=1655692122" alt="M5Atom Lite" width="350" height="350">

A sensor module from M5Stack called [ENV III](https://shop.m5stack.com/collections/m5-sensor/products/env-iii-unit-with-temperature-humidity-air-pressure-sensor-sht30-qmp6988) (SHT30+QMP6988). Which includes temperature, humidity, and air pressure sensor.

<img src="https://cdn.shopify.com/s/files/1/0056/7689/2250/products/1_c0de294c-761b-45d0-9098-fe75effe7f49_1200x1200.jpg?v=1627863922" alt="ENV III" width="350" height="350">

An infrared LED on pin 25. M5Atom has an included IR LED but is very dim. So I added an external one.

Make sure you have the "IGMP Proxy" feature enabled on your router. HomeSpan needs it to work properly. Otherwise you might experence not responding problem frequently.

## Dependencies

- [M5Atom](https://github.com/m5stack/M5Atom): To control the board and sensor modual. Optional if you have other libs that can control SHT3X.
- [HomeSpan](https://github.com/HomeSpan/HomeSpan): Used for HomeKit integration.
- [IRremoteESP8266](https://github.com/crankyoldgit/IRremoteESP8266): To control the air conditioner.

## Usage

1. Upload and watch the serial port. You will see the logs from HomeSpan. If not, press the reset button.
2. Send `W` to config your wifi credentials. 
3. Send `S <8 Digits Code>` to set the connection code.
4. Connect the accessory from your iPhone, you may need to tap on "Other options".
