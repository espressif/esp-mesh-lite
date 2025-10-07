- [English Version](https://github.com/espressif/esp-mesh-lite/blob/master/examples/no_router/README.md)

# 无路由器示例

## 介绍

本示例将介绍如何快速组建一个无路由器 ESP-Mesh-Lite 网络，其余的组网方案及详细使用方式参见 [README](https://github.com/espressif/esp-mesh-lite/blob/master/examples/mesh_local_control/README_CN.md)，在运行本示例之前请详细阅读 [User Guide](https://github.com/espressif/esp-mesh-lite/blob/master/components/mesh_lite/User_Guide_CN.md)。

## 配置

运行本示例，您至少需要准备两块开发板，一块配置为根节点，其余为非根节点，本示例默认为非根节点类型。

- 根节点：一个 ESP-Mesh-Lite 网络中有且仅有一个根节点，通过 `MESH_LITE_ID` 和信道来区分不同的 `MESH-LITE` 网络
- 非根节点：包含叶子节点和中间节点，其根据网络情况自动选择父节点
	- 叶子节点：叶子节点不会做为中间节点，即不允许有下游链接

你需要通过 `idf.py menuconfig`(CMake)，在 `Example Configuration` 子菜单下，配置一个设备为根节点，其余设备为非根节点。
您也可以在 `Component config -> ESP Wi-Fi Mesh Lite` 子菜单下，配置 ESP-Mesh-Lite 的最大层级、`MESH_LITE_ID` 等参数。

<img src="https://raw.githubusercontent.com/espressif/esp-mesh-lite/master/examples/no_router/example_config.png" alt="example_config" width="80%" div align=center />

<img src="https://raw.githubusercontent.com/espressif/esp-mesh-lite/master/examples/no_router/mesh_config.png" alt="mesh_config" width="80%" div align=center />

## 运行

1. 初始化 Wi-Fi，根据配置启动 ESP-Mesh-Lite；
2. 创建定时器：定时打印 ESP-Mesh-Lite 网络的层级，父节点的信号强度及剩余内存。

```c
I (1386) vendor_ie: Mesh ID: 77
W (1390) vendor_ie: Error Get[4354]
W (1394) vendor_ie: Error Get[4354]
I (1399) ESP_Mesh_Lite_Comm: msg action add success
I (1405) ESP_Mesh_Lite_Comm: Bind Socket 54, port 6364
I (1410) ESP_Mesh_Lite_Comm: Bind Socket 55, port 6363
I (1416) ESP_Mesh_Lite_Comm: Bind Socket 56, port 6366
I (1421) ESP_Mesh_Lite_Comm: Bind Socket 57, port 6365
I (1428) ESP_Mesh_Lite_Comm: msg action add success
I (1433) no_router: Child node
I (1436) Mesh-Lite: Mesh-Lite connecting
I (1442) main_task: Returned from app_main()
I (4274) vendor_ie: Mesh-Lite Scan done
I (4276) vendor_ie: Find ESP_Bridge_1eb259
I (4276) vendor_ie: RTC store: ssid:ESP_Bridge_1eb259; bssid:58:cf:79:1e:b2:59 crc:3027520052
I (6340) wifi:ap channel adjust o:1,1 n:13,2
I (6340) wifi:new:<13,2>, old:<1,1>, ap:<13,2>, sta:<13,2>, prof:1
I (7031) wifi:state: init -> auth (b0)
I (7033) ESP_Mesh_Lite_Comm: Retry to connect to the AP
I (7044) wifi:state: auth -> assoc (0)
I (7057) wifi:state: assoc -> run (10)
I (7071) wifi:connected with ESP_Bridge_1eb259, aid = 1, channel 13, 40D, bssid = 58:cf:79:1e:b2:59
I (7072) wifi:security: WPA2-PSK, phy: bgn, rssi: -12
I (7074) wifi:pm start, type: 1

I (7077) wifi:set rx beacon pti, rx_bcn_pti: 0, bcn_timeout: 0, mt_pti: 25000, mt_time: 10000
I (7154) wifi:AP's beacon interval = 102400 us, DTIM period = 2
I (7155) Mesh-Lite: netif network segment conflict check
I (7156) ip select: IP Address:192.168.5.1
I (7160) ip select: GW Address:192.168.5.1
I (7164) ip select: NM Address:255.255.255.0
I (7170) bridge_common: ip reallocate new:192.168.5.1
W (7175) bridge_wifi: SoftAP IP network segment has changed, deauth all station
I (8087) esp_netif_handlers: sta ip: 192.168.5.2, mask: 255.255.255.0, gw: 192.168.5.1
I (8088) bridge_wifi: Connected with IP Address:192.168.5.2
I (8091) ip select: IP Address:192.168.6.1
I (8096) ip select: GW Address:192.168.6.1
I (8101) ip select: NM Address:255.255.255.0
I (8106) bridge_common: ip reallocate new:192.168.6.1
W (8112) bridge_wifi: SoftAP IP network segment has changed, deauth all station
I (8120) vendor_ie: RTC store: ssid:ESP_Bridge_1eb259; bssid:58:cf:79:1e:b2:59 crc:3027520052
I (11442) no_router: System information, channel: 13, layer: 2, self mac: 58:cf:79:e9:9e:c0, parent bssid: 58:cf:79:1e:b2:59, parent rssi: -14, free heap: 209280
I (21442) no_router: System information, channel: 13, layer: 2, self mac: 58:cf:79:e9:9e:c0, parent bssid: 58:cf:79:1e:b2:59, parent rssi: -13, free heap: 209280
I (31442) no_router: System information, channel: 13, layer: 2, self mac: 58:cf:79:e9:9e:c0, parent bssid: 58:cf:79:1e:b2:59, parent rssi: -13, free heap: 209280
```
