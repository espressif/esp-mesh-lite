- [English Version](https://github.com/espressif/esp-mesh-lite/blob/master/examples/wireless_debug/README.md)

| Supported Targets | ESP32 | ESP32-C3 | ESP32-S2 | ESP32-S3 | ESP32-C2 | ESP32-C6 |
| ----------------- | ----- | -------- | -------- | -------- | -------- | -------- |

# Mesh-Lite 无线调试示例

## 介绍

本示例介绍了如何基于 Mesh-Lite Wireless Debug 方案来进行调试，通过 espnow 来获取其他设备的一些信息。

## 硬件准备

1. 至少两块 ESP32 开发板
2. 一台支持 2.4G 路由器

## 工作流程

ESP-Mesh-Lite 是基于 ESP-IDF 的功能和工具开发的。因此，首先必须设置 ESP-IDF 开发环境。您可以参考[设置开发环境](https://docs.espressif.com/projects/esp-idf/zh_CN/latest/esp32/get-started/index.html)获取详细的步骤。之后，您可以直接在 ESP-Mesh-Lite 中构建示例，类似于在 ESP-IDF 中构建示例。

### 配置设备

输入 `idf.py menuconfig`，在 “Example Configuration” 子菜单下，进行配置：

 * 路由器信息

### 编译和烧录

CMake:
```shell
idf.py erase_flash flash monitor -p /dev/ttyUSBx
```
