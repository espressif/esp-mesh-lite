/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <string.h>
#include "esp_mac.h"
#include "esp_log.h"
#include "esp_wifi.h"
#include "esp_mesh_lite_port.h"

wifi_ap_record_t *esp_mesh_lite_scan_get_ap_records_list(uint16_t count)
{
    wifi_ap_record_t *ap_list = (wifi_ap_record_t *)malloc(sizeof(wifi_ap_record_t) * count);
    if (ap_list) {
        esp_wifi_scan_get_ap_records(&count, ap_list);
    }
    return ap_list;
}

void *esp_mesh_lite_scan_get_next_ap_record(void *ap_record)
{
    if (!ap_record) {
        return NULL;
    } else {
        return ((wifi_ap_record_t *)ap_record) + 1;
    }
}

esp_err_t esp_mesh_lite_get_ap_record(mesh_lite_ap_record_t *ap_record)
{
    if (!ap_record) {
        return ESP_FAIL;
    }

    wifi_ap_record_t ap_info;
    if (esp_wifi_sta_get_ap_info(&ap_info) != ESP_OK) {
        return ESP_FAIL;
    }
    memcpy((char*)ap_record->bssid, (char*)ap_info.bssid, sizeof(ap_record->bssid));
    memcpy((char*)ap_record->ssid, (char*)ap_info.ssid, sizeof(ap_record->ssid));
    ap_record->primary = ap_info.primary;
    ap_record->second = ap_info.second;
    ap_record->rssi = ap_info.rssi;

    return ESP_OK;
}

esp_err_t esp_mesh_lite_set_wifi_config(mesh_lite_sta_config_t *cfg)
{
    if (!cfg) {
        return ESP_FAIL;
    }

    wifi_config_t wifi_cfg;
    memset(&wifi_cfg, 0x0, sizeof(wifi_config_t));

    memcpy((char *)wifi_cfg.sta.ssid, (char *)cfg->ssid, sizeof(wifi_cfg.sta.ssid));
    strlcpy((char *)wifi_cfg.sta.password, (char *)cfg->password, sizeof(wifi_cfg.sta.password));

    if (cfg->bssid_set) {
        wifi_cfg.sta.bssid_set = 1;
        memcpy((char *)wifi_cfg.sta.bssid, (char *)cfg->bssid, sizeof(wifi_cfg.sta.bssid));
    }

    wifi_cfg.sta.threshold.rssi = cfg->threshold.rssi;
    wifi_cfg.sta.threshold.authmode = cfg->threshold.authmode;

    if (esp_wifi_set_config(WIFI_IF_STA, &wifi_cfg) != ESP_OK) {
        return ESP_FAIL;
    }
    return ESP_OK;
}

esp_err_t esp_mesh_lite_get_wifi_config(mesh_lite_sta_config_t *cfg)
{
    if (!cfg) {
        return ESP_FAIL;
    }

    wifi_config_t wifi_cfg;
    if (esp_wifi_get_config(WIFI_IF_STA, &wifi_cfg) != ESP_OK) {
        return ESP_FAIL;
    }

    memcpy((char *)cfg->ssid, (char *)wifi_cfg.sta.ssid, sizeof(cfg->ssid));
    strlcpy((char *)cfg->password, (char *)wifi_cfg.sta.password, sizeof(cfg->password));

    cfg->bssid_set = wifi_cfg.sta.bssid_set;
    if (cfg->bssid_set) {
        memcpy((char *)cfg->bssid, (char *)wifi_cfg.sta.bssid, sizeof(cfg->bssid));
    } else {
        memset((char *)cfg->bssid, 0x0, sizeof(cfg->bssid));
    }

    cfg->threshold.rssi = wifi_cfg.sta.threshold.rssi;
    cfg->threshold.authmode = wifi_cfg.sta.threshold.authmode;

    return ESP_OK;
}

esp_err_t esp_mesh_lite_get_ap_config(mesh_lite_ap_config_t *cfg)
{
    if (!cfg) {
        return ESP_FAIL;
    }

    wifi_config_t wifi_cfg;
    if (esp_wifi_get_config(WIFI_IF_AP, &wifi_cfg) != ESP_OK) {
        return ESP_FAIL;
    }

    memcpy((char *)cfg->ssid, (char *)wifi_cfg.ap.ssid, sizeof(cfg->ssid));
    strlcpy((char *)cfg->password, (char *)wifi_cfg.ap.password, sizeof(cfg->password));

    return ESP_OK;
}
