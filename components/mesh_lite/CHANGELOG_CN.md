# v0.1.1

适配 iot_bridge(0.2.0) 组件

# v0.1.0

这是 mesh-lite 组件的第一个发布版本，更多关于项目的详细说明，请参考 [ESP-MESH-LITE](https://github.com/espressif/esp-mesh-lite/blob/master/components/mesh_lite/README_CN.md).

## Feature

### Mesh

- 支持 mesh 网络自修复
- 支持即使在没有连接路由器的情况下根节点也可以建立 mesh 网络
- 支持节点在没有配置路由器信息的情况下自动加入 mesh 网络
- 支持只设置节点为根节点
- 支持只设置节点为非根节点
- 支持设置 mesh 网络是否允许加入
- 支持检查加入的设备是否合法
- 支持根节点与子节点通信
  - 子节点向父节点发送广播信息
  - 子节点向根节点发送消息
  - 子节点向父节点发送消息
  - 父节点向子节点发送广播信息
- 支持对 mesh 网络间的数据通信进行加密
- 支持 mesh 网络内部 OTA 升级

## 支持的芯片

- ESP32
- ESP32-C3
- ESP32-S2
- ESP32-S3

## 支持的 ESP-IDF 版本

- v4.4
- v5.0
