/*
 * SPDX-FileCopyrightText: 2023-2024 Espressif Systems (Shanghai) CO LTD
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
    uint8_t failure_retry_cnt;                /**< Number of connection retries station will do before moving to next AP. scan_method should be set as WIFI_ALL_CHANNEL_SCAN to use this config.
                                                   Note: Enabling this may cause connection time to increase incase best AP doesn't behave properly. */
} mesh_lite_sta_config_t;

typedef struct {
    uint8_t ssid[32];           /**< SSID of ESP32 soft-AP. If ssid_len field is 0, this must be a Null terminated string. Otherwise, length is set according to ssid_len. */
    uint8_t password[64];       /**< Password of ESP32 soft-AP. */
    uint8_t ssid_len;           /**< Optional length of SSID field. */
    uint8_t channel;            /**< Channel of soft-AP */
    wifi_auth_mode_t authmode;  /**< Auth mode of soft-AP. Do not support AUTH_WEP, AUTH_WAPI_PSK and AUTH_OWE in soft-AP mode. When the auth mode is set to WPA2_PSK, WPA2_WPA3_PSK or WPA3_PSK, the pairwise cipher will be overwritten with WIFI_CIPHER_TYPE_CCMP.  */
    uint8_t ssid_hidden;        /**< Broadcast SSID or not, default 0, broadcast the SSID */
    uint8_t max_connection;     /**< Max number of stations allowed to connect in */
    uint16_t beacon_interval;   /**< Beacon interval which should be multiples of 100. Unit: TU(time unit, 1 TU = 1024 us). Range: 100 ~ 60000. Default value: 100 */
} mesh_lite_ap_config_t;

typedef struct {
    uint8_t bssid[6];                     /**< MAC address of AP */
    uint8_t ssid[33];                     /**< SSID of AP */
    uint8_t primary;                      /**< channel of AP */
    wifi_second_chan_t second;            /**< secondary channel of AP */
    int8_t  rssi;                         /**< signal strength of AP */
} mesh_lite_ap_record_t;

/**
 * @brief Get access point records when scan is done.
 *
 * This function retrieves the list of access point records after a Wi-Fi scan
 * has been completed.
 *
 * @param count Number of access point records to retrieve.
 * @return Pointer to the list of access point records (wifi_ap_record_t).
 */
wifi_ap_record_t *esp_mesh_lite_scan_get_ap_records_list(uint16_t count);

/**
 * @brief Get the next access point record.
 *
 * This function retrieves the next access point record from the given list.
 *
 * @param ap_record Pointer to the current access point record.
 * @return Pointer to the next access point record.
 */
void *esp_mesh_lite_scan_get_next_ap_record(void *ap_record);

/**
 * @brief Get access point information.
 *
 * This function retrieves information about a specific access point.
 *
 * @param ap_record Pointer to the structure to hold the access point information (mesh_lite_ap_record_t).
 * @return ESP_OK on success, or an appropriate error code on failure.
 */
esp_err_t esp_mesh_lite_get_ap_record(mesh_lite_ap_record_t *ap_record);

/**
 * @brief Set Wi-Fi station configuration.
 *
 * This function sets the configuration for the Wi-Fi station, including SSID,
 * password, and other connection parameters.
 *
 * @param cfg Pointer to the Wi-Fi station configuration structure (mesh_lite_sta_config_t).
 * @return ESP_OK on success, or an appropriate error code on failure.
 */
esp_err_t esp_mesh_lite_set_wifi_config(mesh_lite_sta_config_t *cfg);

/**
 * @brief Get Wi-Fi station configuration.
 *
 * This function retrieves the current configuration of the Wi-Fi station.
 *
 * @param cfg Pointer to the structure to hold the Wi-Fi station configuration (mesh_lite_sta_config_t).
 * @return ESP_OK on success, or an appropriate error code on failure.
 */
esp_err_t esp_mesh_lite_get_wifi_config(mesh_lite_sta_config_t *cfg);

/**
 * @brief Get access point configuration.
 *
 * This function retrieves the configuration of the access point, including SSID,
 * password, channel, and other parameters.
 *
 * @param cfg Pointer to the structure to hold the access point configuration (mesh_lite_ap_config_t).
 * @return ESP_OK on success, or an appropriate error code on failure.
 */
esp_err_t esp_mesh_lite_get_ap_config(mesh_lite_ap_config_t *cfg);
