- [English Version](https://github.com/espressif/esp-mesh-lite/blob/master/examples/mesh_wifi_provisioning/README.md)

| Supported Targets | ESP32 | ESP32-C3 | ESP32-S2 | ESP32-S3 | ESP32-C2 | ESP32-C6 |
| ----------------- | ----- | -------- | -------- | -------- | -------- | -------- |

# Mesh-Lite 配网示例

## 介绍

本示例将介绍如何基于 ESP Mesh-Lite Provisioning APP 为设备配网。

## 硬件准备

1. 至少两块 ESP32 开发板
2. 一台支持 2.4G 路由器

## APP 准备

ESP Mesh-Lite Provisioning APP 需要客户根据 https://github.com/espressif/esp-mesh-lite/tree/feature/zero_provisioning_android 分支自己编译。

## 工作流程

ESP-Mesh-Lite 是基于 ESP-IDF 的功能和工具开发的。因此，首先必须设置 ESP-IDF 开发环境。您可以参考[设置开发环境](https://docs.espressif.com/projects/esp-idf/zh_CN/latest/esp32/get-started/index.html)获取详细的步骤。之后，您可以直接在 ESP-Mesh-Lite 中构建示例，类似于在 ESP-IDF 中构建示例。

### IDF 配置设备

建议使用 v5.0.7、v5.1.5、v5.2.3、v5.3.1 及其之前的 tag 版本

### 编译&烧录&运行

CMake:
```shell
idf.py erase_flash flash monitor -p /dev/ttyUSBx
```

### 配网

- 打开 ESP Mesh-Lite Provisioning APP，点击 `Provision New Device``
- 点击 `I don't have a QR code`，选择 BLE 类型
- 列表中将会列出待配网设备，选择其中一个作为根节点，点击设备进行配网
- 根节点连接路由器成功后，会自动通过零配流程给其他子节点配网
