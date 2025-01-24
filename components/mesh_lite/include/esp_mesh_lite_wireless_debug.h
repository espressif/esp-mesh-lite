/*
 * SPDX-FileCopyrightText: 2023-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "esp_mesh_lite_espnow.h"

#define ESP_LOG_WIRELESS_DEBUG              0x10  // Custom-defined log level for wireless debug

// Type definition for a log writing function that accepts variadic arguments
typedef void (*wireless_debug_log_writev_t)(esp_log_level_t level, const char *tag, const char *format, ...);

#if CONFIG_MESH_LITE_WIRELESS_DEBUG

// Callback type for handling Wi-Fi error messages
typedef char *(*esp_mesh_lite_wireless_debug_wifi_error_cb)(void);

// Callback type for handling cloud error messages
typedef char *(*esp_mesh_lite_wireless_debug_cloud_error_cb)(void);

// Callback type for receiving response data from wireless debug operations
typedef void (*esp_mesh_lite_wireless_debug_recieve_response_data_cb)(char *response_data, size_t len);

// Callback type for receiving debug logs from other devices
typedef void (*esp_mesh_lite_wireless_debug_recieve_debug_log_cb)(const esp_now_recv_info_t *recv_info, const uint8_t *data, int len);

// Structure to store the list of wireless debug callback functions
typedef struct esp_mesh_lite_wireless_debug_cb_list {
    esp_mesh_lite_wireless_debug_wifi_error_cb wifi_error_cb; // Wi-Fi error callback
    esp_mesh_lite_wireless_debug_cloud_error_cb cloud_error_cb; // Cloud error callback
    esp_mesh_lite_wireless_debug_recieve_response_data_cb recv_resp_data_cb; // Callback for receiving response data
    esp_mesh_lite_wireless_debug_recieve_debug_log_cb recv_debug_log_cb; // Callback for receiving debug logs
} esp_mesh_lite_wireless_debug_cb_list_t;

/**
 * @brief Set the function to write wireless debug log messages.
 *
 * @param writev The log writing function to be used for wireless debug.
 */
void esp_mesh_lite_set_wireless_debug_log_writev(wireless_debug_log_writev_t writev);

