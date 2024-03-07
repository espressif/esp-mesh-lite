/*
 * SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <string.h>
#include <esp_log.h>
#include <esp_rmaker_standard_params.h>
#include <app_insights.h>
#include <esp_rmaker_ota.h>
#include <esp_rmaker_core.h>
#include <esp_rmaker_mqtt.h>
#include <esp_rmaker_schedule.h>
#include <esp_rmaker_common_events.h>
#include <esp_rmaker_standard_devices.h>
#include <cJSON.h>

#include "app_light.h"
#include "app_espnow.h"
#include "app_bridge.h"
#include "app_rainmaker_ota.h"
#include "esp_mesh_lite.h"

static const char *TAG = "app_rainmaker";

esp_rmaker_device_t *light_device;

extern const char ota_server_cert[] asm("_binary_server_crt_start");
extern esp_err_t __real_esp_rmaker_handle_set_params(char *data, size_t data_len, esp_rmaker_req_src_t src);

static void esp_rmaker_app_set_params_callback(const char *topic, void *payload, size_t payload_len, void *priv_data);

/* Callback to handle commands received from the RainMaker cloud */
static esp_err_t write_cb(const esp_rmaker_device_t *device, const esp_rmaker_param_t *param,
            const esp_rmaker_param_val_t val, void *priv_data, esp_rmaker_write_ctx_t *ctx)
{
    if (ctx) {
        ESP_LOGI(TAG, "Received write request via : %s", esp_rmaker_device_cb_src_to_str(ctx->src));
    }
    const char *device_name = esp_rmaker_device_get_name(device);
    const char *param_name = esp_rmaker_param_get_name(param);
    if (strcmp(param_name, ESP_RMAKER_DEF_POWER_NAME) == 0) {
        ESP_LOGI(TAG, "Received value = %s for %s - %s",
                val.val.b? "true" : "false", device_name, param_name);
        app_light_set_power(val.val.b);
    } else if (strcmp(param_name, ESP_RMAKER_DEF_BRIGHTNESS_NAME) == 0) {
        ESP_LOGI(TAG, "Received value = %d for %s - %s",
                val.val.i, device_name, param_name);
        app_light_set_brightness(val.val.i);
    } else if (strcmp(param_name, ESP_RMAKER_DEF_HUE_NAME) == 0) {
        ESP_LOGI(TAG, "Received value = %d for %s - %s",
                val.val.i, device_name, param_name);
        app_light_set_hue(val.val.i);
    } else if (strcmp(param_name, ESP_RMAKER_DEF_SATURATION_NAME) == 0) {
        ESP_LOGI(TAG, "Received value = %d for %s - %s",
                val.val.i, device_name, param_name);
        app_light_set_saturation(val.val.i);
    } else {
        /* Silently ignoring invalid params */
        return ESP_OK;
    }
    esp_rmaker_param_update_and_report(param, val);
    return ESP_OK;
}

