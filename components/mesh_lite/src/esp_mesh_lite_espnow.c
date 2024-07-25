/*
 * SPDX-FileCopyrightText: 2023-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <string.h>
#include <inttypes.h>
#include "freertos/FreeRTOS.h"
#include "esp_log.h"
#include "esp_mac.h"
#include "esp_mesh_lite.h"

static uint8_t espnow_data[ESPNOW_PAYLOAD_MAX_LEN];
static espnow_recv_failed_hook_t espnow_recv_failed_hook = NULL;
static espnow_cb_register_t *esp_mesh_lite_espnow_cb_list = NULL;

esp_err_t esp_mesh_lite_espnow_register_failed_recv_callback(espnow_recv_failed_hook_t cb)
{
    espnow_recv_failed_hook = cb;
    return ESP_OK;
}

static inline esp_err_t esp_mesh_lite_espnow_recv_callback(uint8_t type, const uint8_t *mac_addr, const uint8_t *data, int len)
{
    esp_err_t ret = ESP_FAIL;
    espnow_cb_register_t *current = esp_mesh_lite_espnow_cb_list;
    while (current != NULL) {
        if (current->type == type) {
            current->esp_mesh_lite_espnow_recv_cb(mac_addr, data, len);
            ret = ESP_OK;
            break;
        }
        current = current->next;
    }
    return ret;
}

static void espnow_recv_cb(const esp_now_recv_info_t *recv_info, const uint8_t *data, int len)
{
    uint8_t *mac_addr = recv_info->src_addr;
    esp_err_t ret = esp_mesh_lite_espnow_recv_callback(data[0], mac_addr, data + 1, len - 1);

    if (ret != ESP_OK) {
        if (espnow_recv_failed_hook) {
            espnow_recv_failed_hook(recv_info, data, len);
        }
    }
}

esp_err_t esp_mesh_lite_espnow_recv_cb_register(esp_mesh_lite_espnow_data_type_t type,
                                                void (*recv_cb)(const uint8_t *mac_addr, const uint8_t *data, int len))
{
    espnow_cb_register_t *new_espnow_cb = (espnow_cb_register_t *)malloc(sizeof(espnow_cb_register_t));
    if (!new_espnow_cb) {
        return ESP_ERR_NO_MEM;
    }

    new_espnow_cb->type = type;
    new_espnow_cb->esp_mesh_lite_espnow_recv_cb = recv_cb;
    new_espnow_cb->next = NULL;

    if (esp_mesh_lite_espnow_cb_list == NULL) {
        esp_mesh_lite_espnow_cb_list = new_espnow_cb;
    } else {
        new_espnow_cb->next = esp_mesh_lite_espnow_cb_list;
        esp_mesh_lite_espnow_cb_list = new_espnow_cb;
    }

    return ESP_OK;
}

esp_err_t esp_mesh_lite_espnow_send(uint8_t type, uint8_t *peer_addr, const uint8_t *data, size_t len)
{
    if (len == ESPNOW_PAYLOAD_MAX_LEN) {
        len--;
    }
    espnow_data[0] = type;
    memcpy(&espnow_data[1], data, len);

    esp_err_t ret = esp_now_send(peer_addr, espnow_data, len + 1);

    // Check if peer_addr is a broadcast address
    if (!IS_BROADCAST_ADDR(peer_addr)) {
        esp_now_del_peer(peer_addr);
    }

    return ret;
}

esp_err_t esp_mesh_lite_espnow_init(void)
{
    /* Initialize ESPNOW and register sending and receiving callback function. */
    ESP_ERROR_CHECK(esp_now_init());
    ESP_ERROR_CHECK(esp_now_register_recv_cb(espnow_recv_cb));

    /* Set primary master key. */
    // ESP_ERROR_CHECK( esp_now_set_pmk((uint8_t *)CONFIG_ESPNOW_PMK) );

    return ESP_OK;
}
