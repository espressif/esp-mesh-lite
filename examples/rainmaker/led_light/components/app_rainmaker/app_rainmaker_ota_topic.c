/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "string.h"
#include "esp_log.h"
#include "esp_rmaker_ota.h"
#include "app_rainmaker_ota.h"
#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"

typedef struct {
    esp_rmaker_ota_type_t type;
    esp_rmaker_ota_cb_t ota_cb;
    void *priv;
    esp_rmaker_post_ota_diag_t ota_diag;
    TimerHandle_t rollback_timer;
    const char *server_cert;
    char *url;
    char *fw_version;
    int filesize;
    bool ota_in_progress;
    bool validation_in_progress;
    bool rolled_back;
    ota_status_t last_reported_status;
    void *transient_priv;
    char *metadata;
} esp_rmaker_ota_t;

static const char *TAG = "app_rainmaker_ota_topic";

extern void esp_rmaker_ota_common_cb(void *priv);
static esp_rmaker_ota_t *ota_info = NULL;
static esp_rmaker_ota_t *esp_rmaker_ota = NULL;

static esp_err_t esp_mesh_lite_ota_from_extern_url(void)
{
    if (esp_rmaker_work_queue_add_task(esp_rmaker_ota_common_cb, ota_info) != ESP_OK) {
        esp_rmaker_ota_finish_using_topics(ota_info);
        return ESP_OK;
    } else {
        return ESP_FAIL;
    }
}

static void esp_rmaker_mesh_lite_ota_url_handler(const char *topic, void *payload, size_t payload_len, void *priv_data)
{
    if (!priv_data) {
        return;
    }
    esp_rmaker_ota_handle_t ota_handle = priv_data;
    esp_rmaker_ota_t *ota = (esp_rmaker_ota_t *)ota_handle;
    if (ota->ota_in_progress) {
        ESP_LOGE(TAG, "OTA already in progress. Please try later.");
        return;
    }
    ota->ota_in_progress = true;
    /* Starting Firmware Upgrades */
    ESP_LOGI(TAG, "Upgrade Handler got:%.*s on %s topic\n", (int) payload_len, (char *)payload, topic);
    /*
       {
       "ota_job_id": "<ota_job_id>",
       "url": "<fw_url>",
       "fw_version": "<fw_version>",
       "filesize": <size_in_bytes>
       }
    */
    jparse_ctx_t jctx;
    char *url = NULL, *ota_job_id = NULL, *fw_version = NULL;
    int ret = json_parse_start(&jctx, (char *)payload, (int) payload_len);
    if (ret != 0) {
        ESP_LOGE(TAG, "Invalid JSON received: %s", (char *)payload);
        esp_rmaker_ota_report_status(ota_handle, OTA_STATUS_FAILED, "Aborted. JSON Payload error");
        ota->ota_in_progress = false;
        return;
    }
    int len = 0;
    ret = json_obj_get_strlen(&jctx, "ota_job_id", &len);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Aborted. OTA Job ID not found in JSON");
        esp_rmaker_ota_report_status(ota_handle, OTA_STATUS_FAILED, "Aborted. OTA Updated ID not found in JSON");
        goto end;
    }
    len++; /* Increment for NULL character */
    ota_job_id = calloc(1, len);
    if (!ota_job_id) {
        ESP_LOGE(TAG, "Aborted. OTA Job ID memory allocation failed");
        esp_rmaker_ota_report_status(ota_handle, OTA_STATUS_FAILED, "Aborted. OTA Updated ID memory allocation failed");
        goto end;
    }
    json_obj_get_string(&jctx, "ota_job_id", ota_job_id, len);
    nvs_handle handle;
    esp_err_t err = nvs_open_from_partition(ESP_RMAKER_NVS_PART_NAME, RMAKER_OTA_NVS_NAMESPACE, NVS_READWRITE, &handle);
    if (err == ESP_OK) {
        nvs_set_blob(handle, RMAKER_OTA_JOB_ID_NVS_NAME, ota_job_id, strlen(ota_job_id));
        nvs_close(handle);
    }
    ESP_LOGI(TAG, "OTA Job ID: %s", ota_job_id);
    ota->transient_priv = ota_job_id;
    len = 0;
    ret = json_obj_get_strlen(&jctx, "url", &len);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Aborted. URL not found in JSON");
        esp_rmaker_ota_report_status(ota_handle, OTA_STATUS_FAILED, "Aborted. URL not found in JSON");
        goto end;
    }
    len++; /* Increment for NULL character */
    url = calloc(1, len);
    if (!url) {
        ESP_LOGE(TAG, "Aborted. URL memory allocation failed");
        esp_rmaker_ota_report_status(ota_handle, OTA_STATUS_FAILED, "Aborted. URL memory allocation failed");
        goto end;
    }
    json_obj_get_string(&jctx, "url", url, len);
    ESP_LOGI(TAG, "URL: %s", url);

    int filesize = 0;
    json_obj_get_int(&jctx, "file_size", &filesize);
    ESP_LOGI(TAG, "File Size: %d", filesize);

    len = 0;
    ret = json_obj_get_strlen(&jctx, "fw_version", &len);
    if (ret == ESP_OK && len > 0) {
        len++; /* Increment for NULL character */
        fw_version = calloc(1, len);
        if (!fw_version) {
            ESP_LOGE(TAG, "Aborted. Firmware version memory allocation failed");
            esp_rmaker_ota_report_status(ota_handle, OTA_STATUS_FAILED, "Aborted. Firmware version memory allocation failed");
            goto end;
        }
        json_obj_get_string(&jctx, "fw_version", fw_version, len);
        ESP_LOGI(TAG, "Firmware version: %s", fw_version);
    }

    int metadata_size = 0;
    char *metadata = NULL;
    ret = json_obj_get_object_strlen(&jctx, "metadata", &metadata_size);
    if (ret == ESP_OK && metadata_size > 0) {
        metadata_size++; /* Increment for NULL character */
        metadata = calloc(1, metadata_size);
        if (!metadata) {
            ESP_LOGE(TAG, "Aborted. OTA metadata memory allocation failed");
            esp_rmaker_ota_report_status(ota_handle, OTA_STATUS_FAILED, "Aborted. OTA metadata memory allocation failed");
            goto end;
        }
        json_obj_get_object_str(&jctx, "metadata", metadata, metadata_size);
        ota->metadata = metadata;
    }

    json_parse_end(&jctx);
    if (ota->url) {
        free(ota->url);
    }
    ota->url = url;
    ota->fw_version = fw_version;
    ota->filesize = filesize;
    ota->ota_in_progress = true;

