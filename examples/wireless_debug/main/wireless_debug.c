/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <inttypes.h>
#include "esp_log.h"
#include "esp_mac.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/timers.h"

#include "esp_wifi.h"
#include "nvs_flash.h"
#include <sys/socket.h>

#include "esp_bridge.h"
#include "esp_mesh_lite.h"
#include "esp_mesh_lite_wireless_debug.h"

#include "iot_button.h"

#define BUTTON_NUM            1
#define BUTTON_SW1            CONFIG_APP_GPIO_BUTTON_SW1

static button_handle_t g_btns[BUTTON_NUM] = { 0 };
static char wifi_error_payload[ESPNOW_PAYLOAD_MAX_LEN];
static uint8_t s_broadcast_mac[ESP_NOW_ETH_ALEN] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };

static const char *TAG = "wireless_debug";

/**
 * @brief Timed printing system information
 */
static void print_system_info_timercb(TimerHandle_t timer)
{
    uint8_t primary                 = 0;
    uint8_t sta_mac[6]              = {0};
    wifi_ap_record_t ap_info        = {0};
    wifi_second_chan_t second       = 0;
    wifi_sta_list_t wifi_sta_list   = {0x0};

    esp_wifi_sta_get_ap_info(&ap_info);
    esp_wifi_get_mac(ESP_IF_WIFI_STA, sta_mac);
    esp_wifi_ap_get_sta_list(&wifi_sta_list);
    esp_wifi_get_channel(&primary, &second);

    ESP_LOGI(TAG, "System information, channel: %d, layer: %d, self mac: " MACSTR ", parent bssid: " MACSTR
             ", parent rssi: %d, free heap: %"PRIu32"", primary,
             esp_mesh_lite_get_level(), MAC2STR(sta_mac), MAC2STR(ap_info.bssid),
             (ap_info.rssi != 0 ? ap_info.rssi : -120), esp_get_free_heap_size());
#if CONFIG_MESH_LITE_NODE_INFO_REPORT
    ESP_LOGI(TAG, "All node number: %"PRIu32"", esp_mesh_lite_get_mesh_node_number());
#endif /* MESH_LITE_NODE_INFO_REPORT */
    for (int i = 0; i < wifi_sta_list.num; i++) {
        ESP_LOGI(TAG, "Child mac: " MACSTR, MAC2STR(wifi_sta_list.sta[i].mac));
    }
}

static esp_err_t esp_storage_init(void)
{
    esp_err_t ret = nvs_flash_init();

    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        // NVS partition was truncated and needs to be erased
        // Retry nvs_flash_init
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }

    return ret;
}

static void wifi_init(void)
{
    // Station
    wifi_config_t wifi_config = {
        .sta = {
            .ssid = CONFIG_ROUTER_SSID,
            .password = CONFIG_ROUTER_PASSWORD,
        },
    };
    esp_bridge_wifi_set_config(WIFI_IF_STA, &wifi_config);

    // Softap
    wifi_config_t wifi_softap_config = {
        .ap = {
            .ssid = CONFIG_BRIDGE_SOFTAP_SSID,
            .password = CONFIG_BRIDGE_SOFTAP_PASSWORD,
        },
    };
    esp_bridge_wifi_set_config(WIFI_IF_AP, &wifi_softap_config);
}

void app_wifi_set_softap_info(void)
{
    char softap_ssid[33];
    char softap_psw[64];
    uint8_t softap_mac[6];
    size_t ssid_size = sizeof(softap_ssid);
    size_t psw_size = sizeof(softap_psw);
    esp_wifi_get_mac(WIFI_IF_AP, softap_mac);
    memset(softap_ssid, 0x0, sizeof(softap_ssid));
    memset(softap_psw, 0x0, sizeof(softap_psw));

    if (esp_mesh_lite_get_softap_ssid_from_nvs(softap_ssid, &ssid_size) == ESP_OK) {
        ESP_LOGI(TAG, "Get ssid from nvs: %s", softap_ssid);
    } else {
#ifdef CONFIG_BRIDGE_SOFTAP_SSID_END_WITH_THE_MAC
        snprintf(softap_ssid, sizeof(softap_ssid), "%.25s_%02x%02x%02x", CONFIG_BRIDGE_SOFTAP_SSID, softap_mac[3], softap_mac[4], softap_mac[5]);
#else
        snprintf(softap_ssid, sizeof(softap_ssid), "%.32s", CONFIG_BRIDGE_SOFTAP_SSID);
#endif
        ESP_LOGI(TAG, "Get ssid from nvs failed, set ssid: %s", softap_ssid);
    }

    if (esp_mesh_lite_get_softap_psw_from_nvs(softap_psw, &psw_size) == ESP_OK) {
        ESP_LOGI(TAG, "Get psw from nvs: [HIDDEN]");
    } else {
        strlcpy(softap_psw, CONFIG_BRIDGE_SOFTAP_PASSWORD, sizeof(softap_psw));
        ESP_LOGI(TAG, "Get psw from nvs failed, set psw: [HIDDEN]");
    }

    esp_mesh_lite_set_softap_info(softap_ssid, softap_psw);
}