/* Event handler for catching RainMaker events */
static void event_handler(void* arg, esp_event_base_t event_base,
                          int32_t event_id, void* event_data)
{
    if (event_base == RMAKER_EVENT) {
        switch (event_id) {
            case RMAKER_EVENT_INIT_DONE:
                ESP_LOGI(TAG, "RainMaker Initialised.");
                break;
            case RMAKER_EVENT_CLAIM_STARTED:
                ESP_LOGI(TAG, "RainMaker Claim Started.");
                break;
            case RMAKER_EVENT_CLAIM_SUCCESSFUL:
                ESP_LOGI(TAG, "RainMaker Claim Successful.");
                break;
            case RMAKER_EVENT_CLAIM_FAILED:
                ESP_LOGI(TAG, "RainMaker Claim Failed.");
                break;
            default:
                ESP_LOGW(TAG, "Unhandled RainMaker Event: %"PRId32"", event_id);
        }
    } else if (event_base == RMAKER_COMMON_EVENT) {
        switch (event_id) {
            case RMAKER_MQTT_EVENT_PUBLISHED: {
                static uint8_t resubscribe = 4;
                if (resubscribe) {
                    // "params/remote" topic resubscribe
                    char subscribe_topic[MQTT_TOPIC_BUFFER_SIZE];
                    memset(subscribe_topic, 0, sizeof(subscribe_topic));
                    snprintf(subscribe_topic, sizeof(subscribe_topic), "node/%s/%s",
                                esp_rmaker_get_node_id(), NODE_PARAMS_REMOTE_TOPIC_SUFFIX);
                    esp_err_t err = esp_rmaker_mqtt_unsubscribe(subscribe_topic);
                    if (err == ESP_OK) {
                        err = esp_rmaker_mqtt_subscribe(subscribe_topic, esp_rmaker_app_set_params_callback, RMAKER_MQTT_QOS1, NULL);
                        if (err != ESP_OK) {
                            ESP_LOGE(TAG, "Failed to subscribe to %s. Error %d", subscribe_topic, err);
                        }
                    }

                    // "otaurl" topic resubscribe
                    esp_rmaker_mesh_lite_ota_subscribe_topic();

                    resubscribe--;
                    ESP_LOGI(TAG, "MQTT Resubscribe [%s]", NODE_PARAMS_REMOTE_TOPIC_SUFFIX);
                }
                break;
            }
            case RMAKER_EVENT_REBOOT:
                ESP_LOGI(TAG, "Rebooting in %d seconds.", *((uint8_t *)event_data));
                break;
            case RMAKER_EVENT_WIFI_RESET:
                ESP_LOGI(TAG, "Wi-Fi credentials reset.");
                break;
            case RMAKER_EVENT_FACTORY_RESET:
                ESP_LOGI(TAG, "Node reset to factory defaults.");
                esp_mesh_lite_erase_rtc_store();
                break;
            default:
                ESP_LOGW(TAG, "Unhandled RainMaker Common Event: %"PRId32"", event_id);
        }
    } else {
        ESP_LOGW(TAG, "Invalid event received!");
    }
}

#define ESP_RMAKER_UI_IDENTIFY_SERV_NAME "ui-identify"
#define ESP_RMAKER_PARAM_IS_NOVA "is_nova_home"

static esp_err_t esp_rmaker_app_utils_service_cb(const esp_rmaker_device_t* device, const esp_rmaker_param_t* param,
                                                 const esp_rmaker_param_val_t val, void* priv_data, esp_rmaker_write_ctx_t* ctx)
{
    esp_err_t err = ESP_FAIL;
    if (strcmp(esp_rmaker_param_get_type(param), ESP_RMAKER_PARAM_IS_NOVA) == 0) {
        // Nothing here
    }
    esp_rmaker_param_update_and_report(param, val);
    return err;
}

esp_err_t app_utils_rmaker_ui_identify_service_enable(void)
{
    esp_rmaker_device_t* service = esp_rmaker_service_create(ESP_RMAKER_UI_IDENTIFY_SERV_NAME, "esp.service.ui-identify", NULL);
    if (service) {
        esp_rmaker_param_t* param = esp_rmaker_param_create(ESP_RMAKER_PARAM_IS_NOVA, "esp.param.is_nova_home", esp_rmaker_bool(true), PROP_FLAG_READ);
        esp_rmaker_device_add_param(service, param);
    }
    esp_rmaker_device_add_cb(service, esp_rmaker_app_utils_service_cb, NULL);
    esp_err_t err = esp_rmaker_node_add_device(esp_rmaker_get_node(), service);
    if (err == ESP_OK) {
        ESP_LOGI(TAG, "ui identify service enabled");
    } else {
        esp_rmaker_device_delete(service);
    }
    return err;
}

void esp_rmaker_control_light_by_user(char* data)
{
    __real_esp_rmaker_handle_set_params((char *)data, strlen(data), ESP_RMAKER_REQ_SRC_CLOUD);
}

