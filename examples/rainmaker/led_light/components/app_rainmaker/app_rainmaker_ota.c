/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <string.h>
#include <esp_log.h>
#include <esp_wifi.h>
#include <esp_rmaker_standard_params.h>
#include <esp_rmaker_ota.h>
#include <esp_rmaker_utils.h>
#include <app_rainmaker_ota.h>

static const char *TAG = "app_rainmaker_ota";

static esp_err_t validate_image_header(esp_rmaker_ota_handle_t ota_handle,
                                       esp_app_desc_t *new_app_info)
{
    if (new_app_info == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    const esp_partition_t *running = esp_ota_get_running_partition();
    esp_app_desc_t running_app_info;
    if (esp_ota_get_partition_description(running, &running_app_info) == ESP_OK) {
        ESP_LOGD(TAG, "Running firmware version: %s", running_app_info.version);
    }

#ifndef CONFIG_ESP_RMAKER_SKIP_PROJECT_NAME_CHECK
    if (memcmp(new_app_info->project_name, running_app_info.project_name, sizeof(new_app_info->project_name)) != 0) {
        ESP_LOGW(TAG, "OTA Image built for Project: %s. Expected: %s",
                 new_app_info->project_name, running_app_info.project_name);
        esp_rmaker_ota_report_status(ota_handle, OTA_STATUS_REJECTED, "Project Name mismatch");
        return ESP_FAIL;
    }
#endif

#ifndef CONFIG_ESP_RMAKER_SKIP_VERSION_CHECK
    if (memcmp(new_app_info->version, running_app_info.version, sizeof(new_app_info->version)) == 0) {
        ESP_LOGW(TAG, "Current running version is same as the new. We will not continue the update.");
        esp_rmaker_ota_report_status(ota_handle, OTA_STATUS_REJECTED, "Same version received");
        return ESP_FAIL;
    }
#endif

    return ESP_OK;
}

esp_err_t esp_rmaker_mesh_lite_ota_cb(esp_rmaker_ota_handle_t ota_handle, esp_rmaker_ota_data_t *ota_data)
{
    if (!ota_data->url) {
        return ESP_FAIL;
    }
    // esp_rmaker_ota_post_event(RMAKER_OTA_EVENT_STARTING, NULL, 0);
    esp_event_post(RMAKER_OTA_EVENT, RMAKER_OTA_EVENT_STARTING, NULL, 0, portMAX_DELAY);
    int buffer_size_tx = DEF_HTTP_TX_BUFFER_SIZE;
    /* In case received url is longer, we will increase the tx buffer size
     * to accomodate the longer url and other headers.
     */
    if (strlen(ota_data->url) > buffer_size_tx) {
        buffer_size_tx = strlen(ota_data->url) + 128;
    }
    esp_err_t ota_finish_err = ESP_OK;
    esp_http_client_config_t config = {
        .url = ota_data->url,
#ifdef ESP_RMAKER_USE_CERT_BUNDLE
        .crt_bundle_attach = esp_crt_bundle_attach,
#else
        .cert_pem = ota_data->server_cert,
#endif
        .timeout_ms = 5000,
        .buffer_size = DEF_HTTP_RX_BUFFER_SIZE,
        .buffer_size_tx = buffer_size_tx,
        .keep_alive_enable = true
    };
#ifdef CONFIG_ESP_RMAKER_SKIP_COMMON_NAME_CHECK
    config.skip_cert_common_name_check = true;
#endif

    esp_https_ota_config_t ota_config = {
        .http_config = &config,
    };

    if (ota_data->filesize) {
        ESP_LOGD(TAG, "Received file size: %d", ota_data->filesize);
    }

    esp_rmaker_ota_report_status(ota_handle, OTA_STATUS_IN_PROGRESS, "Starting OTA Upgrade");

    /* Using a warning just to highlight the message */
    ESP_LOGW(TAG, "Starting OTA. This may take time.");
    esp_https_ota_handle_t https_ota_handle = NULL;
    esp_err_t err = esp_https_ota_begin(&ota_config, &https_ota_handle);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "ESP HTTPS OTA Begin failed");
        esp_rmaker_ota_report_status(ota_handle, OTA_STATUS_FAILED, "ESP HTTPS OTA Begin failed");
        return ESP_FAIL;
    }

    /* Get the current Wi-Fi power save type. In case OTA fails and we need this
     * to restore power saving.
     */
    wifi_ps_type_t ps_type;
    esp_wifi_get_ps(&ps_type);
    /* Disable Wi-Fi power save to speed up OTA, iff BT is controller is idle/disabled.
     * Co-ex requirement, device panics otherwise.*/
#if CONFIG_BT_ENABLED
    if (esp_bt_controller_get_status() == ESP_BT_CONTROLLER_STATUS_IDLE) {
        esp_wifi_set_ps(WIFI_PS_NONE);
    }
#else
    esp_wifi_set_ps(WIFI_PS_NONE);
