# Medibox Project

The Medibox project uses an ESP32 microcontroller to manage medication schedules, interfacing with various sensors and actuators, displaying data on an OLED screen, and connecting to a Node-RED dashboard via MQTT.

## Prerequisites

- Arduino IDE installed
- ESP32 board support installed in Arduino IDE
- Required libraries:
  - Wire
  - Adafruit GFX
  - Adafruit SSD1306
  - DHTesp
  - WiFi
  - ESP32Servo
  - PubSubClient

## Installation

1. Clone the repository:
    ```sh
    git clone https://github.com/yourusername/medibox-project.git
    ```
2. Open the project in Arduino IDE:
    ```sh
    File -> Open -> Select main.cpp
    ```
3. Install the required libraries in Arduino IDE:
    ```sh
    Sketch -> Include Library -> Manage Libraries...
    ```
    Search for and install the following libraries:
    - Adafruit GFX
    - Adafruit SSD1306
    - DHTesp
    - ESP32Servo
    - PubSubClient

## Usage

1. Connect the hardware components as per the pin definitions in [`main.cpp`](command:_github.copilot.openRelativePath?%5B%7B%22scheme%22%3A%22file%22%2C%22authority%22%3A%22%22%2C%22path%22%3A%22%2FE%3A%2FMedibox%2Fsrc%2Fmain.cpp%22%2C%22query%22%3A%22%22%2C%22fragment%22%3A%22%22%7D%5D "e:\Medibox\src\main.cpp").
2. Configure your WiFi credentials in the code:
    ```cpp
    const char* ssid = "your_SSID";
    const char* password = "your_PASSWORD";
    ```
3. Upload the code to the ESP32:
    ```sh
    Sketch -> Upload
    ```
4. Open the Serial Monitor to view the output:
    ```sh
    Tools -> Serial Monitor
    ```

## Contributing

1. Fork the repository.
2. Create a new branch:
    ```sh
    git checkout -b feature-branch
    ```
3. Make your changes and commit them:
    ```sh
    git commit -m "Description of changes"
    ```
4. Push to the branch:
    ```sh
    git push origin feature-branch
    ```
5. Open a pull request.

## License

This project is licensed under the MIT License.