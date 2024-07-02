- [English Version](https://github.com/espressif/esp-mesh-lite/blob/master/examples/mesh_local_control/README.md)

| Supported Targets | ESP32 | ESP32-C3 | ESP32-S2 | ESP32-S3 | ESP32-C2 | ESP32-C6 |
| ----------------- | ----- | -------- | -------- | -------- | -------- | -------- |

# Mesh-Lite 局域网控制示例

## 介绍

本示例将介绍如何基于 Mesh-Lite 实现设备连接远程外部服务器。与 ESP-WIFI-MESH 不同，Mesh-Lite 组网内每个设备均可独立访问外部网络。

本示例实现了 mesh 网络中设备数据传输到 TCP 服务器功能。

## 硬件准备

1. 至少两块 ESP32 开发板
2. 一台支持 2.4G 路由器

## 工作流程

ESP-Mesh-Lite 是基于 ESP-IDF 的功能和工具开发的。因此，首先必须设置 ESP-IDF 开发环境。您可以参考[设置开发环境](https://docs.espressif.com/projects/esp-idf/zh_CN/latest/esp32/get-started/index.html)获取详细的步骤。之后，您可以直接在 ESP-Mesh-Lite 中构建示例，类似于在 ESP-IDF 中构建示例。

### 运行 TCP 服务器

1. 将主机（PC 或手机）连接到路由器。
2. 使用 TCP 测试工具（此工具为任意一个第三方的 TCP 测试软件）来创建 TCP 服务器。

### 配置设备

输入 `idf.py menuconfig`，在 “Example Configuration” 子菜单下，进行配置：

 * 路由器信息
 * ESP-WIFI-MESH 网络：密码长度要大于 8 位并小于 64 位，设置为空则不加密
 * TCP 服务器：主机上运行的 TCP 服务器信息, 包含：IP 地址、端口

<img src="https://raw.githubusercontent.com/espressif/esp-mesh-lite/master/examples/mesh_local_control/device_config.png" alt="device_config" width="80%" div align=center />

### 编译和烧录

CMake:
```shell
idf.py erase_flash flash monitor -p /dev/ttyUSBx
```

### 运行

Mesh-Lite 设备每隔三秒会给 TCP 服务发送当前设备的信息
