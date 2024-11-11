/*
 * SPDX-FileCopyrightText: 2019-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include <string.h>
#include <esp_err.h>
#include <esp_log.h>

#include <esp_wifi.h>
#include <esp_netif.h>

#include "wifi_provisioning/wifi_config.h"
#include "wifi_provisioning/wifi_scan.h"
#include "wifi_ctrl.h"
#include "wifi_provisioning/manager.h"
#include "wifi_provisioning_priv.h"

static const char *TAG = "wifi_prov_handlers";

typedef struct mesh_lite_config {
    uint8_t ssid[32];                         /**< SSID of target AP. */
    uint8_t password[64];
    uint32_t mesh_id;
} mesh_lite_config_t;
/* Provide definition of wifi_prov_ctx_t */
struct wifi_prov_ctx {
    wifi_config_t wifi_cfg;
    mesh_lite_config_t mesh_lite_cfg;
};

static wifi_config_t *get_config(wifi_prov_ctx_t **ctx)
{
    return (*ctx ? & (*ctx)->wifi_cfg : NULL);
}

static wifi_config_t *new_config(wifi_prov_ctx_t **ctx)
{
    free(*ctx);
    (*ctx) = (wifi_prov_ctx_t *) calloc(1, sizeof(wifi_prov_ctx_t));
    return get_config(ctx);
}

static void free_config(wifi_prov_ctx_t **ctx)
{
    free(*ctx);
    *ctx = NULL;
}

static esp_err_t get_status_handler(wifi_prov_config_get_data_t *resp_data, wifi_prov_ctx_t **ctx)
{
    /* Initialize to zero */
    memset(resp_data, 0, sizeof(wifi_prov_config_get_data_t));

    if (wifi_prov_mgr_get_wifi_state(&resp_data->wifi_state) != ESP_OK) {
        ESP_LOGW(TAG, "Wi-Fi provisioning manager not running");
        return ESP_ERR_INVALID_STATE;
    }

    if (resp_data->wifi_state == WIFI_PROV_STA_CONNECTED) {
        ESP_LOGD(TAG, "Got state : connected");

        /* IP Addr assigned to STA */
        esp_netif_ip_info_t ip_info;
        esp_netif_get_ip_info(esp_netif_get_handle_from_ifkey("WIFI_STA_DEF"), &ip_info);
        esp_ip4addr_ntoa(&ip_info.ip, resp_data->conn_info.ip_addr, sizeof(resp_data->conn_info.ip_addr));

        /* AP information to which STA is connected */
        wifi_ap_record_t ap_info;
        esp_wifi_sta_get_ap_info(&ap_info);
        memcpy(resp_data->conn_info.bssid, (char *)ap_info.bssid, sizeof(ap_info.bssid));
        memcpy(resp_data->conn_info.ssid, (char *)ap_info.ssid,  sizeof(ap_info.ssid));
        resp_data->conn_info.channel   = ap_info.primary;
        resp_data->conn_info.auth_mode = ap_info.authmode;

        /* Tell manager to stop provisioning service */
        wifi_prov_mgr_done();
    } else if (resp_data->wifi_state == WIFI_PROV_STA_DISCONNECTED) {
        ESP_LOGD(TAG, "Got state : disconnected");

        /* If disconnected, convey reason */
        wifi_prov_mgr_get_wifi_disconnect_reason(&resp_data->fail_reason);
    } else {
        ESP_LOGD(TAG, "Got state : connecting");
    }
    return ESP_OK;
}

static esp_err_t set_config_handler(const wifi_prov_config_set_data_t *req_data, wifi_prov_ctx_t **ctx)
{
    wifi_config_t *wifi_cfg = get_config(ctx);
    if (wifi_cfg) {
        free_config(ctx);
    }

    wifi_cfg = new_config(ctx);
    if (!wifi_cfg) {
        ESP_LOGE(TAG, "Unable to allocate Wi-Fi config");
        return ESP_ERR_NO_MEM;
    }

    ESP_LOGD(TAG, "Wi-Fi Credentials Received");

    /* Using memcpy allows the max SSID length to be 32 bytes (as per 802.11 standard).
     * But this doesn't guarantee that the saved SSID will be null terminated, because
     * wifi_cfg->sta.ssid is also 32 bytes long (without extra 1 byte for null character).
     * Although, this is not a matter for concern because esp_wifi library reads the SSID
     * upto 32 bytes in absence of null termination */
    const size_t ssid_len = strnlen(req_data->ssid, sizeof(wifi_cfg->sta.ssid));
    /* Ensure SSID less than 32 bytes is null terminated */
    memset(wifi_cfg->sta.ssid, 0, sizeof(wifi_cfg->sta.ssid));
    memcpy(wifi_cfg->sta.ssid, req_data->ssid, ssid_len);

    /* Using strlcpy allows both max passphrase length (63 bytes) and ensures null termination
     * because size of wifi_cfg->sta.password is 64 bytes (1 extra byte for null character) */
    strlcpy((char *) wifi_cfg->sta.password, req_data->password, sizeof(wifi_cfg->sta.password));

#ifdef CONFIG_MESH_LITE_WIFI_PROV_STA_ALL_CHANNEL_SCAN
    wifi_cfg->sta.scan_method = WIFI_ALL_CHANNEL_SCAN;
#else /* CONFIG_MESH_LITE_WIFI_PROV_STA_FAST_SCAN */
    wifi_cfg->sta.scan_method = WIFI_FAST_SCAN;
#endif

    mesh_lite_config_t* mesh_lite_cfg = &((*ctx)->mesh_lite_cfg);

    memset(mesh_lite_cfg->ssid, 0, sizeof(mesh_lite_cfg->ssid));
    memcpy(mesh_lite_cfg->ssid, req_data->softap_ssid, sizeof(mesh_lite_cfg->ssid));

    memset(mesh_lite_cfg->password, 0, sizeof(mesh_lite_cfg->password));
    memcpy(mesh_lite_cfg->password, req_data->softap_passphrase, sizeof(mesh_lite_cfg->password));
    mesh_lite_cfg->mesh_id = req_data->mesh_id;

    return ESP_OK;
}

