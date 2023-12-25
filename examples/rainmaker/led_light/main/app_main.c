/*
 * SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "inttypes.h"
#include "nvs.h"
#include "nvs_flash.h"
#include "esp_log.h"
#include "app_wifi.h"
#include "app_bridge.h"
#include "app_espnow.h"
#include "app_rainmaker.h"

static const char *TAG = "app_main";

static esp_err_t esp_storage_init(void)
{
    esp_err_t ret = nvs_flash_init();

    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        // NVS partition was truncated and needs to be erased
        // Retry nvs_flash_init
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }

    return ret;
}

void session_cost_information(const char *tag, const char *func, int line, const char *desc)
{
    static uint32_t free_heap = 0;

    ESP_LOGW(tag, "%s %d %s const heap %"PRIu32"", func, line, desc ? desc : "NULL", esp_get_free_heap_size() - free_heap);
    free_heap = esp_get_free_heap_size();
    ESP_LOGW(tag, "free heap %"PRIu32", minimum %"PRIu32"", free_heap, esp_get_minimum_free_heap_size());
}

void app_main(void)
{
    session_cost_information(TAG, __func__, __LINE__, "app_main");

    esp_storage_init();

    app_rmaker_enable_bridge();

    app_rainmaker_start();

    group_control_init();

    /* Start wifi provisioning */
    app_wifi_start(POP_TYPE_MAC);

    app_rmaker_mesh_lite_service_create();
}