#endif /* CONFIG_BT_ENABLED */

    esp_app_desc_t app_desc;
    err = esp_https_ota_get_img_desc(https_ota_handle, &app_desc);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "esp_https_ota_read_img_desc failed");
        esp_rmaker_ota_report_status(ota_handle, OTA_STATUS_FAILED, "Failed to read image decription");
        goto ota_end;
    }
    err = validate_image_header(ota_handle, &app_desc);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "image header verification failed");
        goto ota_end;
    }

    esp_rmaker_ota_report_status(ota_handle, OTA_STATUS_IN_PROGRESS, "Downloading Firmware Image");
    int count = 0;
    while (1) {
#ifdef CONFIG_ESP_MESH_LITE_OTA_ENABLE
        if (esp_mesh_lite_wait_ota_allow() != ESP_OK) {
            err = ESP_FAIL;
            break;
        }
#endif
        err = esp_https_ota_perform(https_ota_handle);
        if (err != ESP_ERR_HTTPS_OTA_IN_PROGRESS) {
            break;
        }
        /* esp_https_ota_perform returns after every read operation which gives user the ability to
         * monitor the status of OTA upgrade by calling esp_https_ota_get_image_len_read, which gives length of image
         * data read so far.
         * We are using a counter just to reduce the number of prints
         */

        count++;
        if (count == 50) {
            ESP_LOGI(TAG, "Image bytes read: %d", esp_https_ota_get_image_len_read(https_ota_handle));
            count = 0;
        }
    }
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "ESP_HTTPS_OTA upgrade failed %s", esp_err_to_name(err));
        char description[40];
        snprintf(description, sizeof(description), "OTA failed: Error %s", esp_err_to_name(err));
        esp_rmaker_ota_report_status(ota_handle, OTA_STATUS_FAILED, description);
    }

    if (esp_https_ota_is_complete_data_received(https_ota_handle) != true) {
        // the OTA image was not completely received and user can customise the response to this situation.
        ESP_LOGE(TAG, "Complete data was not received.");
    } else {
        esp_rmaker_ota_report_status(ota_handle, OTA_STATUS_IN_PROGRESS, "Firmware Image download complete");
    }

ota_end:
#ifdef CONFIG_BT_ENABLED
    if (esp_bt_controller_get_status() == ESP_BT_CONTROLLER_STATUS_IDLE) {
        esp_wifi_set_ps(ps_type);
    }
#else
    esp_wifi_set_ps(ps_type);
#endif /* CONFIG_BT_ENABLED */
    ota_finish_err = esp_https_ota_finish(https_ota_handle);
    if ((err == ESP_OK) && (ota_finish_err == ESP_OK)) {
#ifdef CONFIG_BOOTLOADER_APP_ROLLBACK_ENABLE
        nvs_handle handle;
        esp_err_t err = nvs_open_from_partition(ESP_RMAKER_NVS_PART_NAME, RMAKER_OTA_NVS_NAMESPACE, NVS_READWRITE, &handle);
        if (err == ESP_OK) {
            uint8_t ota_update = 1;
            nvs_set_blob(handle, RMAKER_OTA_UPDATE_FLAG_NVS_NAME, &ota_update, sizeof(ota_update));
            nvs_close(handle);
        }
        /* Success will be reported after a reboot since Rollback is enabled */
        esp_rmaker_ota_report_status(ota_handle, OTA_STATUS_IN_PROGRESS, "Rebooting into new firmware");
#else
        esp_rmaker_ota_report_status(ota_handle, OTA_STATUS_SUCCESS, "OTA Upgrade finished successfully");
#endif
#ifndef CONFIG_ESP_RMAKER_OTA_DISABLE_AUTO_REBOOT
        ESP_LOGI(TAG, "OTA upgrade successful. Rebooting in %d seconds...", OTA_REBOOT_TIMER_SEC);
        esp_rmaker_reboot(OTA_REBOOT_TIMER_SEC);
#else
        ESP_LOGI(TAG, "OTA upgrade successful. Auto reboot is disabled. Requesting a Reboot via Event handler.");
        // esp_rmaker_ota_post_event(RMAKER_OTA_EVENT_REQ_FOR_REBOOT, NULL, 0);
        esp_event_post(RMAKER_OTA_EVENT, RMAKER_OTA_EVENT_REQ_FOR_REBOOT, NULL, 0, portMAX_DELAY);
#endif
        return ESP_OK;
    } else {
        if (ota_finish_err == ESP_ERR_OTA_VALIDATE_FAILED) {
            ESP_LOGE(TAG, "Image validation failed, image is corrupted");
            esp_rmaker_ota_report_status(ota_handle, OTA_STATUS_FAILED, "Image validation failed");
        } else {
            /* Not reporting status here, because relevant error will already be reported
             * in some earlier step
             */
            ESP_LOGE(TAG, "ESP_HTTPS_OTA upgrade failed %d", ota_finish_err);
        }
    }
    return ESP_FAIL;
}
