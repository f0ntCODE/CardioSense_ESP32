# CardioSense - A monitor for heart patients
## Introduction

CardioSense is a device that can be used to monitor the heart rate of patients. It is a wearable device that can be worn on the wrist. It is a simple device that can be used by anyone.

## Features

- Monitor heart rate
- Display heart rate
- Connection with WIFI

## Components

- ESP32 (Microcontroller)
- Pulse Sensor
- OLED Display
- Extender Plate

## Working

The Pulse Sensor is placed on the wrist of the patient. The variation in red blood cells in the blood with each pulse is how beats are detected. The Pulse Sensor is connected to the ESP32. The ESP32 reads the data from the Pulse Sensor and displays it on the OLED Display. The ESP32 is connected to the WIFI and sends the data to the server.

## Instructions to configure the device

- Connect the Prototype to WIFI:
The device will create a WIFI network called "CardioSense-ESP". Connect to this network and open the browser. The configuration page will open. Enter the SSID and password of the WIFI network you want to connect to and click on save. The device will connect to the WIFI network.

After connecting to the this network, check if the device that have the server localhost is connected to the same network. If not, the device will not be able to send the data to the server. (Remember, the IP on code C/C++ must be the IP of the device that have the server running in the network).

After the steps, the device will be ready to use and send data info to server (API Laravel).

## Technologies Used

- C/C++
- Libs:
    Arduino.h
    Adafruit_GFX.h
    Adafruit_SSD1306.h
    WiFi.h
    WiFiManager.h
    HTTPClient.h
    ArduinoJson.h

## Future Scope

- Improve the accuracy of the device
- Add more features (Another index of health signals)
- Add a mobile application for the device
- Add a Bluetooth connection

## Team Members

- Wellington de Elias Rodrigues (Backend, DBs, Infrastructure) [Dablio-0](https://github.com/Dablio-0)
- Matheus Gonçalves Prado Ribeiro (Frontend, UI/UX) [MatheusCosta19](https://github.com/MatheusCosta19)
- Leonardo Fontanive Tonet (Prototype and Embbeded System)[f0ntCODE](https://github.com/f0ntCODE)

To see another information from other layers of the project, please visit the following repositories:

- [Backend](https://github.com/Dablio-0/api-cardiosense)
- [Frontend](https://github.com/Dablio-0/front-cardiosense)

## License

This project is licensed under the MIT License

This README.md file was created by [Dablio-0]
```
