/*
 * SPDX-FileCopyrightText: 2023-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdio.h>
#include <stdlib.h>
#include "esp_now.h"

#define ESPNOW_MAXDELAY                  (512)
#if defined(ESP_NOW_MAX_DATA_LEN_V2)
#define ESPNOW_PAYLOAD_MAX_LEN           (1470)
#else
#define ESPNOW_PAYLOAD_MAX_LEN           (250)
#endif

#define IS_BROADCAST_ADDR(addr) ((((uint8_t*)addr)[0] == 0xFF) && (((uint8_t*)addr)[1] == 0xFF) && (((uint8_t*)addr)[2] == 0xFF) \
                                    && (((uint8_t*)addr)[3] == 0xFF) && (((uint8_t*)addr)[4] == 0xFF) && (((uint8_t*)addr)[5] == 0xFF))

typedef void (*esp_mesh_lite_espnow_handler_failed_hook_t)(const esp_now_recv_info_t *recv_info, const uint8_t *data, int len);
typedef esp_err_t (*esp_mesh_lite_espnow_recv_cb_t)(const esp_now_recv_info_t *recv_info, const uint8_t *data, int len);

typedef enum {
    ESPNOW_DATA_TYPE_MESH_LITE_CORE,
    ESPNOW_DATA_TYPE_RM_GROUP_CONTROL,
    ESPNOW_DATA_TYPE_ZERO_PROV,
    ESPNOW_DATA_TYPE_WIRELESS_DEBUG,
    ESPNOW_DATA_TYPE_WIRELESS_LOG,
    ESPNOW_DATA_TYPE_RESERVE = 200,
} esp_mesh_lite_espnow_data_type_t;

typedef struct espnow_cb_register {
    esp_mesh_lite_espnow_data_type_t type;
    esp_mesh_lite_espnow_recv_cb_t recv_cb;
    struct espnow_cb_register *next;
} espnow_cb_register_t;

typedef struct {
    uint8_t mac_addr[ESP_NOW_ETH_ALEN];
    esp_now_send_status_t status;
} espnow_send_cb_t;

typedef struct {
    uint8_t mac_addr[ESP_NOW_ETH_ALEN];
    uint8_t type;
    uint8_t *data;
    int data_len;
} espnow_recv_cb_t;

typedef enum {
    ESPNOW_SEND_CB,
    ESPNOW_RECV_CB,
} esp_mesh_lite_espnow_event_id_t;

typedef union {
    espnow_send_cb_t send_cb;
    espnow_recv_cb_t recv_cb;
} esp_mesh_lite_espnow_event_info_t;

/* When ESPNOW sending or receiving callback function is called, post event to ESPNOW task. */
typedef struct {
    esp_mesh_lite_espnow_event_id_t id;
    esp_mesh_lite_espnow_event_info_t info;
} esp_mesh_lite_espnow_event_t;

/**
 * @brief Initialize ESP-Mesh-Lite ESP-NOW module.
 *
 * This function initializes the ESP-Mesh-Lite ESP-NOW module, enabling communication
 * between Mesh Lite nodes using ESP-NOW protocol.
 *
 * @return
 *      - ESP_OK: Initialization successful
 *      - ESP_FAIL: Failed to initialize ESP-NOW module
 */
esp_err_t esp_mesh_lite_espnow_init(void);

/**
 * @brief Send data using ESP-Mesh-Lite ESP-NOW.
 *
 * This function sends data of the specified type to a peer node with the given MAC address
 * using ESP-Mesh-Lite ESP-NOW protocol.
 *
 * @param[in] type Type of data being sent.
 * @param[in] peer_addr MAC address of the peer node.
 * @param[in] data Pointer to the data to be sent.
 * @param[in] len Length of the data.
 * @return
 *      - ESP_OK: Data sent successfully
 *      - ESP_FAIL: Failed to send data
 */
esp_err_t esp_mesh_lite_espnow_send(uint8_t type, uint8_t *peer_addr, const uint8_t *data, size_t len);

/**
 * @brief Send data and delete the peer using ESP-Mesh-Lite ESP-NOW.
 *
 * This function sends data of the specified type to a peer node with the given MAC address
 * using the ESP-Mesh-Lite ESP-NOW protocol, and then deletes the peer from the ESP-NOW peer list.
 *
 * @param[in] type Type of data being sent.
 * @param[in] peer_addr MAC address of the peer node.
 * @param[in] data Pointer to the data to be sent.
 * @param[in] len Length of the data.
 * @return
 *      - ESP_OK: Data sent and peer deleted successfully
 *      - ESP_FAIL: Failed to send data or delete the peer
 */
esp_err_t esp_mesh_lite_espnow_send_and_del_peer(uint8_t type, uint8_t *peer_addr, const uint8_t *data, size_t len);

/**
 * @brief Register a callback function for handling ESP-Mesh-Lite ESP-NOW data reception.
 *
 * This function registers a callback function to handle the reception of ESP-NOW data
 * with the specified data type. When data of the specified type is received, the
 * registered callback function will be invoked.
 *
 * @param[in] type Type of data for which the callback is registered.
 * @param[in] recv_cb Pointer to the callback function for data reception.
 *
 * @return
 *      - ESP_OK: Callback registration successful
 *      - ESP_FAIL: Failed to register the callback
 */
esp_err_t esp_mesh_lite_espnow_recv_cb_register(esp_mesh_lite_espnow_data_type_t type, esp_mesh_lite_espnow_recv_cb_t recv_cb);

/**
 * @brief Unregister a callback function for handling ESP-Mesh-Lite ESP-NOW data reception.
 *
 * This function removes a previously registered callback function for the specified data type.
 * If a callback function was registered for the given data type using `esp_mesh_lite_espnow_recv_cb_register`,
 * this function will unregister it, and the callback will no longer be invoked when data of
 * that type is received.
 *
 * @param[in] type Type of data for which the callback is unregistered.
 *
 * @return
 *      - ESP_OK: Callback unregistration successful
 *      - ESP_ERR_NOT_FOUND: No callback found for the specified type
 */
esp_err_t esp_mesh_lite_espnow_recv_cb_unregister(esp_mesh_lite_espnow_data_type_t type);

/**
 * @brief Register ESP-Mesh-Lite ESP-NOW failed handler callback function
 *
 * @param[in] cb  The callback function to be registered.
 *
 * @return
 *      - ESP_OK: Callback registration successful
 *      - ESP_FAIL: Failed to register the callback
 */
esp_err_t esp_mesh_lite_espnow_register_handler_failed_callback(esp_mesh_lite_espnow_handler_failed_hook_t cb);
