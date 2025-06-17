- [中文版本](https://github.com/espressif/esp-mesh-lite/blob/master/examples/rainmaker/led_light/README_CN.md)

| Supported Targets | ESP32 | ESP32-C3 | ESP32-S3 | ESP32-C6 |
| ----------------- | ----- | -------- | -------- | -------- |

# LED Light Example(Nova Home)

This example uses the [Rainmaker cloud platform](https://rainmaker.espressif.com) to demonstrate the ESP-Mesh-Lite function. User can configure the device through the `Nova Home` APP and successfully connect to the Rainmaker cloud. The device is connected to the cloud based on Rainmaker, It can also provide other devices with the ability to surf the Internet wirelessly, and form a network with the Mesh-Lite function, which greatly reduces the load on the router and expands the wireless communication range.

## Get Start

### 1. Apps

- [Google PlayStore](https://play.google.com/store/apps/details?id=com.espressif.novahome)
- [Apple App Store](https://apps.apple.com/us/app/nova-home/id1563728960)

### 2. Get Key

Currently, there are two ways for a device to obtain a certificate:

- Self Claiming: After the network is configured, the device directly sends an http request to pull the certificate from the server, which can only be applied on **ESP32-S3** and **ESP32-C3**. Due to the binding with the MAC, the certificate pulled by each device is the same each time.
- Assisted Claiming: When configuring the network, the mobile APP requests a certificate from the server, and then sends it to the device through Bluetooth. It is not bound to the MAC. By default, an account has a limit of 5 applications.

Currently, ESP32-S3 and ESP32-C3 use Self-Claiming to obtain certificates by default, while ESP32 can only use Assisted Claiming to obtain certificates.

> Nova Home's Assisted Claiming method is currently unstable. If you use ESP32 to obtain a certificate, it is recommended to use the [ESP Rainmaker](https://github.com/espressif/esp-rainmaker#phone-apps) APP for Claiming

### 3. IDF environment setup & SDK

Refer to [README](https://github.com/espressif/esp-iot-bridge/blob/master/components/iot_bridge/User_Guide.md#3-set-up-development-environment)

### 4. Mesh-Lite function

- You can choose whether to enable the Mesh-Lite function in the menuconfig `Component config → ESP Wi-Fi Mesh Lite`. This example enables this function by default.
- If the Mesh-Lite function is enabled, the first networked device will connect to the target router and serve as the root node, and subsequent devices will be connected to the root node device and act as child nodes to form a Mesh-Lite network. For details, please refer to [Mesh-Lite](https://github.com/espressif/esp-mesh-lite/blob/master/components/mesh_lite/User_Guide.md).

### 5. Build & Flash

After the ESP-IDF environment is successfully set up, you can execute the following commands to compile and burn the firmware.

```
$ cd esp-mesh-lite/examples/rainmaker/led_light
$ idf.py set-target esp32c3
$ idf.py build
$ idf.py flash
```

### 6. Operation Steps

#### 6.1 Add Root Node

- Open the Nova Home app，The app will automatically discover the devices ready for network configuration.

<img src="https://raw.githubusercontent.com/espressif/esp-mesh-lite/master/examples/rainmaker/led_light/_static/find_devices.jpg" alt="find_devices" width="25%" div align=center />

- Select one of the devices from the list.

<img src="https://raw.githubusercontent.com/espressif/esp-mesh-lite/master/examples/rainmaker/led_light/_static/select_root_node.png" alt="select_root_node" width="25%" div align=center />

- Enter the network configuration details to proceed with the setup.

<img src="https://raw.githubusercontent.com/espressif/esp-mesh-lite/master/examples/rainmaker/led_light/_static/select_network.jpg" alt="select_network" width="25%" div align=center />

- After successful network configuration, the device will be added to the app.

<img src="https://raw.githubusercontent.com/espressif/esp-mesh-lite/master/examples/rainmaker/led_light/_static/root_done.png" alt="img" width="25%" div align=center />

<img src="https://raw.githubusercontent.com/espressif/esp-mesh-lite/master/examples/rainmaker/led_light/_static/root_device_of_common.png" alt="root_device_of_common" width="25%" div align=center />

- Page of the Device in the Mesh (Mesh ID is 117)

<img src="https://raw.githubusercontent.com/espressif/esp-mesh-lite/master/examples/rainmaker/led_light/_static/root_device_of_mesh.png" alt="root_device_of_mesh" width="25%" div align=center />

#### 6.2 Add child node

- On the Mesh page of the root node, click the plus icon in the upper-right corner.

<img src="https://raw.githubusercontent.com/espressif/esp-mesh-lite/master/examples/rainmaker/led_light/_static/mesh_page.png" alt="mesh_page" width="25%" div align=center />

- Choose the child node device you want to add to the network.

<img src="https://raw.githubusercontent.com/espressif/esp-mesh-lite/master/examples/rainmaker/led_light/_static/select_child_devices.jpg" alt="select_child_devices" width="25%" div align=center />

- Enter the network configuration details (same as the router's), and proceed with the setup.

<img src="https://raw.githubusercontent.com/espressif/esp-mesh-lite/master/examples/rainmaker/led_light/_static/select_network.jpg" alt="select_network" width="25%" div align=center />

- Network configuration successful.

<img src="https://raw.githubusercontent.com/espressif/esp-mesh-lite/master/examples/rainmaker/led_light/_static/child_done.png" alt="child_done" width="25%" div align=center />

#### 6.3 Control all devices in the Mesh network.

- Access the Mesh page, tap on the corresponding root node to enter the control page. On the control page, you can control the lighting effects of all devices within the Mesh 

<img src="https://raw.githubusercontent.com/espressif/esp-mesh-lite/master/examples/rainmaker/led_light/_static/root_control.png" alt="root_control" width="25%" div align=center />

#### 6.4 Group control

- Access the Mesh page, tap on the "Group" option in the lower-left corner.

<img src="https://raw.githubusercontent.com/espressif/esp-mesh-lite/master/examples/rainmaker/led_light/_static/click_group.png" alt="click_group" width="25%" div align=center />

- Create a new Mesh Group.

<img src="https://raw.githubusercontent.com/espressif/esp-mesh-lite/master/examples/rainmaker/led_light/_static/select_device_for_group.png" alt="select_device_for_group" width="25%" div align=center />

- Use the created Group page to control the devices associated with that group.

<img src="https://raw.githubusercontent.com/espressif/esp-mesh-lite/master/examples/rainmaker/led_light/_static/group_control.png" alt="group_control" width="25%" div align=center />

### 7. Precautions

- Currently `Nova Home` only supports Wi-Fi Provisioning over Bluetooth Low Energy, so this example does not support ESP32-S2 chip currently.
