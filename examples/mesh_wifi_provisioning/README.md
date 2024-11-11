- [中文版本](https://github.com/espressif/esp-mesh-lite/blob/master/examples/mesh_wifi_provisioning/README_CN.md)

| Supported Targets | ESP32 | ESP32-C3 | ESP32-S2 | ESP32-S3 | ESP32-C2 | ESP32-C6 |
| ----------------- | ----- | -------- | -------- | -------- | -------- | -------- |

# Mesh-Lite provisioning example

## Introduction

This example will demonstrate how to provision devices using the ESP Mesh-Lite Provisioning APP.

## Hardware

* At least 2 x ESP32 development boards
* 1 x router that supports 2.4G

## APP Preparation

The ESP Mesh-Lite Provisioning APP requires the customer to compile it themselves from the branch https://github.com/espressif/esp-mesh-lite/tree/feature/zero_provisioning_android.

## Process

ESP-Mesh-Lite is developed based on ESP-IDF functions and tools. Therefore, the ESP-IDF development environment must be set up first. You can refer to [Setting up Development Environment](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/get-started/index.html) for detailed steps. Afterward, you can build the example directly in ESP-Mesh-Lite, similar to building the example in ESP-IDF.

### Run TCP server

1. Connect PC or the mobile phone to the router.
2. Use a TCP testing tool (any third-party TCP testing software) to create a TCP server.

### Configure the devices

Enter `idf.py menuconfig`, and configure the followings under the submenu "Example Configuration".

 * ESP-WIFI-MESH network: The network password length should be between 8 and 64 bits (both exclusive), and the network will not be encrypted if you leave the password blank.
 * TCP server: the information of the TCP server run on the PC.

<img src="https://raw.githubusercontent.com/espressif/esp-mesh-lite/master/examples/mesh_wifi_provisioning/device_config.png" alt="device_config" width="80%" div align=center />

### Build and Flash and Monitor

CMake:
```shell
idf.py erase_flash flash monitor -p /dev/ttyUSBx
```

### Provisioning

- Open the ESP Mesh-Lite Provisioning APP and click on "Provision New Device"
- Click "I don't have a QR code" and select the BLE type
- The list will show devices waiting for provisioning; select one as the root node and click on the device to provision it
- Once the root node connects to the router successfully, it will automatically provision other sub-nodes through the zero-touch provisioning process
