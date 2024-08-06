# TFT-Time-ESP32
a Simple time display on TFT SPI 160*128 and Temperatur/Humidity sensor

what we need
- ESP32
- TFT SPI 160*128 1.8inch
- DHT 11 Sensors
- Breadboard
- Jumper wires

Library
- Adafruit ST7735 and ST7789
- DHT
- Adafruit GFX
- TFT eSPI

Circuit
TFT SPI 160*128 1.8inch
- VCC to 5V
- GND to GND
- CS to GPIO 5 / D5
- RST to GPIO 16 / D16
- DC/A0 to GPIO 4 / D4
- MOSI/SDA to GPIO 23 / D23
- SCK to GPIO 18 / D18
- LED to 3.3V

DHT 11
- VCC/VIN/("+") to 5V
- Out to GPIO 14 / D14
- GND/("-") to GND