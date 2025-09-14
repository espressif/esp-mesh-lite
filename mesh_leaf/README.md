- [中文版本](https://github.com/espressif/esp-mesh-lite/blob/master/examples/no_router/README_CN.md)

# No Router Example

## Introduction

It introduces a quick way to build an ESP-Mesh-Lite network without a router. For details on other network configuration methods, please refer to [README](https://github.com/espressif/esp-mesh-lite/blob/master/examples/mesh_local_control/README.md). Before running this example, please firstly go through [User Guide](https://github.com/espressif/esp-mesh-lite/blob/master/components/mesh_lite/User_Guide.md).

## Configure

To run this example, you need at least two development boards, one configured as a root node, and the other a non-root node. In this example, all the devices are non-root nodes by default.

- Root node: There is only one root node in an ESP-Mesh-Lite network. `MESH-LITE` networks can be differentiated by their `MESH_LITE_ID` and channels.
- Non-root node: Includes leaf nodes and intermediate nodes, which automatically select their parent nodes according to the network conditions.
	- Leaf node: A leaf node cannot also be an intermediate node, which means leaf node cannot has any child nodes.

You need to go to the submenu `Example Configuration` and configure one device as a root node, and the others as non-root nodes with `idf.py menuconfig`(CMake). 

You can also go to the submenu `Component config -> ESP Wi-Fi Mesh Lite`, and configure the ESP-Mesh-Lite related parameters such as the max number of layers allowed and the `MESH_LITE_ID`.

<img src="https://raw.githubusercontent.com/espressif/esp-mesh-lite/master/examples/no_router/example_config.png" alt="example_config" width="80%" div align=center />

<img src="https://raw.githubusercontent.com/espressif/esp-mesh-lite/master/examples/no_router/mesh_config.png" alt="mesh_config" width="80%" div align=center />

## Run

1. Initialize Wi-Fi, and start ESP-Mesh-Lite according to configuration;
2. Create a timer: print at the specified time the ESP-Mesh-Lite network information about layers, and the parent node's signal strength and the remaining memory.

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
