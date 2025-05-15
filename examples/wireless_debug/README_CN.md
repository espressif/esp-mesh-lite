- [English Version](https://github.com/espressif/esp-mesh-lite/blob/master/examples/wireless_debug/README.md)

| Supported Targets | ESP32 | ESP32-C3 | ESP32-S2 | ESP32-S3 | ESP32-C2 | ESP32-C6 |
| ----------------- | ----- | -------- | -------- | -------- | -------- | -------- |

# **ESP-Mesh-Lite 无线调试示例**

## **1. 功能概述**
**Wireless Debug** 是一种创新的无线调试解决方案。该方案使开发者无需通过物理连接 UART 或 JTAG 接口，即可通过无线方式控制目标设备，并实时获取 Mesh-Lite 网络中其他设备的运行状态信息(包括 Wi-Fi 连接状态、应用层连接状态、调试日志等)。此外，开发者还可以通过云端或在线设备转发的方式获取离线设备的运行日志和连接状态，大大提升了分布式设备的调试效率。

---

## **2. 应用场景**
### **(1) 多设备协同调试**
当多个 ESP32 设备组成 Mesh-Lite 网络时，可通过 **Wireless Debug** 实现对所有设备运行状态的集中监控，无需逐一连接串口进行调试。

### **(2) 现场故障诊断**
对于部署在难以直接接触的位置的设备（如嵌入式传感器、工业控制器等），可通过无线方式快速获取故障日志，提高问题诊断效率。

---

## **3. 硬件要求**
| 设备 | 数量 | 说明 |
|------|------|------|
| ESP32/ESP32-C3/ESP32-S3 等 | ≥2 | 至少需要 1 个作为 **调试主机**，1 个作为 **被调试设备** |
| 2.4GHz 路由器 | 1 | 所有设备需连接至同一路由器，确保工作在相同信道 |

---

## **4. 配置说明**
运行 `idf.py menuconfig`，在 **Example Configuration** 中配置以下参数：

| 配置项 | 说明 |
|--------|------|
| `Router SSID` | 路由器 SSID |
| `Router Password` | 路由器密码 |
| `APP_GPIO_BUTTON_SW1` | 按键引脚编号 |

---

## **5. 编译与烧录**  
```bash
# 清除旧固件并烧录新固件
idf.py erase_flash flash monitor -p /dev/ttyUSB0
```
> 将 `/dev/ttyUSB0` 替换为实际串口设备。  

---

## **6. 示例输出**  
成功运行后，调试主机可通过按键模拟发送对应无线调试指令，串口会显示：  
```
Command Type: discover
MAC Address: 58:cf:79:1e:b1:14
Channel: 11
I (329519) wireless_debug: BTN: BUTTON_PRESS_UP
I (329519) Mesh-Lite-Wireless-Debug: send command: discover --delay 1000 --channel 11 --mac 58:cf:79:1e:b1:14
I (330094) wireless_debug: BTN: BUTTON_PRESS_UP
I (330094) Mesh-Lite-Wireless-Debug: send command: wifi_error
Command Type: discover
MAC Address: 58:cf:79:1e:b1:14
Channel: 11
I (330694) wireless_debug: BTN: BUTTON_PRESS_UP
I (330694) Mesh-Lite-Wireless-Debug: send command: wifi_error --mac 58:cf:79:1e:b1:14
Command Type: wifi_error
Command Value: This message from app_wifi_error_cb
```

---
