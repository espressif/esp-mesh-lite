/*
 * Root node: Mesh-Lite + UDP listener + UART bridge + log-only UI
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include <string.h>
#include <inttypes.h>
#include <sys/socket.h>

#include "lwip/inet.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/timers.h"

#include "esp_err.h"
#include "esp_event.h"
#include "esp_log.h"
#include "esp_mac.h"
#include "esp_netif.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "nvs_flash.h"

#include "esp_bridge.h"        // <-- for esp_bridge_create_all_netif()
#include "esp_mesh_lite.h"

#include "root_udp.h"
#include "uart_bridge.h"
#include "ui_display.h"

#ifndef FORCE_ROOT
#define FORCE_ROOT 1          // this build is the ROOT
#endif

static const char *TAG = "no_router";

/* ---------------- NVS ---------------- */
static esp_err_t esp_storage_init(void)
{
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    return ret;
}

/* ---------------- SoftAP helpers ---------------- */
static void app_wifi_set_softap_info(void)
{
    char    softap_ssid[33];
    char    softap_psw[64];
    uint8_t softap_mac[6];
    size_t  ssid_size = sizeof(softap_ssid);
    size_t  psw_size  = sizeof(softap_psw);

    ESP_ERROR_CHECK(esp_wifi_get_mac(WIFI_IF_AP, softap_mac));

    memset(softap_ssid, 0, sizeof(softap_ssid));
    memset(softap_psw,  0, sizeof(softap_psw));

    if (esp_mesh_lite_get_softap_ssid_from_nvs(softap_ssid, &ssid_size) == ESP_OK) {
        ESP_LOGI(TAG, "SoftAP SSID (nvs): %s", softap_ssid);
    } else {
    #ifdef CONFIG_BRIDGE_SOFTAP_SSID_END_WITH_THE_MAC
        snprintf(softap_ssid, sizeof(softap_ssid), "%.25s_%02x%02x%02x",
                 CONFIG_BRIDGE_SOFTAP_SSID, softap_mac[3], softap_mac[4], softap_mac[5]);
    #else
        snprintf(softap_ssid, sizeof(softap_ssid), "%.32s", CONFIG_BRIDGE_SOFTAP_SSID);
    #endif
        ESP_LOGI(TAG, "SoftAP SSID (default): %s", softap_ssid);
    }

    if (esp_mesh_lite_get_softap_psw_from_nvs(softap_psw, &psw_size) == ESP_OK) {
        ESP_LOGI(TAG, "SoftAP PSW (nvs): [HIDDEN]");
    } else {
        strlcpy(softap_psw, CONFIG_BRIDGE_SOFTAP_PASSWORD, sizeof(softap_psw));
        ESP_LOGI(TAG, "SoftAP PSW (default): [HIDDEN]");
    }

    esp_mesh_lite_set_softap_info(softap_ssid, softap_psw);
}

/* ---------------- periodic system info ---------------- */
static void print_system_info_timercb(TimerHandle_t timer)
{
    (void)timer;

    uint8_t             primary = 0;
    uint8_t             sta_mac[6] = {0};
    wifi_ap_record_t    ap_info = {0};
    wifi_second_chan_t  second = 0;
    wifi_sta_list_t     wifi_sta_list = (wifi_sta_list_t){0};

    (void)esp_wifi_get_mac(ESP_IF_WIFI_STA, sta_mac);
    (void)esp_wifi_ap_get_sta_list(&wifi_sta_list);
    (void)esp_wifi_get_channel(&primary, &second);
    if (esp_mesh_lite_get_level() > 1) (void)esp_wifi_sta_get_ap_info(&ap_info);

    char line[192];
    snprintf(line, sizeof(line),
             "Ch%u Lvl%d self " MACSTR " parent " MACSTR
             " rssi %d heap %" PRIu32,
             (unsigned)primary,
             (int)esp_mesh_lite_get_level(),
             MAC2STR(sta_mac), MAC2STR(ap_info.bssid),
             (ap_info.rssi != 0 ? ap_info.rssi : -120),
             (uint32_t)esp_get_free_heap_size());

    ui_display_append(line);
    ESP_LOGI(TAG, "%s", line);
}

/* ---------------- app_main ---------------- */
void app_main(void)
{
    esp_log_level_set("*", ESP_LOG_INFO);

    ESP_ERROR_CHECK(esp_storage_init());
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());

    /* Create STA/AP netifs up front so Mesh-Lite/Wi-Fi are safe to touch */
    ESP_ERROR_CHECK(esp_bridge_create_all_netif());

    esp_mesh_lite_config_t cfg = ESP_MESH_LITE_DEFAULT_INIT();
    cfg.join_mesh_ignore_router_status = true;
#if FORCE_ROOT
    cfg.join_mesh_without_configured_wifi = false;
#else
    cfg.join_mesh_without_configured_wifi = true;
#endif

    /* Mesh-Lite init/start */
    esp_mesh_lite_init(&cfg);          // returns void in this version
#if FORCE_ROOT
    ESP_LOGI(TAG, "Configured as ROOT node");
    esp_mesh_lite_set_allowed_level(1);
#else
    ESP_LOGI(TAG, "Configured as CHILD node");
    esp_mesh_lite_set_disallowed_level(1);
#endif
    esp_mesh_lite_start();

    /* Set SoftAP SSID/PSK once Wi-Fi is alive */
    app_wifi_set_softap_info();

    /* Minimal “UI” (logs only for now) */
    ui_display_init();
    ui_display_append("Mesh root started");

    /* UART bridge for DevKit host + UDP listener for leaf messages */
    uart_bridge_init();
#if FORCE_ROOT
    (void)root_udp_start(3333);
#endif

    /* Periodic system report every 10 s */
    TimerHandle_t t = xTimerCreate("sysinfo",
                                   10000 / portTICK_PERIOD_MS,
                                   pdTRUE, NULL, print_system_info_timercb);
    xTimerStart(t, 0);
}