char group_control_payload[ESPNOW_PAYLOAD_MAX_LEN];
esp_err_t __wrap_esp_rmaker_handle_set_params(char *data, size_t data_len, esp_rmaker_req_src_t src)
{
    ESP_LOGI(TAG, "Received params: %.*s", data_len, data);

    bool control = true;

    if (esp_mesh_lite_get_level() == 1) {
        cJSON *rmaker_data_js = cJSON_Parse((const char*)data);
        cJSON *light_js = cJSON_GetObjectItem(rmaker_data_js, "Light");
        if (light_js) {
            cJSON *group_id_js = cJSON_GetObjectItem(light_js, "group_id");
            if (group_id_js) {
                char* payload_string = cJSON_PrintUnformatted(rmaker_data_js);
                memset(group_control_payload, 0x0, ESPNOW_PAYLOAD_MAX_LEN);
                memcpy(group_control_payload, payload_string, ESPNOW_PAYLOAD_MAX_LEN);
                free(payload_string);
                payload_string = NULL;

                esp_now_send_group_control((uint8_t*)group_control_payload, false);

                if (group_id_js->valueint && (esp_rmaker_is_my_group_id(group_id_js->valueint) == false)) {
                    control = false;
                }
            }
        }
        cJSON_Delete(rmaker_data_js);
    }

    if (control) {
        __real_esp_rmaker_handle_set_params((char *)data, data_len, src);
    }

    return ESP_OK;
}

static void esp_rmaker_app_set_params_callback(const char *topic, void *payload, size_t payload_len, void *priv_data)
{
    esp_rmaker_handle_set_params((char *)payload, payload_len, ESP_RMAKER_REQ_SRC_CLOUD);
}

void app_rainmaker_start(void)
{
    /* Initialize Application specific hardware drivers and
     * set initial state.
     */
    app_driver_init();

    /* Register an event handler to catch RainMaker events */
    ESP_ERROR_CHECK(esp_event_handler_register(RMAKER_EVENT, ESP_EVENT_ANY_ID, &event_handler, NULL));
    ESP_ERROR_CHECK(esp_event_handler_register(RMAKER_COMMON_EVENT, ESP_EVENT_ANY_ID, &event_handler, NULL));

    /* Initialize the ESP RainMaker Agent.
     * Note that this should be called after app_wifi_init() but before app_wifi_start()
     * */
    esp_rmaker_config_t rainmaker_cfg = {
        .enable_time_sync = false,
    };
    esp_rmaker_node_t *node = esp_rmaker_node_init(&rainmaker_cfg, "ESP RainMaker Device", "Lightbulb");
    if (!node) {
        ESP_LOGE(TAG, "Could not initialise node. Aborting!!!");
        vTaskDelay(5000/portTICK_PERIOD_MS);
        abort();
    }
    esp_rmaker_node_add_attribute(node, "LiteMeshDevice", "1");

    /* Create a device and add the relevant parameters to it */
    light_device = esp_rmaker_lightbulb_device_create("Light", NULL, DEFAULT_POWER);
    esp_rmaker_device_add_cb(light_device, write_cb, NULL);

    esp_rmaker_device_add_param(light_device, esp_rmaker_brightness_param_create(ESP_RMAKER_DEF_BRIGHTNESS_NAME, DEFAULT_BRIGHTNESS));
    esp_rmaker_device_add_param(light_device, esp_rmaker_hue_param_create(ESP_RMAKER_DEF_HUE_NAME, DEFAULT_HUE));
    esp_rmaker_device_add_param(light_device, esp_rmaker_saturation_param_create(ESP_RMAKER_DEF_SATURATION_NAME, DEFAULT_SATURATION));

    esp_rmaker_node_add_device(node, light_device);

    esp_rmaker_system_serv_config_t serv_config = {
        .flags = SYSTEM_SERV_FLAGS_ALL,
        .reset_reboot_seconds = 2,
    };

    esp_rmaker_system_service_enable(&serv_config);

    /* Enable OTA */
    esp_rmaker_ota_config_t ota_config = {
        .server_cert = ota_server_cert,
        .ota_cb      = esp_rmaker_mesh_lite_ota_cb,
    };
    esp_rmaker_ota_enable(&ota_config, OTA_USING_TOPICS);

    esp_rmaker_mesh_lite_ota_init(&ota_config);

    /* Enable timezone service which will be require for setting appropriate timezone
     * from the phone apps for scheduling to work correctly.
     * For more information on the various ways of setting timezone, please check
     * https://rainmaker.espressif.com/docs/time-service.html.
     */
    esp_rmaker_timezone_service_enable();

    /* Enable scheduling. */
    esp_rmaker_schedule_enable();

    /* Enable Insights. Requires CONFIG_ESP_INSIGHTS_ENABLED=y */
    app_insights_enable();

    /* Start the ESP RainMaker Agent */
    esp_rmaker_start();

    app_utils_rmaker_ui_identify_service_enable();
}
