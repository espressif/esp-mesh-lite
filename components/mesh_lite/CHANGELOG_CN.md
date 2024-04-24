# ChangeLog

## v0.10.3 - 2024-4-24

### 功能:

#### Bugfix

- 解决了子节点频繁上下电导致根节点内存泄漏的问题 ([79553a6](https://github.com/espressif/esp-mesh-lite/commit/79553a67a65b5644cf1c84800f1e619cea089ab7))

## v0.10.2 - 2024-4-8

### 功能:

#### Mesh

- 添加状态失败事件以监控某些异常状态 ([4b10eee](https://github.com/espressif/esp-mesh-lite/commit/4b10eee7bd76598f8093913672374ba97f9fd68b))
- 控制一些调试日志的输出 ([4b10eee](https://github.com/espressif/esp-mesh-lite/commit/4b10eee7bd76598f8093913672374ba97f9fd68b))
- 支持设置 SoftAP IP 网段 ([4b10eee](https://github.com/espressif/esp-mesh-lite/commit/4b10eee7bd76598f8093913672374ba97f9fd68b))
- 支持通过 mesh lite lan ota 接口传输自定义文件 ([bffbdf9](https://github.com/espressif/esp-mesh-lite/commit/bffbdf9297163ffd97b1996bde9c01c5247f5716))
- 设置 mesh id 时如果与当前 mesh id 不一致，则断开重新寻找新的父节点 ([c262be5](https://github.com/espressif/esp-mesh-lite/commit/c262be5f8dc2acfd75e89959e3d493c18e3069d2))
- 限制最高层级为 15 级 ([c262be5](https://github.com/espressif/esp-mesh-lite/commit/c262be5f8dc2acfd75e89959e3d493c18e3069d2))
- 临时解决方案: 有时连接 AP 时没有 Wi-Fi 事件会导致 mesh-lite 不起作用 ([576fda94](https://github.com/espressif/esp-mesh-lite/commit/576fda945366f43656edc328654e89b09b5bf9a3))
- 支持注册 mesh lite scan cb 接口 ([576fda94](https://github.com/espressif/esp-mesh-lite/commit/576fda945366f43656edc328654e89b09b5bf9a3))
- 支持设置在连接路由器时最小 rssi 阈值 ([262920b](https://github.com/espressif/esp-mesh-lite/commit/262920bc614cb4e089a01447af1ffa020e9c113a))
- 支持自定义设置融合时信号强度阈值 ([ac502e19](https://github.com/espressif/esp-mesh-lite/commit/ac502e19347fb6e33a8e2462b102725505599206))
- 支持路由器模式下，信号强度低于阈值的根节点发生融合 ([ac502e19](https://github.com/espressif/esp-mesh-lite/commit/ac502e19347fb6e33a8e2462b102725505599206))
- 支持自定义设置设备选择父节点时的最小信号强度阈值 ([ac502e19](https://github.com/espressif/esp-mesh-lite/commit/ac502e19347fb6e33a8e2462b102725505599206))

#### Bugfix

- 解决某些特殊场景下根节点设备的融合失败问题 ([4b10eee](https://github.com/espressif/esp-mesh-lite/commit/4b10eee7bd76598f8093913672374ba97f9fd68b))
- 修复了 esp_mesh_lite_get_ssid_by_mac_cb_t 不起作用的问题 ([262920b](https://github.com/espressif/esp-mesh-lite/commit/262920bc614cb4e089a01447af1ffa020e9c113a))
- 修复了设备在寻找父节点时选择一个已经到达最高层级的节点 ([ac502e19](https://github.com/espressif/esp-mesh-lite/commit/ac502e19347fb6e33a8e2462b102725505599206))

## v0.10.1 - 2024-3-5

### 功能:

#### Mesh

- 增加 LAN OTA rejected 事件
- 支持设置融合参数

#### Bugfix

- 修复当 esp_mesh_lite_try_sending_msg 的 max_retry 为 0 时内存泄漏
- 修复了无法取消设备固定或禁用的设置级别
- 兼容旧版本隐藏 ssid 组网
- 修复设置 allowed level 和 disallowed level 时生效失败

## v0.10.0 - 2024-1-9

### 功能:

#### Bugfix

- 修复用户因意外传入空的 SSID 或者停止 Wi-Fi 导致内部无法扫描且无法恢复的问题
- 修复隐藏 SoftAP ssid 时组网失败
- 修复网络拓扑恢复时层级显示错误的问题

## v0.9.2 - 2024-1-3

### 功能:

#### Mesh

- 支持控制叶子节点 SoftAP 开关状态
- 支持自定义扫描父节点时信号强度阈值
- 支持设置组网模式（Mesh 模式、路由器模式，默认为 Mesh 模式）

#### Bugfix

- 修复设备 SoftAP SSID 不一致时子节点向根节点通信失败问题

## v0.9.1 - 2023-12-5

### 功能:

#### Mesh

- 支持控制 mesh_lite 内部输出日志颜色
- 支持注册白名单以供组网时调用

#### Bugfix

- 修复根节点掉线后组网恢复慢的问题

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
