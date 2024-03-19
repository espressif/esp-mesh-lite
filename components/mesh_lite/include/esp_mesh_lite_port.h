/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "esp_ota_ops.h"

typedef struct {
    uint8_t ssid[32];                         /**< SSID of target AP. */
    uint8_t password[64];                     /**< Password of target AP. */
    bool bssid_set;                           /**< whether set MAC address of target AP or not. Generally, station_config.bssid_set needs to be 0; and it needs to be 1 only when users need to check the MAC address of the AP.*/
    uint8_t bssid[6];                         /**< MAC address of target AP*/
    wifi_scan_threshold_t  threshold;         /**< When scan_threshold is set, only APs which have an auth mode that is more secure than the selected auth mode and a signal stronger than the minimum RSSI will be used. */
} mesh_lite_sta_config_t;

typedef struct {
    uint8_t bssid[6];                     /**< MAC address of AP */
    uint8_t ssid[33];                     /**< SSID of AP */
    uint8_t primary;                      /**< channel of AP */
    wifi_second_chan_t second;            /**< secondary channel of AP */
    int8_t  rssi;                         /**< signal strength of AP */
} mesh_lite_ap_record_t;

/**
 * @brief Get app description.
 *
 */
const esp_app_desc_t *esp_app_get_description(void);

/**
 * @brief Get ap records when scan done.
 *
 */
wifi_ap_record_t *esp_mesh_lite_scan_get_ap_records_list(uint16_t count);
void *esp_mesh_lite_scan_get_next_ap_record(void *ap_record);

/**
 * @brief Get ap information.
 *
 */
esp_err_t esp_mesh_lite_get_ap_record(mesh_lite_ap_record_t *ap_record);

/**
 * @brief Set wifi station config.
 *
 */
esp_err_t esp_mesh_lite_set_wifi_config(mesh_lite_sta_config_t *cfg);
esp_err_t esp_mesh_lite_get_wifi_config(mesh_lite_sta_config_t *cfg);
