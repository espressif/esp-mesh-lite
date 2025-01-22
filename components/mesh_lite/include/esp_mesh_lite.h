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

#if CONFIG_MESH_LITE_NODE_INFO_REPORT
#define MESH_LITE_REPORT_INTERVAL_BUFFER            10

#ifndef ETH_HWADDR_LEN
#define ETH_HWADDR_LEN       6
#endif

typedef struct  esp_mesh_lite_node_info {
    uint8_t level;
    uint32_t ip_addr;
    uint8_t mac_addr[ETH_HWADDR_LEN];
} esp_mesh_lite_node_info_t;

typedef struct node_info_list {
    struct node_info_list* next;
    esp_mesh_lite_node_info_t* node;
    uint32_t ttl;
} node_info_list_t;

typedef enum {
    MESH_LITE_MSG_ID_INVALID = 0,
    MESH_LITE_MSG_ID_REPORT_NODE_INFO,
    MESH_LITE_MSG_ID_REPORT_NODE_INFO_RESP,
    MESH_LITE_MSG_ID_UPDATE_NODES_LIST,
} esp_mesh_lite_msg_id_t;

/**
 * @brief Child nodes report MAC and level information to the root node.
 *
 * This function is used by child nodes in the mesh network to report their MAC address
 * and network level information to the root node.
 *
 * @return
 *      - ESP_OK: Successfully reported the information.
 *      - ESP_FAIL: Failed to report the information.
 *      - Other error codes: As defined in esp_err_t.
 */
esp_err_t esp_mesh_lite_report_info(void);

/**
 * @brief Get the list of nodes in the mesh network.
 *
 * This function retrieves the list of nodes currently connected in the mesh network.
 * The list includes details such as the MAC address and network level of each node.
 *
 * @param[out] size Pointer to a variable where the number of nodes in the list will be stored.
 *                  This value will be set to the number of nodes in the returned list.
 *
 * @return
 *      - Pointer to the list of node information structures (node_info_list_t*).
 *      - NULL: If the list could not be retrieved or is empty.
 *
 * @note The returned pointer should not be modified or freed by the caller.
 */
const node_info_list_t *esp_mesh_lite_get_nodes_list(uint32_t *size);

#endif /* CONFIG_MESH_LITE_NODE_INFO_REPORT */

/**
 * @brief Get all node number of Mesh-Lite
 *
 * @attention The valid number of all nodes can only be obtained when CONFIG_MESH_LITE_NODE_INFO_REPORT is configured, otherwise 0 is returned.
 *
 * @return the all node number of Mesh-Lite
 */
uint32_t esp_mesh_lite_get_mesh_node_number(void);

/**
 * @brief Get all node number of Mesh-Lite
 *
 * @attention The valid number of all nodes can only be obtained when CONFIG_MESH_LITE_NODE_INFO_REPORT is configured, otherwise 0 is returned.
 *
 * @note This API is present for backward compatibility reasons. Alternative function
 * with the same functionality is `esp_mesh_lite_get_mesh_node_number`
 *
 * @return the all node number of Mesh-Lite
 */
uint32_t esp_mesh_lite_get_child_node_number(void) __attribute__((deprecated("Please use esp_mesh_lite_get_mesh_node_number instead")));

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
