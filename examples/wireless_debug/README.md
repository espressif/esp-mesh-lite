- [中文版本](https://github.com/espressif/esp-mesh-lite/blob/master/examples/wireless_debug/README_CN.md)

| Supported Targets | ESP32 | ESP32-C3 | ESP32-S2 | ESP32-S3 | ESP32-C2 | ESP32-C6 |
| ----------------- | ----- | -------- | -------- | -------- | -------- | -------- |

# Mesh Local Control Example

## Introduction

This example introduces how to debug based on the Mesh-Lite Wireless Debug solution and obtain some information of other devices through espnow.

## Hardware

* At least 2 x ESP32 development boards
* 1 x router that supports 2.4G

## Process

ESP-Mesh-Lite is developed based on ESP-IDF functions and tools. Therefore, the ESP-IDF development environment must be set up first. You can refer to [Setting up Development Environment](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/get-started/index.html) for detailed steps. Afterward, you can build the example directly in ESP-Mesh-Lite, similar to building the example in ESP-IDF.

### Configure the devices

Enter `idf.py menuconfig`, and configure the followings under the submenu "Example Configuration".

 * The router information.

### Build and Flash

CMake:
```shell
idf.py erase_flash flash monitor -p /dev/ttyUSBx
```
