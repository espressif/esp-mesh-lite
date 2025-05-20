- [中文版本](https://github.com/espressif/esp-mesh-lite/blob/master/examples/wireless_debug/README_CN.md)

| Supported Targets | ESP32 | ESP32-C3 | ESP32-S2 | ESP32-S3 | ESP32-C2 | ESP32-C6 |
| ----------------- | ----- | -------- | -------- | -------- | -------- | -------- |

# **ESP-Mesh-Lite Wireless Debug Example**

## **1. Feature Overview**
**Wireless Debug** is an innovative wireless debugging solution that enables developers to control target devices without physical UART or JTAG connections. It provides real-time status information of devices in the Mesh-Lite network, including Wi-Fi connection status, application layer connection status, and debug logs. Furthermore, developers can retrieve runtime logs and connection status of offline devices through cloud services or online device forwarding, significantly improving the debugging efficiency of distributed devices.

---

## **2. Typical Use Cases**
### **(1) Multi-Device Collaborative Debugging**
Enables centralized monitoring of all devices in a Mesh-Lite network without requiring individual UART connections for debugging.

### **(2) Field Troubleshooting**
Facilitates quick access to fault logs from devices installed in hard-to-reach locations (such as embedded sensors and industrial controllers) through wireless communication.

---

## **3. Hardware Requirements**
| Device | Quantity | Notes |
|--------|----------|-------|
| ESP32/ESP32-C3/ESP32-S3 etc. | ≥2 | Minimum requirement: 1 **Debug Host** and 1 **Target Device** |
| 2.4GHz Router | 1 | All devices must connect to the same router to ensure channel synchronization |

---

## **4. Configuration**
Navigate to **Example Configuration** in `idf.py menuconfig` and configure the following parameters:

| Option | Description |
|--------|-------------|
| `Router SSID` | Wi-Fi network name |
| `Router Password` | Wi-Fi network password |
| `APP_GPIO_BUTTON_SW1` | GPIO pin number for button input |

---

## **5. Build and Flash**  
```bash
# Erase and flash firmware
idf.py erase_flash flash monitor -p /dev/ttyUSB0
```
> Replace `/dev/ttyUSB0` with your actual port.  

---

## **6. Sample Output**  
When operational, the debug host can send wireless commands via button presses. Serial output shows:  
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
