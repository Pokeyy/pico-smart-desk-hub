# Pico Smart Desk Display Hub

## Directory
- [Overview](#overview)
- [Features](#features)
- [System Architecture](#system-architecture)
- [Hardware & Dependencies](#hardware--dependicies)
- [Setting it up](#setting-it-up)
- [Credits & References](#credits--references)

insert some real pics here

## Overview
An embedded display to be used on a desk built on the Raspberry Pi Pico 2W running FreeRTOS. Designed to be a single, glanceable display interface with multiple screens to showcase a wide variety of important information such as a real-time clock, weather for the week, room temperature, and phone notifications using BLE.

## Features

## System Architecture
This project is built on FreeRTOS to handle the real-time requirements and network concurrency. There are many independent tasks that are event-driven that communicate through different thread-safe mechanisms:
- ```wifi_task``` - handles network connectivity using lwIP, SNTP time syncing and notifying tasks on connection status via event groups
- ```ble_task``` - monitors any incoming smartphone notifications via ```BTstack``` and app tbd
- ```display_task``` - manages initial screen drawings and handles all redrawing via SPI to the ST7735 screen utilizing a 5x7 ASCII font, modified from adafruits original driver 
- ```weather_task``` - fetches all weather data, handles parsing using cJSON and sends data to be drawn using a queue 


## Hardware & Dependicies
### Hardware
- Raspberry Pi Pico 2W
- ST7735 1.77" 160x128 Display (Green Tab)
- Integrated AON Timer (for real time clock)

### Dependencies
- **Build System**: CMake
- **RTOS**: FreeRTOS
- **Networking**: lwIP via Pico SDK
- **Weather Parsing**: cJSON
- **Bluetooth Low Energy**: BTstack

## Setting it up: 
This utilizes Pi Pico SDK Extension on VSCode 

## Credits & References
The repositories below helped me a lot when trying to understand better code practices, what to look for when creating my own driver, and are much better than what I have tried to produce. I have referenced them multiple times, and don't claim any of their work as my own. I have utilized the code in these repositories to save time if I had felt the code was not needed to be replicated on my own. (i.e. creating my own font)
- https://github.com/bablokb/pico-st7735/tree/main 
- https://github.com/adafruit/Adafruit-ST7735-Library
- https://github.com/davegamble/cjson






