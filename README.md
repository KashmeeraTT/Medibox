# Medibox Project (ESP32)

The Medibox project is an ESP32-based system designed to assist with medication management and monitoring. It includes features such as timekeeping, alarm setting, temperature, and humidity monitoring, as well as user interaction via buttons and display.

## Features

- Timekeeping: The system keeps track of current time and date, allowing users to schedule medication reminders.
- Alarm Setting: Users can set up to three different alarms for medication reminders.
- Temperature and Humidity Monitoring: The system monitors temperature and humidity levels to ensure proper storage conditions for medications.
- User Interaction: The system provides buttons for user interaction and feedback via an OLED display.

## Components

The project utilizes the following components:

- ESP32 Development Board
- OLED Display (128x64 pixels)
- DHT22 Temperature and Humidity Sensor
- Buzzer
- LEDs
- Buttons

## Dependencies

The project relies on the following libraries:

- Wire.h
- Adafruit_GFX.h
- Adafruit_SSD1306.h
- DHTesp.h
- WiFi.h
- time.h

## Setup and Usage

1. Connect the components according to the specified pin configuration.
2. Upload the provided code to your ESP32 development board using the Arduino IDE or PlatformIO.
3. Ensure proper WiFi connectivity for time synchronization (if applicable).
4. Power on the system and interact with the buttons to set time, alarms, and other settings as needed.
5. Monitor the OLED display for system status and medication reminders.

## Contributing

Contributions to the Medibox project are welcome! If you have ideas for improvements or new features, feel free to submit a pull request.