/**
 * @brief Send a wireless debug command to a specified device.
 *
 * @param dst_mac The destination MAC address.
 * @param command The command to send.
 * @param command_len The length of the command.
 * @param channel The Wi-Fi channel to use for sending the command.
 *
 * @return esp_err_t Returns ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_mesh_lite_wireless_debug_send_command(uint8_t *dst_mac, char *command, size_t command_len, uint8_t channel);

/**
 * @brief Register wireless debug callbacks for handling various debug events.
 *
 * @param cb Pointer to the structure that contains the callback functions.
 *
 * Usage Example: If you need to hide the SSID and cannot use ESP-NOW, you can achieve networking by registering the following callback
 * @code{.c}
 * #include "esp_mesh_lite_wireless_debug.h"
 * static char wifi_error_payload[ESPNOW_PAYLOAD_MAX_LEN];
 * static uint8_t s_broadcast_mac[ESP_NOW_ETH_ALEN] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };
 *
 * char *test_command[] = {
 *     "discover",
 *     "discover --mac 58:cf:79:1e:a2:20",
 *     "discover --channel 11 --mac 58:cf:79:1e:a2:20",
 *     "discover --delay 1000 --channel 11 --mac 58:cf:79:1e:a2:20",
 *     "wifi_error",
 *     "wifi_error --mac 58:cf:79:1e:a2:20",
 *     "wifi_error --channel 11 --mac 58:cf:79:1e:a2:20",
 *     "wifi_error --delay 100 --channel 11 --mac 58:cf:79:1e:a2:20",
 *     "cloud_error",
 *     "cloud_error --mac 58:cf:79:1e:a2:20",
 *     "cloud_error --channel 11 --mac 58:cf:79:1e:a2:20",
 *     "cloud_error --channel 11 --delay 100 --mac 58:cf:79:1e:a2:20",
 *     "core_log --onoff 1 --level 1 --channel 11 --mac 58:cf:79:1e:a2:20",
 *     "core_log --onoff 1 --level 0 --channel 11 --mac 58:cf:79:1e:a2:20",
 *     "core_log --onoff 0 --level 1 --channel 11 --mac 58:cf:79:1e:a2:20",
 *     "core_log --onoff 0 --level 0 --channel 11 --mac 58:cf:79:1e:a2:20",
 *     "reboot",
 * };
 *
 * static void button_press_up_cb(void *hardware_data, void *usr_data)
 * {
 *     ESP_LOGI(TAG, "BTN: BUTTON_PRESS_UP");
 *
 * #ifdef MESH_LITE_WIRELESS_DEBUG
 *     static uint8_t command_index = 0;
 *     size_t command_len = strlen(test_command[command_index]);
 *
 *     wifi_ap_record_t ap_info;
 *     esp_wifi_sta_get_ap_info(&ap_info);
 *
 *     esp_mesh_lite_wireless_debug_send_command(s_broadcast_mac, test_command[command_index], command_len, ap_info.primary);
 *     command_index++;
 *
 *     if (command_index == 13) {
 *         command_index = 0;
 *     }
 * #endif
 * }
 *
 * static char *app_wifi_error_cb(void)
 * {
 *     snprintf(wifi_error_payload, ESPNOW_PAYLOAD_MAX_LEN, "This message from %s", __func__);
 *     return wifi_error_payload;
 * }
 *
 * static char *app_cloud_error_cb(void)
 * {
 *     snprintf(wifi_error_payload, ESPNOW_PAYLOAD_MAX_LEN, "This message from %s", __func__);
 *     return wifi_error_payload;
 * }
 *
 * static void app_recv_resp_data_cb(char *response_data, size_t len)
 * {
 *     char buffer[ESPNOW_PAYLOAD_MAX_LEN];
 *     memset(buffer, 0x0, ESPNOW_PAYLOAD_MAX_LEN);
 *     strncpy(buffer, response_data, len);
 *
 *     char *command_type = strtok(buffer, ":");
 *     char *command_value = strtok(NULL, ".");
 *
 *     if (command_type && command_value) {
 *         printf("Command Type: %s\n", command_type);
 *
 *         if (strcmp(command_type, "discover") == 0 || strcmp(command_type, "core_log") == 0) {
 *             char *mac_address = strtok(command_value, ",");
 *             char *channel = strtok(NULL, ".");
 *
 *             if (mac_address && channel) {
 *                 printf("MAC Address: %s\n", mac_address);
 *                 printf("Channel: %s\n", channel);
 *             } else {
 *                 printf("Invalid format for response value: %s\n", command_value);
 *             }
 *         } else {
 *             printf("Command Value: %s\n", command_value);
 *         }
 *     } else {
 *         printf("Invalid response format: %s\n", buffer);
 *     }
 * }
 *
 * static void app_recv_debug_log_cb(const esp_now_recv_info_t *recv_info, const uint8_t *data, int len)
 * {
 *     printf(""MACSTR": %s", MAC2STR(recv_info->src_addr), (char*)data);
 * }
 *
 * esp_mesh_lite_wireless_debug_cb_list_t cb = {
 *     .wifi_error_cb = app_wifi_error_cb,
 *     .cloud_error_cb = app_cloud_error_cb,
 *     .recv_resp_data_cb = app_recv_resp_data_cb,
 *     .recv_debug_log_cb = app_recv_debug_log_cb,
 * };
 * esp_mesh_lite_wireless_debug_cb_register(&cb);
 * @endcode
 *
 */
void esp_mesh_lite_wireless_debug_cb_register(esp_mesh_lite_wireless_debug_cb_list_t *cb);

/**
 * @brief Initialize the wireless debug feature for ESP-Mesh-Lite.
 *
 * This function sets up necessary configurations for wireless debugging,
 * including setting up the communication channels and enabling mDNS for device discovery.
 * Call this function before using any wireless debugging functionalities.
 *
 */
void esp_mesh_lite_wireless_debug_init(void);

/**
 * @brief Deinitialize the wireless debug feature for ESP-Mesh-Lite.
 *
 * This function cleans up resources allocated for wireless debugging.
 * It should be called when wireless debugging is no longer needed,
 * to free memory and disable the wireless debug functionalities.
 */
void esp_mesh_lite_wireless_debug_deinit(void);

#endif
