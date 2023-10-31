# ChangeLog

## v0.9.0 - 2023-10-31

### 功能:

#### Mesh

- 增加 max node number 配置项
- 增加 LAN OTA 进度上报
- 当设置 allowed level 和 disallowed level 时内部会自动检查是否与当前层级冲突，如果冲突则断开 station，重新寻找新的父节点
- 修复叶子节点被其他子节点连接
- 增加融合时对信号强度相同的根节点的处理

#### Bugfix

- 修复节点软重启后连接原父节点失败
- 修 no_router 示例提示通讯错误的问题

## v0.8.0 - 2023-9-19

### 功能:

#### 支持的 ESP-IDF 版本

- 增加对 v5.1 的支持

#### 支持的芯片

- ESP32-C6

## v0.7.0 - 2023-9-4

### Enhancements:

#### Mesh

- 增加 esp_mesh_lite_start API
- LAN OTA: LAN OTA 成功后 esp_restart 的调用放到 lib 外部

#### Bugfix

- 修复网络异常时的内存泄露

## v0.6.0 - 2023-8-29

### Enhancements:

#### Mesh

- 删除 end_with_mac 字段以适应各种类型的后缀

#### Bugfix

- 修复根节点在连接路由器异常时子节点不能正常加入 Mesh 网络

## v0.5.0 - 2023-8-3

### Enhancements:

#### Mesh

- 支持节点不配网自动加入 Mesh 网络
- 优化无路由器的场景
- 适配新的 Nova Home APP，完善群控、组控功能

#### Bugfix

- 修复硬件复位造成的环形连接

#### Docs

- 更新图片 url

## v0.4.0 - 2023-7-12

### 功能:

#### Mesh

- 新增功能：添加了 LAN OTA 升级失败和成功的事件上报。

#### 修复

- 修复当多个根节点相距较远且无法相互发现时，它们将不断地与路由器断开连接并重新连接。
- 修复 LAN OTA 升级过程中，突然停止。
- 修复 LAN OTA 升级失败。

## v0.3.0 - 2023-6-30

### 功能:

#### Mesh

- 为 esp_mesh_lite_wifi_scan_start 添加 wifi_scan_config 参数
- 开放 API 以允许应用层获取路由器信息和根节点 SoftAP IP 地址
- 支持手动指定设备为叶子节点

#### 修复

- 删除 `CONFIG_LWIP_IPV6=n`，解决 rainmaker/led_light 编译不过的问题
- Demo 示例禁用 PMF
- 优化 mesh_local_control 示例，解决 socket connect 阻塞 event task 的问题

## v0.2.0 - 2023-6-5

### 功能:

#### Mesh

- 节点层级变动后进行层级检查，避免超过规定的最高层级

#### 支持的芯片

- ESP32-C2

#### 修复

- idf5.x 和 idf4.x 版本的 API 兼容性问题
- 更新文档，修复链接无法正确跳转
- 修复当 MESH_LITE_ENABLE 配置成 n 时组件编译失败

## v0.1.2 - 2023-3-26

### 功能:

#### 支持的 ESP-IDF 版本

- 增加对 v4.3 的支持

## v0.1.1 - 2023-3-1

### 功能:

- 适配 iot_bridge(0.*) 组件

## v0.1.0 - 2023-2-7

这是 mesh-lite 组件的第一个发布版本，更多关于项目的详细说明，请参考 [ESP-MESH-LITE](https://github.com/espressif/esp-mesh-lite/blob/master/components/mesh_lite/User_Guide_CN.md).

### 功能:

#### Mesh

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

#### 支持的芯片

- ESP32
- ESP32-C3
- ESP32-S2
- ESP32-S3

#### 支持的 ESP-IDF 版本

- v4.4
- v5.0
