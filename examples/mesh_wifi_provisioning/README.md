- [中文版本](https://github.com/espressif/esp-mesh-lite/blob/master/examples/mesh_local_control/README_CN.md)

| Supported Targets | ESP32 | ESP32-C3 | ESP32-S2 | ESP32-S3 | ESP32-C2 | ESP32-C6 |
| ----------------- | ----- | -------- | -------- | -------- | -------- | -------- |

# Mesh Local Control Example

## Introduction

This example will introduce how to implement a device connection to a remote external server based on Mesh-Lite. Different from ESP-WIFI-MESH, each device in the Mesh-Lite network can independently access the external network.

This example implements the function of device data transmission in the mesh network to the TCP server.

**It is recommended to compile with esp-idf version 4.4**

## Hardware

* At least 2 x ESP32 development boards
* 1 x router that supports 2.4G

## Process

ESP-Mesh-Lite is developed based on ESP-IDF functions and tools. Therefore, the ESP-IDF development environment must be set up first. You can refer to [Setting up Development Environment](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/get-started/index.html) for detailed steps. Afterward, you can build the example directly in ESP-Mesh-Lite, similar to building the example in ESP-IDF.

### Run TCP server

1. Connect PC or the mobile phone to the router.
2. Use a TCP testing tool (any third-party TCP testing software) to create a TCP server.

### Configure the devices

Enter `idf.py menuconfig`, and configure the followings under the submenu "Example Configuration".

 * The router information.
 * ESP-WIFI-MESH network: The network password length should be between 8 and 64 bits (both exclusive), and the network will not be encrypted if you leave the password blank.
 * TCP server: the information of the TCP server run on the PC.

<img src="https://raw.githubusercontent.com/espressif/esp-mesh-lite/master/examples/mesh_local_control/device_config.png" alt="device_config" width="80%" div align=center />

### Build and Flash

CMake:
```shell
idf.py erase_flash flash monitor -p /dev/ttyUSBx
```

### Run

ESP-WIFI-MESH devices send the real-time device status to the TCP server at an interval of three seconds.