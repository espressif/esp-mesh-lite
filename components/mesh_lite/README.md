# ESP-Mesh-Lite Component

[![Component Registry](https://components.espressif.com/components/espressif/mesh_lite/badge.svg)](https://components.espressif.com/components/espressif/mesh_lite)

- [User Guide](https://github.com/espressif/esp-mesh-lite/tree/master/components/mesh_lite/User_Guide.md)

ESP-MESH-LITE is a Wi-Fi networking application of [IoT-Bridge](https://github.com/espressif/esp-iot-bridge), based on the **SoftAP + Station** mode, a set of Mesh solutions built on top of the Wi-Fi protocol. ESP-MESH-LITE allows numerous devices (henceforth referred to as nodes) spread over a large physical area (both indoors and outdoors) to be interconnected under a single WLAN (Wireless Local-Area Network). The biggest difference between ESP-MESH-LITE and [ESP-MESH](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-guides/esp-wifi-mesh.html) (also known as ESP-WIFI-MESH) is that ESP-MESH-LITE allows sub-devices in the network to independently access the external network, and the transmission information is insensitive to the parent node, which greatly reduces the difficulty to develop the application layer. ESP-MESH-LITE is self-organizing and self-healing, which means the network can be built and maintained autonomously.

If you are located in China or have difficulties to access GitHub, you can also use `git clone https://jihulab.com/esp-mirror/espressif/esp-mesh-lite.git` or `git clone https://gitee.com/EspressifSystems/esp-mesh-lite.git` to get ESP-Mesh-Lite, which may be faster.

## Feature

- Support self-healing of the mesh network
- Support establishing mesh network by root node even if no router is connected
- Support automatically joining the mesh network without configuring router information
- Support setting node only as root node
- Support setting node only as non-root node
- Support setting whether the mesh network is allowed to join
- Support checking whether the joined devices are legal
- Support root node to communicate with child nodes
  - Child node sends broadcast message to parent node
  - Child node sends message to root node
  - Child node sends message to parent node
  - Parent node sends broadcast message to child nodes
- Support encryption of data communication between mesh network

## API

- The application layer only needs to call this API at the code level to enable Mesh-Lite.

	```
    esp_bridge_create_all_netif();

    esp_mesh_lite_config_t mesh_lite_config = ESP_MESH_LITE_DEFAULT_INIT();
    esp_mesh_lite_init(&mesh_lite_config);

    esp_mesh_lite_start();
	```

## Add component to your project
Please use the component manager command `add-dependency` to add the `mesh_lite` to your project's dependency, during the CMake step the component will be downloaded automatically.

```
idf.py add-dependency "espressif/mesh_lite=*"
```

## Examples

Please use the component manager command `create-project-from-example` to create the project from example template.

```
idf.py create-project-from-example "espressif/mesh_lite=*:mesh_local_control"
```

Then the example will be downloaded in current folder, you can check into it for build and flash.

> Or you can download examples from `esp-mesh-lite` repository: [mesh_local_control](https://github.com/espressif/esp-mesh-lite/tree/master/examples/mesh_local_control)

## Q&A

Q1. I encountered the following problems when using the package manager

```
Executing action: create-project-from-example
CMakeLists.txt not found in project directory /home/username
```

A1. This is because an older version packege manager was used, please run `pip install -U idf-component-manager` in ESP-IDF environment to update.

Q2. When Mesh-Lite is working, I call the `esp_wifi_scan_start` interface but it does not work.

A2. Because Mesh-Lite also has scan operations inside, it is recommended to use the `esp_mesh_lite_wifi_scan_start()` interface to scan. To call the scan interface externally, there needs to be a flag in the scan_done cb to control whether it is an externally called scan. If it is not an externally called scan, do not enter the external scan_done cb for processing.

```
if (esp_mesh_lite_wifi_scan_start(NULL, 3000 / TICK_MS) == ESP_OK) {
   scan_flag = true;
}
```
