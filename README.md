- [中文版本](./README_CN.md)

# ESP-Mesh-Lite


ESP-MESH-LITE is a Wi-Fi networking application of [IoT-Bridge](https://github.com/espressif/esp-iot-bridge), based on the **SoftAP + Station** mode, a set of Mesh solutions built on top of the Wi-Fi protocol. ESP-MESH-LITE allows numerous devices (henceforth referred to as nodes) spread over a large physical area (both indoors and outdoors) to be interconnected under a single WLAN (Wireless Local-Area Network). The biggest difference between ESP-MESH-LITE and [ESP-MESH](https://docs.espressif.com/projects/esp-idf/en/v5.0/esp32/api-guides/esp-wifi-mesh.html) (also known as ESP-WIFI-MESH) is that ESP-MESH-LITE allows sub-devices in the network to independently access the external network, and the transmission information is insensitive to the parent node, which greatly reduces the difficulty to develop the application layer. ESP-MESH-LITE is self-organizing and self-healing, which means the network can be built and maintained autonomously.

For more information about ESP-MESH-LITE, please refer to [ESP-MESH-LITE Guide](https://github.com/espressif/esp-mesh-lite/blob/master/components/mesh_lite/User_Guide.md).

In the [examples](https://github.com/espressif/esp-mesh-lite/blob/master/examples) directory, demos of some common application scenarios are implemented for users to quickly integrate into their own application projects.

- [examples/mesh_local_control](examples/mesh_local_control): This example only simply demonstrates device networking and TCP communication, and does not include complex network applications. Users can carry out secondary development based on this example.
- [examples/rainmaker_led_light](examples/rainmaker_led_light): This example integrates the Mesh functionality into the Rainmaker application. Users can configure the device through the `Nova Home` APP and successfully connect to the Rainmaker cloud. The device is connected to the cloud based on Rainmaker. It can also provide other devices with the ability to surf the Internet wirelessly, and form a network with the Mesh-Lite function, which greatly reduces the load on the router and expands the wireless communication range.
