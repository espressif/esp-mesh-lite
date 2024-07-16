/*
 * SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once
#include <stdint.h>
#include <esp_err.h>

#include "esp_netif.h"
#include "esp_mesh_lite.h"

typedef enum {
    ESP_MESH_LITE_EVENT_CHILD_NODE_JOIN = ESP_MESH_LITE_EVENT_MAX,
    ESP_MESH_LITE_EVENT_CHILD_NODE_LEAVE,
} mesh_lite_event_child_node_info_t;

typedef struct app_node_info {
    uint8_t level;
    char ip[IP_MAX_LEN];
    char mac[MAC_MAX_LEN];
} app_node_info_t;

/* Enable ESP IoT Bridge in the application
 *
 * @return ESP_OK on success.
 * @return error in case of failure.
 */
esp_err_t app_rmaker_enable_bridge(void);

esp_err_t app_rmaker_mesh_lite_service_create(void);

esp_err_t app_rmaker_mesh_lite_report_child_info(void);

void app_rmaker_mesh_lite_report_info_to_parent(void);

void app_rmaker_mesh_lite_level_update_and_report(uint8_t level);

void app_rmaker_mesh_lite_self_ip_update_and_report(esp_netif_ip_info_t* ap_ip_info, esp_netif_ip_info_t* sta_ip_info);

bool esp_rmaker_is_my_group_id(uint8_t group_id);