char *test_command[] = {
    "discover",
    "discover --mac 58:cf:79:1e:a2:20",
    "discover --channel 11 --mac 58:cf:79:1e:a2:20",
    "discover --delay 1000 --channel 11 --mac 58:cf:79:1e:a2:20",
    "wifi_error",
    "wifi_error --mac 58:cf:79:1e:a2:20",
    "wifi_error --channel 11 --mac 58:cf:79:1e:a2:20",
    "wifi_error --delay 100 --channel 11 --mac 58:cf:79:1e:a2:20",
    "cloud_error",
    "cloud_error --mac 58:cf:79:1e:a2:20",
    "cloud_error --channel 11 --mac 58:cf:79:1e:a2:20",
    "cloud_error --channel 11 --delay 100 --mac 58:cf:79:1e:a2:20",
    "core_log --onoff 1 --level 1 --channel 11 --mac 58:cf:79:1e:bb:80",
    "core_log --onoff 1 --level 0 --channel 11 --mac 58:cf:79:1e:bb:80",
    "core_log --onoff 0 --level 1 --channel 11 --mac 58:cf:79:1e:bb:80",
    "core_log --onoff 0 --level 0 --channel 11 --mac 58:cf:79:1e:bb:80",
    "reboot",
};

static void button_press_up_cb(void *hardware_data, void *usr_data)
{
    ESP_LOGI(TAG, "BTN: BUTTON_PRESS_UP");

#ifdef CONFIG_MESH_LITE_WIRELESS_DEBUG
    static uint8_t command_index = 0;
    size_t command_len = strlen(test_command[command_index]);

    wifi_ap_record_t ap_info;
    esp_wifi_sta_get_ap_info(&ap_info);

    esp_mesh_lite_wireless_debug_send_command(s_broadcast_mac, test_command[command_index], command_len, ap_info.primary);
    command_index++;

    if (command_index == 17) {
        command_index = 0;
    }
#endif
}

static char *app_wifi_error_cb(void)
{
    snprintf(wifi_error_payload, ESPNOW_PAYLOAD_MAX_LEN, "This message from %s", __func__);
    return wifi_error_payload;
}

static char *app_cloud_error_cb(void)
{
    snprintf(wifi_error_payload, ESPNOW_PAYLOAD_MAX_LEN, "This message from %s", __func__);
    return wifi_error_payload;
}

static void app_recv_resp_data_cb(char *response_data, size_t len)
{
    char buffer[ESPNOW_PAYLOAD_MAX_LEN];
    memset(buffer, 0x0, ESPNOW_PAYLOAD_MAX_LEN);
    strncpy(buffer, response_data, len);

    char *command_type = strtok(buffer, ":");
    char *command_value = strtok(NULL, ".");

    if (command_type && command_value) {
        printf("Command Type: %s\n", command_type);

        if (strcmp(command_type, "discover") == 0 || strcmp(command_type, "core_log") == 0) {
            char *mac_address = strtok(command_value, ",");
            char *channel = strtok(NULL, ".");

            if (mac_address && channel) {
                printf("MAC Address: %s\n", mac_address);
                printf("Channel: %s\n", channel);
            } else {
                printf("Invalid format for response value: %s\n", command_value);
            }
        } else {
            printf("Command Value: %s\n", command_value);
        }
    } else {
        printf("Invalid response format: %s\n", buffer);
    }
}

static void app_recv_debug_log_cb(const esp_now_recv_info_t *recv_info, const uint8_t *data, int len)
{
    printf(""MACSTR": %s", MAC2STR((uint8_t *)recv_info->src_addr), (char*)data);
}

esp_mesh_lite_wireless_debug_cb_list_t cb = {
                                                .wifi_error_cb = app_wifi_error_cb,
                                                .cloud_error_cb = app_cloud_error_cb,
                                                .recv_resp_data_cb = app_recv_resp_data_cb,
                                                .recv_debug_log_cb = app_recv_debug_log_cb,
                                            };

void app_main()
{
    esp_log_level_set("*", ESP_LOG_INFO);

    esp_storage_init();

    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());

    esp_bridge_create_all_netif();

    wifi_init();

    esp_mesh_lite_config_t mesh_lite_config = ESP_MESH_LITE_DEFAULT_INIT();
    esp_mesh_lite_init(&mesh_lite_config);

    app_wifi_set_softap_info();

    esp_mesh_lite_wireless_debug_cb_register(&cb);

    esp_mesh_lite_start();

    TimerHandle_t timer = xTimerCreate("print_system_info", 10000 / portTICK_PERIOD_MS,
                                       true, NULL, print_system_info_timercb);
    xTimerStart(timer, 0);

    button_config_t cfg = {
                              .type = BUTTON_TYPE_GPIO,
                              .gpio_button_config = {
                                                        .gpio_num = BUTTON_SW1,
                                                        .active_level = 0,
                                                    },
                          };
    g_btns[0] = iot_button_create(&cfg);
    iot_button_register_cb(g_btns[0], BUTTON_PRESS_UP, button_press_up_cb, 0);
}
