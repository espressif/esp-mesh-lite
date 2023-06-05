- [English Version](./README.md)

# ESP-Mesh-Lite

ESP-MESH-LITE 是以 [IoT-Bridge](https://github.com/espressif/esp-iot-bridge) 为模型，基于 **SoftAP + Station** 模式，建立在 Wi-Fi 协议之上的一套 Mesh 方案。ESP-MESH-LITE 允许分布在大范围区域内（室内和室外）的大量设备（下文称节点）在同一个 WLAN（无线局域网）中相互连接。ESP-MESH-LITE 与 [ESP-MESH](https://docs.espressif.com/projects/esp-idf/zh_CN/v5.0/esp32/api-guides/esp-wifi-mesh.html)（又称 ESP-WIFI-MESH）最大的不同是 ESP-MESH-LITE 允许组网内的子设备独立访问外部网络，传输信息对于父节点无感，大大降低了应用层开发难度，ESP-MESH-LITE 具有自组网和自修复的特性，也就是说 Mesh 网络可以自主地构建和维护。

更多有关 ESP-MESH-LITE 的信息请参考 [ESP-MESH-LITE Guide](https://github.com/espressif/esp-mesh-lite/blob/master/components/mesh_lite/User_Guide_CN.md)。

在 [examples](https://github.com/espressif/esp-mesh-lite/blob/master/examples) 目录下，实现了一些常见应用场景的 demo，可供用户快速集成到自己的应用项目中。

- [examples/mesh_local_control](examples/mesh_local_control)：本示例仅简单演示设备组网，以及 TCP 通信，不包含复杂的网络应用，用户可基于此示例进行二次开发。
- [examples/rainmaker_led_light](examples/rainmaker_led_light)：本示例将 Mesh 功能集成到 Rainmaker 应用中，用户可通过 `Nova Home` APP 来为设备配网并使设备成功连接至 Rainmaker 云端，设备本身基于 Rainmaker 实现自身连接云端的同时，还可以为其他无线设备提供无线上网的能力，搭配 Mesh-Lite 功能形成组网，极大程度上减轻路由器承载压力，同时扩大了无线通信范围。
