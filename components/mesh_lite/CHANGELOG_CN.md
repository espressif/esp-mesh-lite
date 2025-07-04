# ChangeLog

## v1.0.2 - 2025-7-4

### Breaking Change:

- lan_ota_file_transfer： 为 get_file_cb 增加更多参数 ([6265503](https://github.com/espressif/esp-mesh-lite/commit/62655031e5dc527d35ebb2c51389f100208991cd))
- 弃用 esp_mesh_lite_get_child_node_number，改用 esp_mesh_lite_get_mesh_node_number ([a38ba0e](https://github.com/espressif/esp-mesh-lite/commit/a38ba0e68d48a93aa0c536f5a43bf657a5cccaf3))

### 功能:

#### Mesh

- 优化 Station 断开后，mesh lite 内部的扫描时间
- 支持动态设置 Wi-Fi 重连间隔 ([803c8e9](https://github.com/espressif/esp-mesh-lite/commit/803c8e93e9df459fefe29794b66464ce0c21bf5f))
- 新增对 ESP32-C61 的支持 ([4a2b7e4](https://github.com/espressif/esp-mesh-lite/commit/4a2b7e4b4c04b3c96c941f8b8e82758f0c5c4bfb))
- 优化部分场景下的融合过程，缩短融合时间 ([544b925](https://github.com/espressif/esp-mesh-lite/commit/544b925c705b80acf823216f8236bf57339c7b9f))
- Mesh Lite 连接中，若待连接设备与当前连接设备相同，无需断开重连 ([544b925](https://github.com/espressif/esp-mesh-lite/commit/544b925c705b80acf823216f8236bf57339c7b9f))
- 支持 espnow 发送更长的数据 ([29185cc](https://github.com/espressif/esp-mesh-lite/commit/29185cc323d199e609d1ff75fe21e5dab95c4e34))
- 新增 API 以获取当前 mesh-lite 库的 commit id ([6265503](https://github.com/espressif/esp-mesh-lite/commit/62655031e5dc527d35ebb2c51389f100208991cd))
- 当设备被禁止成为根节点时将不会被加入到 “遗嘱” 列表中 ([6265503](https://github.com/espressif/esp-mesh-lite/commit/62655031e5dc527d35ebb2c51389f100208991cd))
- 当前设备的层级超过最大限制时，断开其子节点的 sta 连接 ([9c33dcf](https://github.com/espressif/esp-mesh-lite/commit/9c33dcf1c484ef91abc5227565100c67fb6d6f3d))
- 支持注册发送消息失败的回调函数 ([0b8918b](https://github.com/espressif/esp-mesh-lite/commit/0b8918b28f9bb6a8ccf2f676c2005e5b0d30cdf6))
- 优化固定根节点设备 esp_mesh_lite_connect 的执行逻辑，缩短连接时间 ([0b8918b](https://github.com/espressif/esp-mesh-lite/commit/0b8918b28f9bb6a8ccf2f676c2005e5b0d30cdf6))
- 将 `esp_ota_mark_app_valid_cancel_rollback` 从 mesh lite lib 中移除 ([56d5ad2](https://github.com/espressif/esp-mesh-lite/commit/56d5ad2778d9dec1217434247fe0122fe7f960e6))
- 优化 wifi 重连逻辑 ([8f8f9a1](https://github.com/espressif/esp-mesh-lite/commit/8f8f9a164905f6f90b59c1f6c4db599b81c09eb7))
- 重新定义获取节点总数接口 ([8f8f9a1](https://github.com/espressif/esp-mesh-lite/commit/8f8f9a164905f6f90b59c1f6c4db599b81c09eb7))
- 扩展 esp_mesh_lite_espnow_recv_cb 的参数以适应更多场景 ([eaf4e0a](https://github.com/espressif/esp-mesh-lite/commit/eaf4e0aa47da2a4d93c4d058395f67575d33161b))

#### Zero Provisioning

- 开放 zero_prov_listening API，支持设备随时进入监听状态，实现新上电设备的零配入网 ([c99d261](https://github.com/espressif/esp-mesh-lite/commit/c99d261f71b6c1528cde00a75d87bb8d72b7bdcc))
- 支持重复触发零配功能（需配置 CONFIG_PROV_RELEASE_BLE_MEMORY_AFTER_PROVISIONED=false）([c99d261](https://github.com/espressif/esp-mesh-lite/commit/c99d261f71b6c1528cde00a75d87bb8d72b7bdcc))
- 优化主配设备执行逻辑，避免主配设备因距离路由器过远导致所有设备零配失败 ([c99d261](https://github.com/espressif/esp-mesh-lite/commit/c99d261f71b6c1528cde00a75d87bb8d72b7bdcc))
- 根据国家码自动适配广播时可用信道 ([c99d261](https://github.com/espressif/esp-mesh-lite/commit/c99d261f71b6c1528cde00a75d87bb8d72b7bdcc))

#### Wi-Fi Provisioning

- 支持设备不重启条件下重复配网 ([402f438](https://github.com/espressif/esp-mesh-lite/commit/402f4385d6117da84e0aace6d365847eb13ca12c))

#### Bugfix

- 修复调用 esp_mesh_lite_disconnect 后，偶现仍触发 Wi-Fi 重连的问题
- 修复设备自荐功能失效的问题 ([2f89a55](https://github.com/espressif/esp-mesh-lite/commit/2f89a55ccdd9713c1b13cb5ed98419a3bc48f7c7))
- 修复当根节点与路由器断开连接后，二级节点无法与根节点正常通信的问题 ([2f89a55](https://github.com/espressif/esp-mesh-lite/commit/2f89a55ccdd9713c1b13cb5ed98419a3bc48f7c7))
- 修复原生 espnow API 发送的数据接收失败的问题 ([f176342](https://github.com/espressif/esp-mesh-lite/commit/f17634270d14c907e48611d084217c6514a85920))
- 修复通信过程中偶现的内存泄漏问题 ([f176342](https://github.com/espressif/esp-mesh-lite/commit/f17634270d14c907e48611d084217c6514a85920))
- 修复根节点重新设置 Mesh ID 后，与路由器断开重连的问题 ([f176342](https://github.com/espressif/esp-mesh-lite/commit/f17634270d14c907e48611d084217c6514a85920))
- 修复当通信数据通过 aes 加密时的 LAN OTA 升级以及文件传输失败的问题 ([70f5657](https://github.com/espressif/esp-mesh-lite/commit/70f5657e91e065546f2e626b8a4042de0a948f93))
- 修复当前 RSSI 低于重新设置的父节点 RSSI 阈值时，未触发父节点变更的问题 ([d84f81b](https://github.com/espressif/esp-mesh-lite/commit/d84f81b2309b2ba1ba1819364a99d61f88c73af3))
- 修复设备设置为叶子节点后，其子节点未断开连接的问题 ([d84f81b](https://github.com/espressif/esp-mesh-lite/commit/d84f81b2309b2ba1ba1819364a99d61f88c73af3))
- 修复多个根节点 RSSI 值相同时进行融合，可能导致节点总数超过最大限制的问题 ([d84f81b](https://github.com/espressif/esp-mesh-lite/commit/d84f81b2309b2ba1ba1819364a99d61f88c73af3))
- 修复父节点 mesh id 变更后，其子节点未断开连接的问题 ([d84f81b](https://github.com/espressif/esp-mesh-lite/commit/d84f81b2309b2ba1ba1819364a99d61f88c73af3))
- 修复父节点层级变更后，不符合固定/禁用层级要求的设备未断开连接的问题 ([d84f81b](https://github.com/espressif/esp-mesh-lite/commit/d84f81b2309b2ba1ba1819364a99d61f88c73af3))
- 修复设备连接 Wi-Fi 之前 node number 错误的问题 ([5094532](https://github.com/espressif/esp-mesh-lite/commit/50945329140209bdf3f0f01625a6a7bb798ee683))
- 修复执行 esp_mesh_lite_disconnect 之后仍会触发 Wi-Fi 重连的问题 ([fc251d9](https://github.com/espressif/esp-mesh-lite/commit/fc251d967f066421d505aa25befc09c5da2a1be3))
- 修复调用 `esp_wifi_connect()` 失败时错误地更新 Vendor IE 信息 ([544b925](https://github.com/espressif/esp-mesh-lite/commit/544b925c705b80acf823216f8236bf57339c7b9f))
- 修复新设备加入满载网络时可能会破坏原有拓扑结构的问题 ([544b925](https://github.com/espressif/esp-mesh-lite/commit/544b925c705b80acf823216f8236bf57339c7b9f))
- 修复设备作为固定根节点时，调用 `esp_mesh_lite_connect()` 未触发重连机制的问题，该问题由提交 [0b8918b](https://github.com/espressif/esp-mesh-lite/commit/0b8918b28f9bb6a8ccf2f676c2005e5b0d30cdf6) 引入 ([544b925](https://github.com/espressif/esp-mesh-lite/commit/544b925c705b80acf823216f8236bf57339c7b9f))
- 将废弃的 `esp_mesh_lite_try_sending_msg()` 替换为 `esp_mesh_lite_send_msg()` ([544b925](https://github.com/espressif/esp-mesh-lite/commit/544b925c705b80acf823216f8236bf57339c7b9f))
- 修复特殊情况下的异常融合行为 ([544b925](https://github.com/espressif/esp-mesh-lite/commit/544b925c705b80acf823216f8236bf57339c7b9f))
    - 固定为根节点且仅作为叶节点的设备，误触发了融合导致其反复断开路由器
    - 固定为根节点且仅作为叶节点的设备，误触发其它设备融合，导致其他根节点反复断开路由器
    - 最高层级设置为 1 的根节点，误触发其它设备融合，导致其他根节点反复断开路由器
- 修复在 send_fail cb 中调用 esp_mesh_lite_send_msg 会造成系统卡死问题 ([6265503](https://github.com/espressif/esp-mesh-lite/commit/62655031e5dc527d35ebb2c51389f100208991cd))
- 修复当不禁用设备层级时，无法设置设备 level 为 0 的问题 ([6265503](https://github.com/espressif/esp-mesh-lite/commit/62655031e5dc527d35ebb2c51389f100208991cd))
- 修复在子节点数量没有超过最大限制的情况下，最后一个接入的子节点被 deauth 的问题 ([6265503](https://github.com/espressif/esp-mesh-lite/commit/62655031e5dc527d35ebb2c51389f100208991cd))
- 修复在 mesh lite 内部两个扫描动作连续执行时互相影响导致 Wi-Fi 连接异常的问题 ([6265503](https://github.com/espressif/esp-mesh-lite/commit/62655031e5dc527d35ebb2c51389f100208991cd))
- 修复当发送 json 数据进行通信时，payload 为 NULL 会 send fail 的问题 ([6265503](https://github.com/espressif/esp-mesh-lite/commit/62655031e5dc527d35ebb2c51389f100208991cd))
- 修复 lan ota 时，固件下载完成后，Wi-Fi 变更导致重新 OTA 的问题 ([6265503](https://github.com/espressif/esp-mesh-lite/commit/62655031e5dc527d35ebb2c51389f100208991cd))
- 修复 raw data 通信时的 seq 异常问题 ([8f8f9a1](https://github.com/espressif/esp-mesh-lite/commit/8f8f9a164905f6f90b59c1f6c4db599b81c09eb7))
- 修复了新设备验证逻辑意外未执行的情况 ([8f8f9a1](https://github.com/espressif/esp-mesh-lite/commit/8f8f9a164905f6f90b59c1f6c4db599b81c09eb7))
- 修复了以下问题：设备被子节点连满后，子节点即使断开，该设备也不会再被当作父节点来进行选择，此问题由 888603ef 引入 ([8f8f9a1](https://github.com/espressif/esp-mesh-lite/commit/8f8f9a164905f6f90b59c1f6c4db599b81c09eb7))
- 修复了以下问题：设备成功执行了父节点发现动作，但是没有执行连接 ([8f8f9a1](https://github.com/espressif/esp-mesh-lite/commit/8f8f9a164905f6f90b59c1f6c4db599b81c09eb7))

#### Chore

- 将 raw msg 通信的默认重传间隔修改为 1000ms ([d84f81b](https://github.com/espressif/esp-mesh-lite/commit/d84f81b2309b2ba1ba1819364a99d61f88c73af3))
- 优化 esp_mesh_lite_aes_set_key 的返回值 ([d84f81b](https://github.com/espressif/esp-mesh-lite/commit/d84f81b2309b2ba1ba1819364a99d61f88c73af3))
- 更新部分日志的输出级别 ([0b8918b](https://github.com/espressif/esp-mesh-lite/commit/0b8918b28f9bb6a8ccf2f676c2005e5b0d30cdf6))

## v1.0.1 - 2024-12-16

### 功能:

#### Mesh

- 支持设置消息重传频率 ([888603e](https://github.com/espressif/esp-mesh-lite/commit/888603ef756e70f8fa8aa7e44b36ade11ecf4362))
- 支持无线 debug 功能 ([6c735a6](https://github.com/espressif/esp-mesh-lite/commit/6c735a6406e8cc9f06bee89c187e40850c120a0a))
- 支持 LAN OTA 时断点续传功能 ([6c735a6](https://github.com/espressif/esp-mesh-lite/commit/6c735a6406e8cc9f06bee89c187e40850c120a0a))
- LAN OTA 之前验证固件的 MD5 值 ([6c735a6](https://github.com/espressif/esp-mesh-lite/commit/6c735a6406e8cc9f06bee89c187e40850c120a0a))
- 优化设置组网模式后的一些执行逻辑 ([6c735a6](https://github.com/espressif/esp-mesh-lite/commit/6c735a6406e8cc9f06bee89c187e40850c120a0a))
- 在禁用 ESP—NOW 功能时，内部将不创建相关的 task 以节省 RAM 资源 ([7aec4a6](https://github.com/espressif/esp-mesh-lite/commit/7aec4a6555a933459181be4ea8ac0a27409585ea))
- 支持 esp32c5 ([46d3e3e](https://github.com/espressif/esp-mesh-lite/commit/46d3e3e9a4acbeaad341bf6fef75b5605bcaf3d2))
- 优化组网速度 ([46d3e3e](https://github.com/espressif/esp-mesh-lite/commit/46d3e3e9a4acbeaad341bf6fef75b5605bcaf3d2))
- 根节点向子节点同步路由器信息时加入 bssid ([46d3e3e](https://github.com/espressif/esp-mesh-lite/commit/46d3e3e9a4acbeaad341bf6fef75b5605bcaf3d2))
- 支持隐藏 ssid 场景注册 ssid by mac cb 来获取对端 ssid ([0579dfa](https://github.com/espressif/esp-mesh-lite/commit/0579dfa2b61bb4442d427122d5079397299b88d8))
- 开放 mesh-lite log 控制 API ([defe726](https://github.com/espressif/esp-mesh-lite/commit/defe726f1f98de17f8d1795507dab15ba0bf3917))
- 兼容旧版本的融合机制 ([870642f](https://github.com/espressif/esp-mesh-lite/commit/870642fb799ec23dc87966d22572dc847a371a3c))
- 支持接收原生 espnow API 发送的的数据 ([a6f1544](https://github.com/espressif/esp-mesh-lite/commit/a6f15443857f87b721952bec79f237b98d39f66f))
- 支持内部通信通过 Proto buffer 来传输二进制数据 ([27c511c](https://github.com/espressif/esp-mesh-lite/commit/27c511cc9a80304f68f5bf7e6e7fe3e073d21095))
- 优化网络组建速度 ([0ba98f0](https://github.com/espressif/esp-mesh-lite/commit/0ba98f0b66a914cee527997fb51cd5106b9f5272))
- 在层级变更时尝试优化当前拓扑结构 ([0ba98f0](https://github.com/espressif/esp-mesh-lite/commit/0ba98f0b66a914cee527997fb51cd5106b9f5272))
- 在 LAN OTA 中增加 eth_got_ip 事件处理 ([0ba98f0](https://github.com/espressif/esp-mesh-lite/commit/0ba98f0b66a914cee527997fb51cd5106b9f5272))
- 优化多个根节点之间信号强度较弱时的融合处理 ([0ba98f0](https://github.com/espressif/esp-mesh-lite/commit/0ba98f0b66a914cee527997fb51cd5106b9f5272))

#### Bugfix

- 修复固定叶节点开启 softap 时，被误选为父节点的问题 ([888603e](https://github.com/espressif/esp-mesh-lite/commit/888603ef756e70f8fa8aa7e44b36ade11ecf4362))
- 修复当在 esp_mesh_lite_connect 期间找不到合适的父节点时，固定或禁用设备层级失败 ([888603e](https://github.com/espressif/esp-mesh-lite/commit/888603ef756e70f8fa8aa7e44b36ade11ecf4362))
- 修复节点下 station 数量超过设置的最大连接数量 ([888603e](https://github.com/espressif/esp-mesh-lite/commit/888603ef756e70f8fa8aa7e44b36ade11ecf4362))
- 修复固定根节点的设备与路由器断开时层级发生变化 ([888603e](https://github.com/espressif/esp-mesh-lite/commit/888603ef756e70f8fa8aa7e44b36ade11ecf4362))
- 修复一些融合失败的问题 ([7aec4a6](https://github.com/espressif/esp-mesh-lite/commit/7aec4a6555a933459181be4ea8ac0a27409585ea))
- 修复LAN OTA 使能与不使能设备的兼容问题 ([7aec4a6](https://github.com/espressif/esp-mesh-lite/commit/7aec4a6555a933459181be4ea8ac0a27409585ea))
- 修复使用 ESP-NOW 时可能对隐藏 ssid 下的组网造成的影响 ([7aec4a6](https://github.com/espressif/esp-mesh-lite/commit/7aec4a6555a933459181be4ea8ac0a27409585ea))
- 增加 wifi failure_retry_cnt 以降低组网时连接失败的概率 ([7aec4a6](https://github.com/espressif/esp-mesh-lite/commit/7aec4a6555a933459181be4ea8ac0a27409585ea))
- 修复多次调用 esp_mesh_lite_transmit_file_start 传输不同文件时偶现失败的问题 ([46d3e3e](https://github.com/espressif/esp-mesh-lite/commit/46d3e3e9a4acbeaad341bf6fef75b5605bcaf3d2))
- 修复偶现融合失败的问题 ([0579dfa](https://github.com/espressif/esp-mesh-lite/commit/0579dfa2b61bb4442d427122d5079397299b88d8))
- 修复通信过程中偶现崩溃的问题 ([0579dfa](https://github.com/espressif/esp-mesh-lite/commit/0579dfa2b61bb4442d427122d5079397299b88d8))
- 修复由于 IDF 版本之间 wifi_ap_record_t 结构差异导致的崩溃 ([0579dfa](https://github.com/espressif/esp-mesh-lite/commit/0579dfa2b61bb4442d427122d5079397299b88d8))
- 修复层级显示异常 ([870642f](https://github.com/espressif/esp-mesh-lite/commit/870642fb799ec23dc87966d22572dc847a371a3c))
- 修复层级变更导致拓扑优化时选择最优父节点异常 ([870642f](https://github.com/espressif/esp-mesh-lite/commit/870642fb799ec23dc87966d22572dc847a371a3c))
- 修复 SSID 隐藏时组网失败 ([870642f](https://github.com/espressif/esp-mesh-lite/commit/870642fb799ec23dc87966d22572dc847a371a3c))
- 修复 [27c511c](https://github.com/espressif/esp-mesh-lite/commit/27c511cc9a80304f68f5bf7e6e7fe3e073d21095) 引入的内部通信异常导致的 crash ([2979846](https://github.com/espressif/esp-mesh-lite/commit/29798464f85f636eb640e867371fa7716d31d8ff))
- 修复 [27c511c](https://github.com/espressif/esp-mesh-lite/commit/27c511cc9a80304f68f5bf7e6e7fe3e073d21095) 引入的内存泄漏 ([2979846](https://github.com/espressif/esp-mesh-lite/commit/29798464f85f636eb640e867371fa7716d31d8ff))

## v1.0.0 - 2024-7-12

### 功能:

#### Mesh

- 新增 esp_mesh_lite_disconnect API ([5075c94](https://github.com/espressif/esp-mesh-lite/commit/5075c94dd026a5fb12e569b693fd59b8781abc3d))
- 优化最佳父节点的选择 ([5075c94](https://github.com/espressif/esp-mesh-lite/commit/5075c94dd026a5fb12e569b693fd59b8781abc3d))

#### Bugfix

- 修复内部通信数据解析异常导致的 crash ([1881ef8](https://github.com/espressif/esp-mesh-lite/commit/1881ef8ad900e31902131e62e27b7b848c1a686a))
- 修复连接加密方式为 WEP/WPA 的路由器失败 ([80adbff](https://github.com/espressif/esp-mesh-lite/commit/80adbffb478d852256cc5fb36a1bca96956427af))

## v0.10.5 - 2024-6-7

### 功能:

#### Bugfix

- 适配 iot-bridge 组件至 0.11.7 ([20e4eb9](https://github.com/espressif/esp-mesh-lite/commit/20e4eb96949538e3d2512c06c3495194af5b6ef8))
- LAN OTA： 修复升级过程中无法重新触发升级 ([20e4eb9](https://github.com/espressif/esp-mesh-lite/commit/20e4eb96949538e3d2512c06c3495194af5b6ef8))
- LAN OTA： 修复一些升级失败问题 ([a99aeeb](https://github.com/espressif/esp-mesh-lite/commit/a99aeeb67aa48078f670053ca32a8d2487a70034))
    - 优化一些机制，减少升级时高层级节点的压力
    - 修复升级过程中拓扑结构变化导致的升级失败问题
    - 修复父节点升级完成重启后，子节点不继续请求固件的问题
- 修复子节点上报根节点消息异常问题 ([0dfe6b5](https://github.com/espressif/esp-mesh-lite/commit/0dfe6b5a0df3339472f32b7b263fab809405b734))

#### Mesh

- 支持 idf/v5.2 & idf/v5.2 ([a27afb1](https://github.com/espressif/esp-mesh-lite/commit/a27afb1aaed3910ae22bb1b46a3c13264f044f8f))
- 移除对 idf/v4.3 & idf/v4.4 的支持 ([a27afb1](https://github.com/espressif/esp-mesh-lite/commit/a27afb1aaed3910ae22bb1b46a3c13264f044f8f))

## v0.10.4 - 2024-5-22

### 功能:

#### Mesh

- 增加配置项，OTA 结束后，直接重启芯片或者交由用户通过事件处理控制重启 ([ebeb361](https://github.com/espressif/esp-mesh-lite/commit/ebeb3617ae435f707a2e9aeabd41410513f6594f))

#### Bugfix

- 修复子节点上报根节点消息异常问题 ([50c2ed9](https://github.com/espressif/esp-mesh-lite/commit/50c2ed9ac72464c4491b1e614274e63a9d72e7ae))
- 修复软重启后意外连接至其子节点形成环形 ([50c2ed9](https://github.com/espressif/esp-mesh-lite/commit/50c2ed9ac72464c4491b1e614274e63a9d72e7ae))
- 修复子节点意外触发融合逻辑导致网络异常 ([50c2ed9](https://github.com/espressif/esp-mesh-lite/commit/50c2ed9ac72464c4491b1e614274e63a9d72e7ae))
- 修复意外情况下重联机制无法生效 ([7148df7](https://github.com/espressif/esp-mesh-lite/commit/7148df7693929ea0e3eecd4628c8a10da611d33c))

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
