/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdbool.h>
#include "esp_err.h"
#include "esp_log.h"
#include "esp_mesh_lite_wireless_debug.h"

#define ESP_MESH_LITE_LOG_LEVEL             ESP_LOG_DEBUG

void esp_mesh_lite_log_write(esp_log_level_t level, const char *tag, const char *format, ...);

#define ESP_MESH_LITE_LOGE(format, ... ) do { \
        if (ESP_MESH_LITE_LOG_LEVEL >= ESP_LOG_ERROR) { \
            esp_mesh_lite_log_write(ESP_LOG_ERROR, TAG, format, ##__VA_ARGS__); \
        } \
    } while(0)

#define ESP_MESH_LITE_LOGW(format, ... ) do { \
        if (ESP_MESH_LITE_LOG_LEVEL >= ESP_LOG_WARN) { \
            esp_mesh_lite_log_write(ESP_LOG_WARN, TAG, format, ##__VA_ARGS__); \
        } \
    } while(0)

#define ESP_MESH_LITE_LOGI(format, ... ) do { \
        if (ESP_MESH_LITE_LOG_LEVEL >= ESP_LOG_INFO) { \
            esp_mesh_lite_log_write(ESP_LOG_INFO, TAG, format, ##__VA_ARGS__); \
        } \
    } while(0)

#define ESP_MESH_LITE_LOGD(format, ... ) do { \
        if (ESP_MESH_LITE_LOG_LEVEL >= ESP_LOG_DEBUG) { \
            esp_mesh_lite_log_write(ESP_LOG_DEBUG, TAG, format, ##__VA_ARGS__); \
        } \
    } while(0)

#define ESP_MESH_LITE_LOGV(format, ... ) do { \
        if (ESP_MESH_LITE_LOG_LEVEL >= ESP_LOG_VERBOSE) { \
            esp_mesh_lite_log_write(ESP_LOG_VERBOSE, TAG, format, ##__VA_ARGS__); \
        } \
    } while(0)