#ifdef CONFIG_ESP_MESH_LITE_OTA_ENABLE
    if (esp_mesh_lite_get_level() > 1) {
        const esp_app_desc_t *app_desc;
        app_desc = esp_app_get_description();
        ESP_LOGI(TAG, "OTA Image version for Project: %s. Expected: %s",
                 app_desc->version, fw_version);
        if (!strncmp(app_desc->version, fw_version, strnlen(fw_version, sizeof(app_desc->version)))) {
            ESP_LOGW(TAG, "Current running version is same as the new. We will not continue the update.");
            esp_rmaker_ota_report_status(ota_handle, OTA_STATUS_REJECTED, "Same version received");
            ESP_LOGE(TAG, "image header verification failed");
            esp_rmaker_ota_report_status(ota_handle, OTA_STATUS_FAILED, "Image validation failed");
        } else {
            esp_mesh_lite_file_transmit_config_t transmit_config = {
                .type = ESP_MESH_LITE_OTA_TRANSMIT_FIRMWARE,
                .size = filesize,
                .extern_url_ota_cb = esp_mesh_lite_ota_from_extern_url,
            };
            memcpy(transmit_config.fw_version, fw_version, sizeof(transmit_config.fw_version));
            esp_mesh_lite_transmit_file_start(&transmit_config);
            ota_info = ota;
        }
    } else {
        esp_mesh_lite_ota_notify_child_node_pause();
#endif
        if (esp_rmaker_work_queue_add_task(esp_rmaker_ota_common_cb, ota) != ESP_OK) {
            esp_rmaker_ota_finish_using_topics(ota);
        }
#ifdef CONFIG_ESP_MESH_LITE_OTA_ENABLE
    }
#endif
    return;
end:
    if (url) {
        free(url);
    }
    if (fw_version) {
        free(fw_version);
    }
    esp_rmaker_ota_finish_using_topics(ota);
    json_parse_end(&jctx);
    return;
}

esp_err_t esp_rmaker_mesh_lite_ota_subscribe_topic(void)
{
    char subscribe_topic[MQTT_TOPIC_BUFFER_SIZE];
    memset(subscribe_topic, 0, sizeof(subscribe_topic));
    snprintf(subscribe_topic, sizeof(subscribe_topic), "node/%s/%s",
             esp_rmaker_get_node_id(), OTAURL_TOPIC_SUFFIX);
    esp_err_t err = esp_rmaker_mqtt_unsubscribe(subscribe_topic);
    if (err == ESP_OK) {
        err = esp_rmaker_mqtt_subscribe(subscribe_topic, esp_rmaker_mesh_lite_ota_url_handler, RMAKER_MQTT_QOS1, esp_rmaker_ota);
        if (err != ESP_OK) {
            ESP_LOGE(TAG, "Failed to subscribe to %s. Error %d", subscribe_topic, err);
        }
    }
    ESP_LOGI(TAG, "MQTT Resubscribe [%s]", OTAURL_TOPIC_SUFFIX);
}

esp_err_t esp_rmaker_mesh_lite_ota_init(esp_rmaker_ota_config_t *ota_config)
{
    esp_err_t ret = ESP_FAIL;
    esp_rmaker_ota = calloc(1, sizeof(esp_rmaker_ota_t));
    if (!esp_rmaker_ota) {
        ESP_LOGE(TAG, "Failed to allocate memory for esp_rmaker_ota_t");
    } else {
        esp_rmaker_ota->ota_cb      = ota_config->ota_cb;
        esp_rmaker_ota->priv        = ota_config->priv;
        esp_rmaker_ota->server_cert = ota_config->server_cert;
        esp_rmaker_ota->type        = OTA_USING_TOPICS;
        ret = ESP_OK;
    }
    return ret;
}
