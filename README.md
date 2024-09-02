# Medibox Project

The Medibox project is a smart device designed for medication management and environmental monitoring. The device features a menu-driven interface to set alarms, check environmental conditions, and adjust settings based on input from various sensors. It uses an ESP32 microcontroller and connects to a Node-RED dashboard via MQTT (Mosquitto broker).

## Table of Contents

- [Features](#features)
- [Hardware](#hardware)
- [Software](#software)
- [Setup](#setup)
- [Usage](#usage)
- [Libraries Used](#libraries-used)
- [Node-RED Dashboard](#node-red-dashboard)
- [License](#license)

## Features

- Set and manage alarms for medication reminders.
- Monitor and display environmental conditions such as temperature and humidity.
- Light intensity detection using LDRs.
- Servo motor control for physical actions (e.g., opening a medication compartment).
- WiFi connectivity for real-time updates and time synchronization.
- MQTT communication to send and receive data from a Node-RED dashboard.

## Hardware

- **ESP32 Microcontroller**
- **OLED Display (128x64)**
- **DHT22 Temperature & Humidity Sensor**
- **Servo Motor**
- **Light-Dependent Resistors (LDRs)**
- **Buzzer**
- **LEDs**
- **Buttons (Up, Down, OK, Cancel)**
- **WiFi Module (built-in on ESP32)**

## Software

- **Arduino IDE** for programming the ESP32.
- **Node-RED** for creating a dashboard to monitor and control the Medibox.
- **Mosquitto Broker** for MQTT communication between the ESP32 and Node-RED.

## Setup

1. **Hardware Assembly**:
    - Connect the OLED display to the ESP32 via I2C.
    - Connect the DHT22 sensor, LDRs, LEDs, buzzer, and buttons to appropriate GPIO pins on the ESP32.
    - Connect the servo motor to the designated pin on the ESP32.

2. **Software Installation**:
    - Install the required libraries in the Arduino IDE (see [Libraries Used](#libraries-used)).
    - Set up a Mosquitto broker on your local network or use a cloud-based MQTT broker.
    - Deploy the Node-RED dashboard on a server or your local machine.

3. **Code Upload**:
    - Modify the WiFi credentials in the code (`ssid` and `password` variables).
    - Upload the code to the ESP32 using Arduino IDE.

## Usage

- **Menu Navigation**:
    - Use the Up and Down buttons to scroll through menu options.
    - Press OK to select an option and Cancel to go back.

- **Setting Time**:
    - The device can automatically synchronize time using an NTP server, or you can set it manually.

- **Setting Alarms**:
    - Set up to 3 alarms to remind you when it’s time to take your medication.

- **Monitoring Environment**:
    - The OLED display shows current temperature, humidity, and light intensity.

- **MQTT Communication**:
    - The Medibox sends data to the Node-RED dashboard and can receive commands to control its features.

## Libraries Used

- [Wire](https://www.arduino.cc/en/Reference/Wire) - I2C communication.
- [Adafruit GFX](https://github.com/adafruit/Adafruit-GFX-Library) - Graphics library for the OLED display.
- [Adafruit SSD1306](https://github.com/adafruit/Adafruit_SSD1306) - Driver for the OLED display.
- [DHTesp](https://github.com/beegee-tokyo/DHTesp) - Library for DHT22 sensor.
- [WiFi](https://www.arduino.cc/en/Reference/WiFi) - WiFi library for ESP32.
- [ESP32Servo](https://github.com/jkb-git/ESP32Servo) - Servo motor control for ESP32.
- [PubSubClient](https://github.com/knolleary/pubsubclient) - MQTT client library.

## Node-RED Dashboard

The Node-RED dashboard provides a user-friendly interface to monitor and control the Medibox. The MQTT messages are sent to and from the ESP32 to update the dashboard and trigger actions on the Medibox.

- **Real-time Monitoring**: View the current temperature, humidity, and light intensity.
- **Alarm Management**: Set, update, or delete alarms directly from the dashboard.
- **Notifications**: Receive alerts when it’s time to take medication or when environmental conditions change.

## License

This project is licensed under the MIT License. See the [LICENSE](LICENSE) file for details.

