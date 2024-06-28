/*
 * SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#ifdef __cplusplus
extern "C"
{
#endif

#include "esp_mesh_lite_log.h"
#include "esp_mesh_lite_core.h"
#include "esp_mesh_lite_espnow.h"

#define ROOT    (1)

typedef enum {
    ESP_MESH_LITE_EVENT_NODE_JOIN = ESP_MESH_LITE_EVENT_OTA_MAX,
    ESP_MESH_LITE_EVENT_NODE_LEAVE,
    ESP_MESH_LITE_EVENT_NODE_CHANGE,
    ESP_MESH_LITE_EVENT_MAX,
} esp_mesh_lite_event_node_info_t;

/**
 * @brief Initialization Mesh-Lite.
 *
 */
void esp_mesh_lite_init(esp_mesh_lite_config_t* config);

/**
 * @brief Start Mesh-Lite.
 *
 */
void esp_mesh_lite_start(void);

typedef struct  esp_mesh_lite_node_info {
    uint8_t level;
    char mac[MAC_MAX_LEN];
    char ip[IP_MAX_LEN];
} esp_mesh_lite_node_info_t;

#if CONFIG_MESH_LITE_NODE_INFO_REPORT
/**
 * @brief child nodes report mac and level information to the root node.
 *
 */
esp_err_t esp_mesh_lite_report_info(void);
#endif /* CONFIG_MESH_LITE_NODE_INFO_REPORT */

/**
 * @brief Get the child node number of Mesh-Lite
 *
 * @attention The valid number of child nodes can only be obtained when CONFIG_MESH_LITE_NODE_INFO_REPORT is configured, otherwise 0 is returned.
 *
 * @return the child node number of Mesh-Lite
 */
uint8_t esp_mesh_lite_get_child_node_number(void);

/**
 * @brief Get the softap ssid from NVS
 *
 * @param[out]    softap_ssid: Pointer to the softap ssid.
 *
 * @param[inout]  size         A non-zero pointer to the variable holding the length of softap_ssid.
 *                             In case softap_ssid a zero, will be set to the length
 *                             required to hold the value. In case softap_ssid is not
 *                             zero, will be set to the actual length of the value
 *                             written. For nvs_get_str this includes zero terminator.
 *
 * @return
 *             - ESP_OK if the value was retrieved successfully
 *             - ESP_FAIL if there is an internal error; most likely due to corrupted
 */
esp_err_t esp_mesh_lite_get_softap_ssid_from_nvs(char* softap_ssid, size_t* size);

/**
 * @brief Get the softap password from NVS
 *
 * @param[out]    softap_psw: Pointer to the softap password.
 *
 * @param[inout]  size        A non-zero pointer to the variable holding the length of softap_psw.
 *                            In case softap_psw a zero, will be set to the length
 *                            required to hold the value. In case softap_psw is not
 *                            zero, will be set to the actual length of the value
 *                            written. For nvs_get_str this includes zero terminator.
 *
 * @return
 *             - ESP_OK if the value was retrieved successfully
 *             - ESP_FAIL if there is an internal error; most likely due to corrupted
 */
esp_err_t esp_mesh_lite_get_softap_psw_from_nvs(char* softap_psw, size_t* size);

/**
 * @brief Set the softap ssid to NVS
 *
 * @param[in]   softap_ssid: Pointer to the softap ssid buffer.
 *
 * @return
 *             - ESP_OK if the value was retrieved successfully
 *             - ESP_FAIL if there is an internal error; most likely due to corrupted
 */
esp_err_t esp_mesh_lite_set_softap_ssid_to_nvs(char* softap_ssid);

/**
 * @brief Set the softap password from NVS
 *
 * @param[in]   softap_psw: Pointer to the softap password buffer.
 *
 * @return
 *             - ESP_OK if the value was retrieved successfully
 *             - ESP_FAIL if there is an internal error; most likely due to corrupted
 */
esp_err_t esp_mesh_lite_set_softap_psw_to_nvs(char* softap_psw);

#ifdef __cplusplus
}
#endif
