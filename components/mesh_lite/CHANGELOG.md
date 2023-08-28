- [中文版本](https://github.com/espressif/esp-mesh-lite/blob/master/components/mesh_lite/CHANGELOG_CN.md)

# ChangeLog

# v0.5.0 - 2023-8-3

### Enhancements:

#### Mesh

- feat: Add join mesh without configured wifi.
- feat: Optimization for the "no_router" scenario.
- feat: Adapted to the new Nova Home APP and improved group control functionality.

#### Bugfix

- fix: circular recovery issue caused by hard reboot

#### Docs

- feat: update image url

# v0.4.0 - 2023-7-12

### Enhancements:

#### Mesh

- feat: Added event posting for OTA failure and success.

#### Bugfix

- fix: When multiple root nodes are far apart and unable to discover each other, they will continuously disconnect and reconnect to the router.
- fix: During the LAN OTA upgrade process, it suddenly paused.
- fix: LAN OTA upgrade failed.

## v0.3.0 - 2023-6-30

### Enhancements:

#### Mesh

- refactor: add wifi_scan_config param for esp_mesh_lite_wifi_scan_start
- chore: Open APIs to allow external access to router information and root SoftAP IP addr
- feat: Support manually specifying a device as a leaf node.

#### Bugfix

- Disable PMF in the Demo example.
- Remove `CONFIG_LWIP_IPV6=n` to resolve the compilation issue of rainmaker_led_light.
- Optimize the mesh_local_control example to resolve the problem of socket connect blocking the event task.

## v0.2.0 - 2023-6-5

### Enhancements:

#### Mesh

- Check if the level exceeds the maximum level when level changes

#### Supported Socs

- ESP32-C2

#### Bugfix

- API compatible with idf5.x and idf4.x versions
- Update documentation to fix links not jumping correctly
- Compile error when MESH_LITE_ENABLE is n

## v0.1.2 - 2023-3-26

#### Supported ESP-IDF Version

- Add v4.3

## v0.1.1 - 2023-3-1

### Enhancements:

- Adapt to iot_bridge(0.*) component.

## v0.1.0 - 2023-2-7

This is the first released version for mesh-lite component, more detailed descriptions about the project, please refer to [ESP-MESH-LITE](https://github.com/espressif/esp-mesh-lite/blob/master/components/mesh_lite/User_Guide.md).

### Enhancements:

#### Mesh

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
- Support OTA upgrade within the mesh network

#### Supported Socs

- ESP32
- ESP32-C3
- ESP32-S2
- ESP32-S3

#### Supported ESP-IDF Version

- v4.4
- v5.0
