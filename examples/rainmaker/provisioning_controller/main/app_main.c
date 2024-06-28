/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "nvs.h"
#include "nvs_flash.h"
#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "esp_log.h"
#include "esp_wifi.h"
#include "esp_netif.h"
#include "app_espnow.h"
#include "iot_button.h"

#define BUTTON_NUM            1
#define BUTTON_SW1            CONFIG_APP_GPIO_BUTTON_SW1

static TimerHandle_t trigger_timer;
static button_handle_t g_btns[BUTTON_NUM] = { 0 };

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

static void button_press_up_cb(void *hardware_data, void *usr_data)
{
    if (xTimerIsTimerActive(trigger_timer) == pdFALSE) {
        // xTimer is not active
        app_espnow_reset_group_control();
        xTimerStart(trigger_timer, 0);
    }
}

static void trigger_timer_callback(TimerHandle_t timer)
{
    app_espnow_trigger_distribution_network();
    xTimerStop(trigger_timer, 0);
}

void app_button_init(void)
{
    trigger_timer = xTimerCreate("trigger_timer", pdMS_TO_TICKS(1000), pdTRUE, NULL, trigger_timer_callback);

    button_config_t button_cfg = {
        .type = BUTTON_TYPE_GPIO,
        .gpio_button_config = {
            .gpio_num = BUTTON_SW1,
            .active_level = 0,
        },
    };
    g_btns[0] = iot_button_create(&button_cfg);
    iot_button_register_cb(g_btns[0], BUTTON_PRESS_UP, button_press_up_cb, 0);
}

void app_wifi_init(void)
{
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    wifi_init_config_t wifi_cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&wifi_cfg));
    ESP_ERROR_CHECK(esp_wifi_set_storage(WIFI_STORAGE_RAM));
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_start());
    ESP_ERROR_CHECK(esp_wifi_set_channel(CONFIG_ESPNOW_CHANNEL, WIFI_SECOND_CHAN_NONE));
}

void app_main(void)
{
    esp_storage_init();

    app_wifi_init();

    app_espnow_init();

    app_button_init();
}
