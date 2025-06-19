- [English Version](https://github.com/espressif/esp-mesh-lite/blob/master/examples/rainmaker/led_light/README.md)

| Supported Targets | ESP32 | ESP32-C3 | ESP32-S3 | ESP32-C6 |
| ----------------- | ----- | -------- | -------- | -------- |

# LED Light 示例(Nova Home)

本示例通过使用 [Rainmaker 云平台](https://rainmaker.espressif.com) 来展示 ESP-Mesh-Lite 功能，用户可通过 `Nova Home` APP 来为设备配网并使设备成功连接至 Rainmaker 云端，设备本身基于 Rainmaker 实现自身连接云端的同时，还可以为其他无线设备提供无线上网的能力，多个设备形成 Mesh 组网，极大程度上减轻路由器承载压力，同时扩大了无线通信范围。

## Get Start

### 1. Apps

- [Google PlayStore](https://play.google.com/store/apps/details?id=com.espressif.novahome)

- [Apple App Store](https://apps.apple.com/us/app/nova-home/id1563728960)

### 2. 获取 Key

目前设备获取证书的方式有两种：

- Self Claiming：配网后设备直接发 http 请求从服务器拉证书，只能在 ESP32-S3 和 ESP32-C3 上应用，由于与 MAC 绑定，每一台设备每次拉取的证书是一样的
- Assisted Claiming：配网时，手机 APP 跟服务器请求证书，随后通过蓝牙发送给设备，不和 MAC 绑定，默认一个账户只有 5 次申请的限制

目前 ESP32-S3 和 ESP32-C3 默认采用 Self-Claiming 的方式获取证书，ESP32 仅能使用 Assisted Claiming 方式获取证书。

>Nova Home 目前 Assisted Claiming 方式不稳定，若使用 ESP32 获取证书建议使用 [ESP Rainmaker](https://github.com/espressif/esp-rainmaker#phone-apps) APP 进行 Claiming

### 3. 编译环境搭建 & SDK 准备

参考 [README](https://github.com/espressif/esp-iot-bridge/blob/master/components/iot_bridge/User_Guide.md#3-set-up-development-environment)

### 4. Mesh-Lite 功能

- 可以在 menuconfig 配置 `Component config → ESP Wi-Fi Mesh Lite` 中选择是否使能 Mesh-Lite 功能，本示例默认使能该功能。

- 若开启 Mesh-Lite 功能，第一个配网的设备会连接至目标路由器并作为 Root 根结点，之后的设备均会连接至根结点设备并作为子节点组成一个 Mesh-Lite 网络，详情请参考 [Mesh-Lite](https://github.com/espressif/esp-mesh-lite/blob/master/components/mesh_lite/User_Guide_CN.md)。

### 5. 固件编译 & 烧录

ESP-IDF 环境搭建成功后，即可执行以下命令进行固件编译和烧录。

```
$ cd esp-mesh-lite/examples/rainmaker/led_light
$ idf.py set-target esp32c3
$ idf.py build
$ idf.py flash
```

### 6. 操作步骤

#### 6.1 添加根节点

- 打开 Nova Home，APP 自动搜索到待配网设备

<img src="https://raw.githubusercontent.com/espressif/esp-mesh-lite/master/examples/rainmaker/led_light/_static/find_devices.jpg" alt="find_devices" width="25%" div align=center />

- 选择其中某个设备

<img src="https://raw.githubusercontent.com/espressif/esp-mesh-lite/master/examples/rainmaker/led_light/_static/select_root_node.png" alt="select_root_node" width="25%" div align=center />

- 输入配网信息进入配网

<img src="https://raw.githubusercontent.com/espressif/esp-mesh-lite/master/examples/rainmaker/led_light/_static/select_network.jpg" alt="select_network" width="25%" div align=center />

- 配网成功

<img src="https://raw.githubusercontent.com/espressif/esp-mesh-lite/master/examples/rainmaker/led_light/_static/root_done.png" alt="root_done" width="25%" div align=center />

- 配网成功后的设备

<img src="https://raw.githubusercontent.com/espressif/esp-mesh-lite/master/examples/rainmaker/led_light/_static/root_device_of_common.png" alt="root_device_of_common" width="25%" div align=center />

- 配网成功后的设备所属 Mesh 页面（Mesh ID 为 117）

<img src="https://raw.githubusercontent.com/espressif/esp-mesh-lite/master/examples/rainmaker/led_light/_static/root_device_of_mesh.png" alt="root_device_of_mesh" width="25%" div align=center />

#### 6.2 添加子节点

- 进入对应 Mesh 页面，点击右上角加号

<img src="https://raw.githubusercontent.com/espressif/esp-mesh-lite/master/examples/rainmaker/led_light/_static/mesh_page.png" alt="mesh_page" width="25%" div align=center />

- 选择要配网的子节电设备

<img src="https://raw.githubusercontent.com/espressif/esp-mesh-lite/master/examples/rainmaker/led_light/_static/select_child_devices.jpg" alt="select_child_devices" width="25%" div align=center />

- 输入配网信息（同为路由器信息）进入配网

<img src="https://raw.githubusercontent.com/espressif/esp-mesh-lite/master/examples/rainmaker/led_light/_static/select_network.jpg" alt="select_network" width="25%" div align=center />

- 配网成功

<img src="https://raw.githubusercontent.com/espressif/esp-mesh-lite/master/examples/rainmaker/led_light/_static/child_done.png" alt="child_done" width="25%" div align=center />

#### 6.3 群控

- 进入 Mesh 页面，点击对应根节点，进入控制页面进行控制 Mesh 组网内部所有设备的灯效

<img src="https://raw.githubusercontent.com/espressif/esp-mesh-lite/master/examples/rainmaker/led_light/_static/root_control.png" alt="root_control" width="25%" div align=center />

#### 6.4 组控

- 进入 Mesh 页面，点击左下角 Group

<img src="https://raw.githubusercontent.com/espressif/esp-mesh-lite/master/examples/rainmaker/led_light/_static/click_group.png" alt="click_group" width="25%" div align=center />

- 创建 Mesh Group

<img src="https://raw.githubusercontent.com/espressif/esp-mesh-lite/master/examples/rainmaker/led_light/_static/select_device_for_group.png" alt="select_device_for_group" width="25%" div align=center />

- 通过创建的 Group 页面进行控制

<img src="https://raw.githubusercontent.com/espressif/esp-mesh-lite/master/examples/rainmaker/led_light/_static/group_control.png" alt="group_control" width="25%" div align=center />

### 7. 注意事项

- 目前 Nova Home 仅支持 Wi-Fi Provisioning 配网（BLE），故该示例目前不支持 ESP32-S2 芯片