static esp_err_t apply_config_handler(wifi_prov_ctx_t **ctx)
{
    wifi_config_t *wifi_cfg = get_config(ctx);
    if (!wifi_cfg) {
        ESP_LOGE(TAG, "Wi-Fi config not set");
        return ESP_ERR_INVALID_STATE;
    }
    esp_err_t wifi_prov_mgr_configure_meshlite(void *data, uint32_t size);
    mesh_lite_config_t* mesh_lite_cfg = &((*ctx)->mesh_lite_cfg);

    ESP_LOGI(TAG, "mesh_lite_cfg->softap_ssid:%s", mesh_lite_cfg->ssid);
    ESP_LOGI(TAG, "mesh_lite_cfg->softap_passphrase:%s", mesh_lite_cfg->password);
    ESP_LOGI(TAG, "mesh_lite_cfg->mesh_id:%ld", mesh_lite_cfg->mesh_id);
    wifi_prov_mgr_configure_meshlite(mesh_lite_cfg, sizeof(mesh_lite_config_t));

    esp_err_t ret = wifi_prov_mgr_configure_sta(wifi_cfg);
    if (ret == ESP_OK) {
        ESP_LOGD(TAG, "Wi-Fi Credentials Applied");
    } else {
        ESP_LOGE(TAG, "Failed to apply Wi-Fi Credentials");
    }

    free_config(ctx);
    return ret;
}

esp_err_t get_wifi_prov_handlers(wifi_prov_config_handlers_t *ptr)
{
    if (!ptr) {
        return ESP_ERR_INVALID_ARG;
    }
    ptr->get_status_handler   = get_status_handler;
    ptr->set_config_handler   = set_config_handler;
    ptr->apply_config_handler = apply_config_handler;
    ptr->ctx = NULL;
    return ESP_OK;
}

/*************************************************************************/

static esp_err_t scan_start(bool blocking, bool passive,
                            uint8_t group_channels, uint32_t period_ms,
                            wifi_prov_scan_ctx_t **ctx)
{
    return wifi_prov_mgr_wifi_scan_start(blocking, passive, group_channels, period_ms);
}

static esp_err_t scan_status(bool *scan_finished,
                             uint16_t *result_count,
                             wifi_prov_scan_ctx_t **ctx)
{
    *scan_finished = wifi_prov_mgr_wifi_scan_finished();
    *result_count  = wifi_prov_mgr_wifi_scan_result_count();
    return ESP_OK;
}

static esp_err_t scan_result(uint16_t result_index,
                             wifi_prov_scan_result_t *result,
                             wifi_prov_scan_ctx_t **ctx)
{
    const wifi_ap_record_t *record = wifi_prov_mgr_wifi_scan_result(result_index);
    if (!record) {
        return ESP_FAIL;
    }

    /* Compile time check ensures memory safety in case SSID length in
     * record / result structure definition changes in future */
    _Static_assert(sizeof(result->ssid) == sizeof(record->ssid),
                   "source and destination should be of same size");
    memcpy(result->ssid, record->ssid, sizeof(record->ssid));
    memcpy(result->bssid, record->bssid, sizeof(record->bssid));
    result->channel = record->primary;
    result->rssi = record->rssi;
    result->auth = record->authmode;
    return ESP_OK;
}

esp_err_t get_wifi_scan_handlers(wifi_prov_scan_handlers_t *ptr)
{
    if (!ptr) {
        return ESP_ERR_INVALID_ARG;
    }
    ptr->scan_start  = scan_start;
    ptr->scan_status = scan_status;
    ptr->scan_result = scan_result;
    ptr->ctx = NULL;
    return ESP_OK;
}

/*************************************************************************/

static esp_err_t ctrl_reset(void)
{
    return wifi_prov_mgr_reset_sm_state_on_failure();
}

static esp_err_t ctrl_reprov(void)
{
    return wifi_prov_mgr_reset_sm_state_for_reprovision();
}

esp_err_t get_wifi_ctrl_handlers(wifi_ctrl_handlers_t *ptr)
{
    if (!ptr) {
        return ESP_ERR_INVALID_ARG;
    }
    ptr->ctrl_reset  = ctrl_reset;
    ptr->ctrl_reprov  = ctrl_reprov;
    return ESP_OK;
}
