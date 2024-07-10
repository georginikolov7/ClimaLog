# ClimaLog

## Table of Contents

- [Overview](#overview)
- [Inside_Module](#insideModule)
- [Outside_Module](#outsideModule)
- [Hardware](#hardware)
- [Software](#software)
- [Installation](#instalation)

## Overview
    Projects consists of up to 6 outside modules for measuring air temperature, humidity and snow depth and transffering the data to the centralised inside module. The inside module has an OLED display for quick visualisation and is WiFi enabled, so that it can transfer the measurements to the Google Apps Script API. A .NET MAUI application is used to fetch the latest reading from Google Drive and show it conveniently on a mobile device, without the need to be anywhere near the station.

## Inside_Module  
Power supply: 5V micro USB
    The inside module is mainly used for visualizing the data on its display, but has other auxiliary functions:
        - Setting the mounting height of each connected outside module through the buttons below its display.  
        - Reading temperature and humidity inside the room where it is mounted  
        - Sending readings to Google Drive via HTTPS  

## Outside_Module  
    Power supply: Either Battery, Battery/Solar or plugged-in  
    Only functionality is reading temperature, humidity and snow depth and sending the readings to the inside module via 2.4 GHz band  

## Hardware  
    - Microcontrollers: ESP32 for inside module; Arduino Nano for outside modules  
    - Radio modules: NRF24  
    - Sensors: DHT22 (temp/hum); VL53 optical distance sensor for measuring snow depth  
    - OLED display -  
    - Circuit boards and enclosures : custom made  

##  Software  
    - Embedded programs written in C++. Inside module code written following the OOP paradigm which ensures code reusability and makes it possible to connect/disconnect several outside modules without changing any code.  
    - Google Apps Script is used for transferring, storing and validating the data in Google Drive  
    - Mobile App written in .NET MAUI is used for visualizing data and (in progress) changing settings of inside module via Bluetooth. E.g. changing mounting height, data transfer intervals, adding/removing connected outside modules...  

##  Installation  
    *Mounting outside module:  
        - The module should be mounted with the optical sensor pointing straight down anywhere above flat ground at the height of 50 - 110 cm.  
    *Setting up inside module:  
        - The module can be placed anywhere inside a room.  
        - Wifi credentials should be provided at initial setup. This is done through a WiFi provisioning terminal. The module will act as an AP with the network ClimaLog-WIFI-MANAGER to which you can connect with any mobile device. Then a terminal will popup asking for SSID and password. After entering them and clicking submit the module will reset itself remembering the credentials (power independatly).  
        *Should you need to change the wifi credentials the inside module can be set into WiFi Provisioning mode by holding the DISP button for 2 seconds.  
        -Setting mounting height of outside modules: In order to select a module, the SET button is held pressed for 2 seconds. A menu will show on the module display. By single-pressing SET another outside module is chosen, by holding SET again the choice is confirmed. Then a menu for selecting height is shown. Single-pressing SET increments the value by 5cm, holding SET for 2 seconds confirms the choice. The selected height of each module is stored in the modules EEPROM, which makes it power independant.  