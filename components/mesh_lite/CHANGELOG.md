- [中文版本](https://github.com/espressif/esp-mesh-lite/blob/master/components/mesh_lite/CHANGELOG_CN.md)

# ChangeLog

## v1.0.1 - 2024-12-16

### Enhancements:

#### Mesh

- feat: Support setting message retransmission frequency ([888603e](https://github.com/espressif/esp-mesh-lite/commit/888603ef756e70f8fa8aa7e44b36ade11ecf4362))
- feat: Support wireless debug ([6c735a6](https://github.com/espressif/esp-mesh-lite/commit/6c735a6406e8cc9f06bee89c187e40850c120a0a))
- feat: Support LAN OTA breakpoint resume function ([6c735a6](https://github.com/espressif/esp-mesh-lite/commit/6c735a6406e8cc9f06bee89c187e40850c120a0a))
- feat: Verify the firmware MD5 value before LAN OTA ([6c735a6](https://github.com/espressif/esp-mesh-lite/commit/6c735a6406e8cc9f06bee89c187e40850c120a0a))
- optimize: Add some execution logic after set_networking_mode ([6c735a6](https://github.com/espressif/esp-mesh-lite/commit/6c735a6406e8cc9f06bee89c187e40850c120a0a))
- When disabling the ESP-NOW function, the related tasks will not be created internally to save RAM resources ([7aec4a6](https://github.com/espressif/esp-mesh-lite/commit/7aec4a6555a933459181be4ea8ac0a27409585ea))
- feat: support esp32c5 ([46d3e3e](https://github.com/espressif/esp-mesh-lite/commit/46d3e3e9a4acbeaad341bf6fef75b5605bcaf3d2))
- feat: optimize networking speed ([46d3e3e](https://github.com/espressif/esp-mesh-lite/commit/46d3e3e9a4acbeaad341bf6fef75b5605bcaf3d2))
- Add bssid when the root node synchronizes router information to the child node ([46d3e3e](https://github.com/espressif/esp-mesh-lite/commit/46d3e3e9a4acbeaad341bf6fef75b5605bcaf3d2))
- Support register ssid by mac cb for hidden ssid ([0579dfa](https://github.com/espressif/esp-mesh-lite/commit/0579dfa2b61bb4442d427122d5079397299b88d8))
- Open enabling mesh-lite log API ([defe726](https://github.com/espressif/esp-mesh-lite/commit/defe726f1f98de17f8d1795507dab15ba0bf3917))
- Add compatibility for older versions in the fusion mechanism ([870642f](https://github.com/espressif/esp-mesh-lite/commit/870642fb799ec23dc87966d22572dc847a371a3c))
- feat: Supports receiving data sent by the native ESP-NOW API ([a6f1544](https://github.com/espressif/esp-mesh-lite/commit/a6f15443857f87b721952bec79f237b98d39f66f))
- feat: Support internal communication to transmit binary data through Proto buffer ([27c511c](https://github.com/espressif/esp-mesh-lite/commit/27c511cc9a80304f68f5bf7e6e7fe3e073d21095))
- feat: Optimize networking speed ([0ba98f0](https://github.com/espressif/esp-mesh-lite/commit/0ba98f0b66a914cee527997fb51cd5106b9f5272))
- feat: Add eth_got_ip event handler for lan ota ([0ba98f0](https://github.com/espressif/esp-mesh-lite/commit/0ba98f0b66a914cee527997fb51cd5106b9f5272))
- feat: Attempt to optimize the current topology when changing levels ([0ba98f0](https://github.com/espressif/esp-mesh-lite/commit/0ba98f0b66a914cee527997fb51cd5106b9f5272))
- feat: Optimize fusion handler when the rssi between two roots is to low ([0ba98f0](https://github.com/espressif/esp-mesh-lite/commit/0ba98f0b66a914cee527997fb51cd5106b9f5272))

#### Bugfix

- Fixed the issue where a leaf node was mistakenly selected as a parent node when softap was enabled ([888603e](https://github.com/espressif/esp-mesh-lite/commit/888603ef756e70f8fa8aa7e44b36ade11ecf4362))
- Fixed failure to pin or disable device level when no suitable parent node is found during esp_mesh_lite_connect ([888603e](https://github.com/espressif/esp-mesh-lite/commit/888603ef756e70f8fa8aa7e44b36ade11ecf4362))
- Fixed station number exceeds the maximum connection limit ([888603e](https://github.com/espressif/esp-mesh-lite/commit/888603ef756e70f8fa8aa7e44b36ade11ecf4362))
- Fixed the level is changed thougth the node is fixed as root when root is disconnect from router ([888603e](https://github.com/espressif/esp-mesh-lite/commit/888603ef756e70f8fa8aa7e44b36ade11ecf4362))
- Fixed some fusion failure issues ([7aec4a6](https://github.com/espressif/esp-mesh-lite/commit/7aec4a6555a933459181be4ea8ac0a27409585ea))
- Fixed compatibility issues between LAN OTA enabled and disabled devices ([7aec4a6](https://github.com/espressif/esp-mesh-lite/commit/7aec4a6555a933459181be4ea8ac0a27409585ea))
- Fixed the impact that may be caused to the network under hidden ssid when using ESP-NOW ([7aec4a6](https://github.com/espressif/esp-mesh-lite/commit/7aec4a6555a933459181be4ea8ac0a27409585ea))
- Increased wifi failure_retry_cnt to reduce the probability of connection failure during networking ([7aec4a6](https://github.com/espressif/esp-mesh-lite/commit/7aec4a6555a933459181be4ea8ac0a27409585ea))
- Fix the occasional failure when calling `esp_mesh_lite_transmit_file_start` multiple times to transfer different files ([46d3e3e](https://github.com/espressif/esp-mesh-lite/commit/46d3e3e9a4acbeaad341bf6fef75b5605bcaf3d2))
- Fix occasional fusion failure ([0579dfa](https://github.com/espressif/esp-mesh-lite/commit/0579dfa2b61bb4442d427122d5079397299b88d8))
- Fix occasional crash during communication ([0579dfa](https://github.com/espressif/esp-mesh-lite/commit/0579dfa2b61bb4442d427122d5079397299b88d8))
- Fix crash caused by differences in the wifi_ap_record_t structure between IDF versions ([0579dfa](https://github.com/espressif/esp-mesh-lite/commit/0579dfa2b61bb4442d427122d5079397299b88d8))
- Fix level display anomaly ([870642f](https://github.com/espressif/esp-mesh-lite/commit/870642fb799ec23dc87966d22572dc847a371a3c))
- Fix optimal parent node selection error during topology optimization caused by level change ([870642f](https://github.com/espressif/esp-mesh-lite/commit/870642fb799ec23dc87966d22572dc847a371a3c))
- Fix networking failure when SSID is hidden ([870642f](https://github.com/espressif/esp-mesh-lite/commit/870642fb799ec23dc87966d22572dc847a371a3c))
- Fix the crash caused by internal communication issues introduced by [27c511c](https://github.com/espressif/esp-mesh-lite/commit/27c511cc9a80304f68f5bf7e6e7fe3e073d21095) ([2979846](https://github.com/espressif/esp-mesh-lite/commit/29798464f85f636eb640e867371fa7716d31d8ff))
- Fix the memory leak introduced by [27c511c](https://github.com/espressif/esp-mesh-lite/commit/27c511cc9a80304f68f5bf7e6e7fe3e073d21095) ([2979846](https://github.com/espressif/esp-mesh-lite/commit/29798464f85f636eb640e867371fa7716d31d8ff))

## v1.0.0 - 2024-7-12

### Enhancements:

#### Mesh

- feat: add esp_mesh_lite_disconnect API ([5075c94](https://github.com/espressif/esp-mesh-lite/commit/5075c94dd026a5fb12e569b693fd59b8781abc3d))
- feat: Optimize the best AP selecting ([5075c94](https://github.com/espressif/esp-mesh-lite/commit/5075c94dd026a5fb12e569b693fd59b8781abc3d))

#### Bugfix

- Fix the crash caused by abnormal data parsing in internal communication ([1881ef8](https://github.com/espressif/esp-mesh-lite/commit/1881ef8ad900e31902131e62e27b7b848c1a686a))
- Fix the problem of failing to connect to a router with WEP/WPA encryption ([80adbff](https://github.com/espressif/esp-mesh-lite/commit/80adbffb478d852256cc5fb36a1bca96956427af))

## v0.10.5 - 2024-6-7

### Enhancements:

#### Bugfix

- Adapt iot-bridge component to 0.11.7 ([20e4eb9](https://github.com/espressif/esp-mesh-lite/commit/20e4eb96949538e3d2512c06c3495194af5b6ef8))
- LAN OTA: Fix issue preventing upgrade re-trigger during the upgrade process ([20e4eb9](https://github.com/espressif/esp-mesh-lite/commit/20e4eb96949538e3d2512c06c3495194af5b6ef8))
- LAN OTA: Fix some issues causing upgrade failures. ([a99aeeb](https://github.com/espressif/esp-mesh-lite/commit/a99aeeb67aa48078f670053ca32a8d2487a70034))
    - Optimize some mechanisms to reduce the pressure on higher-level nodes during upgrades
    - Fix the issue of upgrade failures caused by changes in the topology during the upgrade process
    - Fix the issue where child nodes do not continue to request firmware after the parent node completes the upgrade and restarts
- fix the issue of abnormal root node message reported by child nodes ([0dfe6b5](https://github.com/espressif/esp-mesh-lite/commit/0dfe6b5a0df3339472f32b7b263fab809405b734))

#### Mesh

- Support idf/v5.2 & idf/v5.3 ([a27afb1](https://github.com/espressif/esp-mesh-lite/commit/a27afb1aaed3910ae22bb1b46a3c13264f044f8f))
- Remove support for idf/v4.3 & idf/v4.4 ([a27afb1](https://github.com/espressif/esp-mesh-lite/commit/a27afb1aaed3910ae22bb1b46a3c13264f044f8f))

## v0.10.4 - 2024-5-22

### Enhancements:

#### Mesh

- Add an configuration to restart chip automatically after OTA finish or user restart it ([ebeb361](https://github.com/espressif/esp-mesh-lite/commit/ebeb3617ae435f707a2e9aeabd41410513f6594f))

#### Bugfix

- fix network anomalies caused by unexpected triggering of fusion logic by child nodes ([50c2ed9](https://github.com/espressif/esp-mesh-lite/commit/50c2ed9ac72464c4491b1e614274e63a9d72e7ae))
- fix unexpected connection to its child nodes after soft restart, forming a ring ([50c2ed9](https://github.com/espressif/esp-mesh-lite/commit/50c2ed9ac72464c4491b1e614274e63a9d72e7ae))
- fix the issue of abnormal root node message reported by child nodes ([50c2ed9](https://github.com/espressif/esp-mesh-lite/commit/50c2ed9ac72464c4491b1e614274e63a9d72e7ae))
- fix the inability of reconnection mechanism to take effect in unexpected scenarios ([7148df7](https://github.com/espressif/esp-mesh-lite/commit/7148df7693929ea0e3eecd4628c8a10da611d33c))

## v0.10.3 - 2024-4-24

### Enhancements:

#### Bugfix

- Resolved the issue of memory leak in the root node caused by frequent power cycling of the child nodes ([79553a6](https://github.com/espressif/esp-mesh-lite/commit/79553a67a65b5644cf1c84800f1e619cea089ab7))

## v0.10.2 - 2024-4-8

### Enhancements:

#### Mesh

- Add status fail event to monitor some abnormal state ([4b10eee](https://github.com/espressif/esp-mesh-lite/commit/4b10eee7bd76598f8093913672374ba97f9fd68b))
- Control the output of certain debug logs. ([4b10eee](https://github.com/espressif/esp-mesh-lite/commit/4b10eee7bd76598f8093913672374ba97f9fd68b))
- Support set softap network segment ([4b10eee](https://github.com/espressif/esp-mesh-lite/commit/4b10eee7bd76598f8093913672374ba97f9fd68b))
- Support for transferring custom files via the Mesh Lite LAN OTA interface ([bffbdf9](https://github.com/espressif/esp-mesh-lite/commit/bffbdf9297163ffd97b1996bde9c01c5247f5716))
- Limit the maximum level to 15 ([c262be5](https://github.com/espressif/esp-mesh-lite/commit/c262be5f8dc2acfd75e89959e3d493c18e3069d2))
- If the mesh ID is set and it does not match the current mesh ID, disconnect and search for a new parent node ([c262be5](https://github.com/espressif/esp-mesh-lite/commit/c262be5f8dc2acfd75e89959e3d493c18e3069d2))
- Workaround: Sometimes there is no Wi-Fi event when connecting AP which will cause mesh-lite does not work ([576fda94](https://github.com/espressif/esp-mesh-lite/commit/576fda945366f43656edc328654e89b09b5bf9a3))
- Support the registration of mesh lite scan cb ([576fda94](https://github.com/espressif/esp-mesh-lite/commit/576fda945366f43656edc328654e89b09b5bf9a3))
- support custom setting min rssi threshold when connecting router ([262920b](https://github.com/espressif/esp-mesh-lite/commit/262920bc614cb4e089a01447af1ffa020e9c113a))
- Support custom setting of rssi threshold during fusion ([ac502e19](https://github.com/espressif/esp-mesh-lite/commit/ac502e19347fb6e33a8e2462b102725505599206))
- Support the fusion of root nodes with rssi below the threshold in router mode ([ac502e19](https://github.com/espressif/esp-mesh-lite/commit/ac502e19347fb6e33a8e2462b102725505599206))
- Support custom setting of minimum rssi threshold for device selection of parent node ([ac502e19](https://github.com/espressif/esp-mesh-lite/commit/ac502e19347fb6e33a8e2462b102725505599206))

#### Bugfix

- Resolve fusion failure issue for root node devices in certain special scenarios ([4b10eee](https://github.com/espressif/esp-mesh-lite/commit/4b10eee7bd76598f8093913672374ba97f9fd68b))
- Fixed esp_mesh_lite_get_ssid_by_mac_cb_t doesn't really work ([262920b](https://github.com/espressif/esp-mesh-lite/commit/262920bc614cb4e089a01447af1ffa020e9c113a))
- Fixed device selection of a node that has reached the highest level when searching for a parent node ([ac502e19](https://github.com/espressif/esp-mesh-lite/commit/ac502e19347fb6e33a8e2462b102725505599206))

## v0.10.1 - 2024-3-5

### Enhancements:

#### Mesh

- Add lan ota rejected event
- Support set fusion config

#### Bugfix

- Fix memory leak when max_retry of esp_mesh_lite_try_sending_msg is 0
- Fix unable cancel the allowed and disallowed level
- Increase the compatibility of the temporary fixed version of hidden ssid
- Fix failed to take effect when setting allowed level and disallowed level

## v0.10.0 - 2024-1-9

### Enhancements:

#### Bugfix

- Fix the issue where users accidentally pass an empty SSID or stop Wi-Fi, causing internal scanning to fail and not recover
- Fix the issue of network failure when the SoftAP SSID is hidden.
- Fix the issue of incorrect hierarchy display when the network topology is restored.

## v0.9.2 - 2024-1-3

### Enhancements:

#### Mesh

- Support set leaf node softap status
- Support customizing the signal strength threshold during scanning for parent nodes
- Support set networking mode(Mesh mode or Router mode, default Mesh mode)

#### Bugfix

- Fixed send msg to root fail when softap ssid is different

## v0.9.1 - 2023-12-13

### Enhancements:

#### Mesh

- Support controlling mesh_lite internal log output color
- Support whitelist verification during networking

#### Bugfix

- Fixed the problem of slow network recovery after the root node goes offline

## v0.9.0 - 2023-10-31

### Enhancements:

#### Mesh

- Added a "max node number" configuration option.
- Add LAN OTA progress reporting
- When setting the allowed level and disallowed level, it will automatically check whether it conflicts with the current level. If it conflicts, the station will be disconnected and the new parent node will be searched again.
- Fixed leaf nodes being connected by other child nodes.
- Add processing of root nodes with the same signal strength during fusion

#### Bugfix

- Fixed the issue where nodes failed to reconnect to their original parent node after a soft restart
- Resolved the communication error prompt problem in the "no_router" example

## v0.8.0 - 2023-9-19

### Enhancements:

#### Supported ESP-IDF Version

- Add v5.1

#### Supported Socs

- ESP32-C6

## v0.7.0 - 2023-9-4

### Enhancements:

#### Mesh

- feat: Add esp_mesh_lite_start API
- chore(LAN OTA): After LAN OTA is successful, the call of esp_restart is placed outside lib

#### Bugfix

- fix: Fix the memory leak when the network is abnormal

## v0.6.0 - 2023-8-29

### Enhancements:

#### Mesh

- feat: Remove end_with_mac field to accommodate various types of suffixes.

#### Bugfix

- fix: child node cannot join mesh when root disconnect the router

## v0.5.0 - 2023-8-3

### Enhancements:

#### Mesh

- feat: Add join mesh without configured wifi.
- feat: Optimization for the "no_router" scenario.
- feat: Adapted to the new Nova Home APP and improved group control functionality.

#### Bugfix

- fix: circular recovery issue caused by hard reboot

#### Docs

- feat: update image url

## v0.4.0 - 2023-7-12

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
- Remove `CONFIG_LWIP_IPV6=n` to resolve the compilation issue of rainmaker/led_light.
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

### Enhancements:

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
