# Elderly Health & Safety Monitoring System

An ESP32-based IoT Final Year Project designed to monitor elderly health and safety through real-time temperature and motion monitoring.

## Features

- Body temperature monitoring using DS18B20
- Fall detection using MPU6050
- Local audible alert using buzzer
- OLED display for real-time system information
- Wi-Fi connectivity using ESP32
- Cloud data monitoring using ThingSpeak
- Continuous sensor monitoring

## Hardware

- ESP32
- DS18B20 Temperature Sensor
- MPU6050 Motion Sensor
- OLED Display
- Buzzer
- USB/Battery Power Supply

## Software

- Arduino IDE
- ESP32 Arduino Core
- OneWire
- DallasTemperature
- Adafruit MPU6050
- Adafruit SSD1306
- ThingSpeak
- Wi-Fi

## System Operation

The ESP32 continuously reads temperature data from the DS18B20 and acceleration data from the MPU6050.

The sensor data is processed using predefined thresholds. When abnormal temperature or a possible fall is detected, the ESP32 activates the buzzer.

The current temperature and system status are displayed on the OLED display. Sensor information is also transmitted through Wi-Fi to ThingSpeak for remote monitoring and data visualization.

## Project

Final Year Project:

**Elderly Health & Safety Monitoring System using ESP32**

Developed as part of the Bachelor of Computer Engineering Technology (Computer Systems) with Honours programme.