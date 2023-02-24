/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "esp_rmaker_work_queue.h"

#include <nvs.h>
#include <json_parser.h>
#include <esp_partition.h>
#include <esp_https_ota.h>
#if CONFIG_BT_ENABLED
#include <esp_bt.h>
#endif /* CONFIG_BT_ENABLED */
#include "esp_ota_ops.h"

#define OTA_REBOOT_TIMER_SEC               10
#define DEF_HTTP_TX_BUFFER_SIZE            1024
#define DEF_HTTP_RX_BUFFER_SIZE            CONFIG_ESP_RMAKER_OTA_HTTP_RX_BUFFER_SIZE

#define ESP_RMAKER_NVS_PART_NAME           "nvs"
#define RMAKER_OTA_NVS_NAMESPACE           "rmaker_ota"
#define RMAKER_OTA_UPDATE_FLAG_NVS_NAME    "ota_update"
#define RMAKER_OTA_JOB_ID_NVS_NAME         "rmaker_ota_id"

#define ESP_ERR_OTA_BASE                   0x1500
#define ESP_ERR_OTA_VALIDATE_FAILED        (ESP_ERR_OTA_BASE + 0x03)  /*!< Error if OTA app image is invalid */

// Topic
#define NODE_PARAMS_REMOTE_TOPIC_SUFFIX    "params/remote"
#define OTAURL_TOPIC_SUFFIX                "otaurl"
#define MQTT_TOPIC_BUFFER_SIZE             150
#define RMAKER_MQTT_QOS0                   0
#define RMAKER_MQTT_QOS1                   1

esp_err_t esp_rmaker_mesh_lite_ota_subscribe_topic(void);
esp_err_t esp_rmaker_mesh_lite_ota_init(esp_rmaker_ota_config_t *ota_config);
esp_err_t esp_rmaker_mesh_lite_ota_cb(esp_rmaker_ota_handle_t ota_handle, esp_rmaker_ota_data_t *ota_data);
