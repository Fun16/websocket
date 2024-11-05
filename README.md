# ESP32 WebSocket LED Control

This project demonstrates how to control two LEDs connected to an ESP32 over Wi-Fi using a simple web-based interface with WebSocket communication. The interface allows users to toggle each LED's state and view their current status in real-time.

## Table of Contents
- [Overview](#overview)
- [Features](#features)
- [Hardware Requirements](#hardware-requirements)
- [Software Requirements](#software-requirements)
- [Wiring Diagram](#wiring-diagram)
- [Installation](#installation)
- [Usage](#usage)
- [Project Structure](#project-structure)
- [Troubleshooting](#troubleshooting)
- [License](#license)

## Overview

This project uses the ESP32 to host a web server with WebSocket communication, enabling real-time interaction with two LEDs. By using WebSocket, we achieve fast, bidirectional communication between the ESP32 server and the web client, allowing the LED status to update instantly when toggled.

When connected to the specified Wi-Fi network, the ESP32 runs a server that hosts a webpage with two toggle buttons and status indicators for each LED. Pressing a button will toggle the corresponding LED and update the LED status displayed on the page.

## Features

- Control two GPIO pins (LEDs on GPIO 2 and GPIO 4).
- Real-time LED status updates using WebSockets.
- Responsive web interface for desktop and mobile use.
- Simple HTML and JavaScript interface for ease of modification and extension.

## Hardware Requirements

- **ESP32 Development Board**
- **2 LEDs**
- **2 Resistors** (220Ω - 330Ω recommended)
- **Breadboard and Jumper Wires**

## Software Requirements

- [Arduino IDE](https://www.arduino.cc/en/software) (or similar ESP32-compatible IDE)
- ESP32 Board Package for Arduino IDE
- Required libraries:
  - `AsyncTCP`
  - `ESPAsyncWebServer`

### Installing Required Libraries

1. **AsyncTCP**: Install this library from the Arduino Library Manager or download it [here](https://github.com/me-no-dev/AsyncTCP).
2. **ESPAsyncWebServer**: Install this library from the Arduino Library Manager or download it [here](https://github.com/me-no-dev/ESPAsyncWebServer).

## Wiring Diagram

```
ESP32      LED1 (GPIO 2)      LED2 (GPIO 4)
-------------------------------------------
GPIO 2  -> Anode of LED1
GPIO 4  -> Anode of LED2
GND     -> Cathode of both LEDs (through resistors)
```

- Connect **GPIO 2** on the ESP32 to the anode of **LED 1**.
- Connect **GPIO 4** on the ESP32 to the anode of **LED 2**.
- Connect the cathodes of each LED to ground via a resistor.

### Pinout

| ESP32 Pin | LED | Description   |
|-----------|-----|---------------|
| GPIO 2    | 1   | First LED     |
| GPIO 4    | 2   | Second LED    |

## Installation

### 1. Configure the Code
1. Clone or download the project code.
2. Open the `.ino` file in Arduino IDE.
3. Replace `ssid` and `password` with your Wi-Fi credentials in the following lines:
   ```cpp
   const char* ssid = "Your_SSID";
   const char* password = "Your_Password";
   ```

### 2. Upload the Code
1. In the Arduino IDE, select the appropriate board (`ESP32 Dev Module`) and port.
2. Upload the code to the ESP32.

### 3. Connect to the ESP32 Server
1. After uploading, open the Serial Monitor at a baud rate of `115200` to view the IP address of the ESP32.
2. Enter the IP address in your web browser to access the control interface.

## Usage

1. **Toggle LED 1**: Press the **Toggle LED 1** button to turn LED 1 (connected to GPIO 2) ON or OFF.
2. **Toggle LED 2**: Press the **Toggle LED 2** button to turn LED 2 (connected to GPIO 4) ON or OFF.
3. The web page displays the current state of each LED, which updates in real-time.

## Project Structure

```
ESP32_WebSocket_LED_Control
│
├── src/
│   ├── main.ino                 # Main code file with WebSocket server logic
│   └── index_html[]             # HTML interface stored in PROGMEM
│
├── README.md                    # Project documentation
└── LICENSE                      # Project license
```

### Main Components

- **HTML Interface (`index_html[]`)**: The web page with two buttons and state indicators for each LED.
- **WebSocket Communication**: Real-time bidirectional communication that updates LED states on the interface instantly.
- **LED Control Logic**: ESP32 code to handle GPIO states and communicate state changes to the client.

## Troubleshooting

- **ESP32 not connecting to Wi-Fi**: Verify your `ssid` and `password` values and ensure your network is within range.
- **WebSocket not responding**: Check that both `AsyncTCP` and `ESPAsyncWebServer` libraries are installed correctly.
- **No LED response**: Verify your wiring, ensuring the LEDs and resistors are correctly connected to GPIO 2 and GPIO 4.

## License

This project is licensed under the MIT License. You may freely use and modify the code in accordance with this license. See the [LICENSE](LICENSE) file for details.

---
