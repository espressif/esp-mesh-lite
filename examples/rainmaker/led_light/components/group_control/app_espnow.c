/*
 * SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <string.h>
#include <inttypes.h>
#include "freertos/FreeRTOS.h"
#include "freertos/timers.h"
#include "freertos/semphr.h"
#include "cJSON.h"
#include "esp_log.h"
#include "esp_mac.h"
#include "esp_timer.h"
#include "esp_netif_types.h"

#include "app_wifi.h"
#include "app_espnow.h"
#include "app_bridge.h"
#include "esp_mesh_lite.h"
#include "wifi_provisioning/manager.h"

extern char group_control_payload[ESPNOW_PAYLOAD_MAX_LEN];

static const char *TAG = "app_espnow";
static uint32_t current_seq = 0;
static TaskHandle_t group_control_handle = NULL;
static QueueHandle_t espnow_recv_queue = NULL;
static SemaphoreHandle_t sent_msgs_mutex = NULL;

static uint8_t s_broadcast_mac[ESP_NOW_ETH_ALEN] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };

typedef struct esp_now_msg_send {
    uint32_t retry_times;
    uint32_t max_retry;
    uint32_t msg_len;
    void* sent_msg;
} esp_now_msg_send_t;

static esp_now_msg_send_t* sent_msgs;

/* Parse received ESPNOW data. */
esp_err_t espnow_data_parse(uint8_t *data, uint16_t data_len)
{
    app_espnow_data_t *buf = (app_espnow_data_t *)data;

    if (data_len < sizeof(app_espnow_data_t)) {
        ESP_LOGD(TAG, "Receive ESPNOW data too short, len:%d", data_len);
        return ESP_FAIL;
    }

    if (buf->mesh_id == esp_mesh_lite_get_mesh_id()) {
        return ESP_OK;
    }

    return ESP_FAIL;
}

/* Prepare ESPNOW data to be sent. */
void espnow_data_prepare(uint8_t *buf, uint8_t* payload, size_t payload_len, bool seq_init)
{
    app_espnow_data_t *temp = (app_espnow_data_t*)buf;
    if (seq_init) {
        temp->seq = 0;
        current_seq = 0;
    } else {
        temp->seq = ++current_seq;
    }
    temp->mesh_id = esp_mesh_lite_get_mesh_id();
#if ESPNOW_DEBUG
    printf("send seq: %"PRIu32", current_seq: %"PRIu32"\r\n", temp->seq, current_seq);
    ESP_LOGW(TAG, "free heap %"PRIu32", minimum %"PRIu32"", esp_get_free_heap_size(), esp_get_minimum_free_heap_size());
#endif
    memcpy(temp->payload, payload, payload_len);
}

esp_err_t app_espnow_create_peer(uint8_t dst_mac[ESP_NOW_ETH_ALEN])
{
    esp_err_t ret = ESP_FAIL;
    esp_now_peer_info_t *peer = malloc(sizeof(esp_now_peer_info_t));
    if (peer == NULL) {
        ESP_LOGE(TAG, "Malloc peer information fail");
        return ESP_ERR_NO_MEM;
    }
    memset(peer, 0, sizeof(esp_now_peer_info_t));

    esp_now_get_peer(dst_mac, peer);
    peer->channel = 0;
    peer->ifidx = ESP_IF_WIFI_STA;
    peer->encrypt = false;
    // memcpy(peer->lmk, CONFIG_ESPNOW_LMK, ESP_NOW_KEY_LEN);
    memcpy(peer->peer_addr, dst_mac, ESP_NOW_ETH_ALEN);

    if (esp_now_is_peer_exist(dst_mac) == false) {
        ret = esp_now_add_peer(peer);
    } else {
        ret = esp_now_mod_peer(peer);
    }
    free(peer);

    return ret;
}

static void esp_now_send_timer_cb(TimerHandle_t timer)
{
    if (esp_mesh_lite_get_level() == 1) {
        xSemaphoreTake(sent_msgs_mutex, portMAX_DELAY);
        if (sent_msgs->max_retry > sent_msgs->retry_times) {
            sent_msgs->retry_times++;
            if (sent_msgs->sent_msg) {
                app_espnow_create_peer(s_broadcast_mac);
                esp_err_t ret = esp_mesh_lite_espnow_send(ESPNOW_DATA_TYPE_RM_GROUP_CONTROL, s_broadcast_mac, sent_msgs->sent_msg, sent_msgs->msg_len);
                if (ret != ESP_OK) {
                    ESP_LOGE(TAG, "Send error: %d [%s %d]", ret, __func__, __LINE__);
                }
            }
        } else {
            if (sent_msgs->max_retry) {
                sent_msgs->retry_times = 0;
                sent_msgs->max_retry = 0;
                sent_msgs->msg_len = 0;
                if (sent_msgs->sent_msg) {
                    free(sent_msgs->sent_msg);
                    sent_msgs->sent_msg = NULL;
                }
            }
        }
        xSemaphoreGive(sent_msgs_mutex);
    }
}

void esp_now_send_group_control(uint8_t *payload, bool seq_init)
{
    size_t payload_len = strlen((char*)payload);
    uint8_t *buf = calloc(1, payload_len + ESPNOW_PAYLOAD_HEAD_LEN);
    espnow_data_prepare(buf, payload, payload_len, seq_init);
    app_espnow_create_peer(s_broadcast_mac);
    esp_err_t ret = esp_mesh_lite_espnow_send(ESPNOW_DATA_TYPE_RM_GROUP_CONTROL, s_broadcast_mac, buf, payload_len + ESPNOW_PAYLOAD_HEAD_LEN);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Send error: %d [%s %d]", ret, __func__, __LINE__);
    }

    xSemaphoreTake(sent_msgs_mutex, portMAX_DELAY);
    sent_msgs->retry_times = 0;
    sent_msgs->max_retry = 2;
    sent_msgs->msg_len = payload_len + ESPNOW_PAYLOAD_HEAD_LEN;
    sent_msgs->sent_msg = buf;
    xSemaphoreGive(sent_msgs_mutex);
}

void esp_now_remove_send_msgs(void)
{
    xSemaphoreTake(sent_msgs_mutex, portMAX_DELAY);
    if (sent_msgs->max_retry) {
        sent_msgs->retry_times = 0;
        sent_msgs->max_retry = 0;
        sent_msgs->msg_len = 0;
        if (sent_msgs->sent_msg) {
            free(sent_msgs->sent_msg);
            sent_msgs->sent_msg = NULL;
        }
    }
    xSemaphoreGive(sent_msgs_mutex);
}

/* ESPNOW sending or receiving callback function is called in WiFi task.
 * Users should not do lengthy operations from this task. Instead, post
 * necessary data to a queue and handle it from a lower priority task. */
#if ESP_IDF_VERSION > ESP_IDF_VERSION_VAL(5, 4, 1)
static void espnow_send_cb(const esp_now_send_info_t *tx_info, esp_now_send_status_t status)
#else
static void espnow_send_cb(const uint8_t *mac_addr, esp_now_send_status_t status)
#endif
{
#if ESP_IDF_VERSION > ESP_IDF_VERSION_VAL(5, 4, 1)
    const uint8_t *mac_addr = tx_info->des_addr;
    if (tx_info == NULL) {
#else
    if (mac_addr == NULL) {
#endif
        ESP_LOGE(TAG, "Send cb arg error");
        return;
    }

#if ESPNOW_DEBUG
    if (status == ESP_NOW_SEND_SUCCESS) {
        ESP_LOGW(TAG, "Send OK to "MACSTR" %s %d", MAC2STR(mac_addr), __func__, __LINE__);
    } else {
        ESP_LOGW(TAG, "Send Fail %s %d ", __func__, __LINE__);
    }
#endif
}

static esp_err_t espnow_recv_cb(const esp_now_recv_info_t *recv_info, const uint8_t *data, int len)
{
    esp_mesh_lite_espnow_event_t evt;
    espnow_recv_cb_t *recv_cb = &evt.info.recv_cb;
    uint8_t *mac_addr = (uint8_t *)recv_info->src_addr;

    if (mac_addr == NULL || data == NULL || len <= 0) {
        ESP_LOGE(TAG, "Receive cb arg error");
        return ESP_FAIL;
    }

    if (espnow_data_parse(data, len) != ESP_OK) {
        return ESP_FAIL;
    }

    evt.id = ESPNOW_RECV_CB;
    memcpy(recv_cb->mac_addr, mac_addr, ESP_NOW_ETH_ALEN);
    recv_cb->data = malloc(len);
    if (recv_cb->data == NULL) {
        ESP_LOGE(TAG, "Malloc receive data fail");
        return ESP_FAIL;
    }
    memcpy(recv_cb->data, data, len);
    recv_cb->data_len = len;
    if (xQueueSend(espnow_recv_queue, &evt, ESPNOW_MAXDELAY) != pdTRUE) {
        ESP_LOGW(TAG, "Send receive queue fail");
        free(recv_cb->data);
        recv_cb->data = NULL;
        return ESP_FAIL;
    }
    return ESP_OK;
}

static void espnow_task(void *pvParameter)
{
    esp_mesh_lite_espnow_event_t evt;

    ESP_LOGI(TAG, "Start espnow task");

    while (xQueueReceive(espnow_recv_queue, &evt, portMAX_DELAY) == pdTRUE) {
        switch (evt.id) {
            case ESPNOW_RECV_CB:
                espnow_recv_cb_t *recv_cb = &evt.info.recv_cb;
                app_espnow_data_t *buf = (app_espnow_data_t *)recv_cb->data;
                uint32_t recv_seq = buf->seq;
                memset(group_control_payload, 0x0, ESPNOW_PAYLOAD_MAX_LEN);
                memcpy(group_control_payload, buf->payload, recv_cb->data_len - ESPNOW_PAYLOAD_HEAD_LEN);

                cJSON *light_js = NULL;
                cJSON *rmaker_data_js = cJSON_Parse((const char*)group_control_payload);
                if (rmaker_data_js) {
                    light_js = cJSON_GetObjectItem(rmaker_data_js, ESPNOW_DEVICE_NAME);
                    if (!light_js) {
                        cJSON_Delete(rmaker_data_js);
                        goto cleanup;
                    }
                } else {
                    goto cleanup;
                }

#if ESPNOW_DEBUG
                ESP_LOGI(TAG, "Receive broadcast data from: "MACSTR", len: %d, self level: %d, recv_seq: %"PRIu32", current_seq: %"PRIu32"",
                         MAC2STR(recv_cb->mac_addr),
                         recv_cb->data_len,
                         esp_mesh_lite_get_level(),
                         recv_seq,
                         current_seq);
#endif

                ESPNOW_MSG_MODE espnow_msg_mode = ESPNOW_MSG_MODE_INVALID;
                static ESPNOW_MSG_MODE last_espnow_msg_mode = ESPNOW_MSG_MODE_INVALID;
                static int64_t time_stamp;

                if (recv_seq > current_seq) {
                    espnow_msg_mode = ESPNOW_MSG_MODE_CONTROL;
                } else if (!recv_seq && current_seq) {
                    espnow_msg_mode = ESPNOW_MSG_MODE_RESET;
                }

                if (espnow_msg_mode && last_espnow_msg_mode && (last_espnow_msg_mode != espnow_msg_mode)) {
                    int64_t time_now = esp_timer_get_time();
                    int64_t time_interval = (time_now >= time_stamp) ? (time_now - time_stamp) : (INT64_MAX - time_stamp + time_now);
                    espnow_msg_mode = (time_interval < (1 * 1000 * 1000)) ? ESPNOW_MSG_MODE_INVALID : espnow_msg_mode;
                    if (espnow_msg_mode == ESPNOW_MSG_MODE_INVALID) {
                        current_seq = 0;
                    }
                }

                if (espnow_msg_mode) {
                    current_seq = recv_seq;
#if ESPNOW_DEBUG
                    ESP_LOGI(TAG, "recv_seq: %"PRIu32", current_seq: %"PRIu32"", recv_seq, current_seq);
#endif
                    // Data Forward
                    app_espnow_create_peer(s_broadcast_mac);
                    esp_err_t ret = esp_mesh_lite_espnow_send(ESPNOW_DATA_TYPE_RM_GROUP_CONTROL, s_broadcast_mac, recv_cb->data, recv_cb->data_len);
                    if (ret != ESP_OK) {
                        ESP_LOGE(TAG, "Send error: %d [%s %d]", ret, __func__, __LINE__);
                    }

                    cJSON *group_id_js = cJSON_GetObjectItem(light_js, ESPNOW_GROUP_ID);
                    if (group_id_js) {
                        if (group_id_js->valueint && (esp_rmaker_is_my_group_id(group_id_js->valueint) == false)) {
                            ESP_LOGW(TAG, "The Group_id[%d] does not belong to the device, and control information is ignored", group_id_js->valueint);
                        } else {
                            esp_rmaker_control_light_by_user(group_control_payload);
                        }
                    }

                    cJSON *control_js = cJSON_GetObjectItem(light_js, ESPNOW_DISTRIBUTION_NETWORK);
                    if (control_js) {
                        bool provisioned = false;
                        /* Let's find out if the device is provisioned */
                        ESP_ERROR_CHECK(wifi_prov_mgr_is_provisioned(&provisioned));
                        /* If device is not yet provisioned start provisioning service */
                        if (!provisioned) {
                            if (app_wifi_prov_is_timeout()) {
                                ESP_LOGI(TAG, "Starting provisioning");
                                esp_restart();
                            } else {
                                ESP_LOGW(TAG, "In provisioning");
                            }
                        } else {
                            ESP_LOGW(TAG, "Already provisioned");
                        }
                    }

                    last_espnow_msg_mode = espnow_msg_mode;
                    time_stamp = esp_timer_get_time();
                } else {
                    /* repeat */
                }
                cJSON_Delete(rmaker_data_js);
cleanup:
                free(recv_cb->data);
                recv_cb->data = NULL;
                break;
            default:
                ESP_LOGE(TAG, "Callback type error: %d", evt.id);
                break;
        }
    }
}

esp_err_t app_espnow_reset_group_control(void)
{
    esp_err_t ret = ESP_FAIL;
    cJSON *object = cJSON_CreateObject();
    cJSON *payload = cJSON_CreateObject();
    if (object) {
        if (payload) {
            cJSON_AddNumberToObject(payload, ESPNOW_GROUP_ID, 0);
            cJSON_AddItemToObject(object, ESPNOW_DEVICE_NAME, payload);
            char *rsp_string = cJSON_PrintUnformatted(object);
            if (rsp_string) {
                current_seq = 0;
                esp_now_send_group_control((uint8_t*)rsp_string, true);
                free(rsp_string);
                rsp_string = NULL;
                ret = ESP_OK;
            }
        }
        cJSON_Delete(object);
    }
    return ret;
}

static void ip_event_handler(void *arg, esp_event_base_t event_base,
                             int32_t event_id, void *event_data)
{
    if (esp_mesh_lite_get_level() == 1) {
        app_espnow_reset_group_control();
    }
}

esp_err_t group_control_init(void)
{
    ESP_ERROR_CHECK(esp_event_handler_instance_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &ip_event_handler, NULL, NULL));

    espnow_recv_queue = xQueueCreate(ESPNOW_QUEUE_SIZE, sizeof(esp_mesh_lite_espnow_event_t));
    if (espnow_recv_queue == NULL) {
        ESP_LOGE(TAG, "Create mutex fail");
        return ESP_FAIL;
    }

    ESP_ERROR_CHECK( esp_now_register_send_cb(espnow_send_cb) );
    esp_mesh_lite_espnow_recv_cb_register(ESPNOW_DATA_TYPE_RM_GROUP_CONTROL, espnow_recv_cb);

    /* Add broadcast peer information to peer list. */
    if (app_espnow_create_peer(s_broadcast_mac) != ESP_OK) {
        ESP_LOGE(TAG, "Malloc peer information fail");
        esp_now_unregister_send_cb();
        vSemaphoreDelete(espnow_recv_queue);
        espnow_recv_queue = NULL;
        return ESP_FAIL;
    }

    xTaskCreate(espnow_task, "espnow_task", 3 * 1024, NULL, 4, &group_control_handle);

    sent_msgs = (esp_now_msg_send_t*)malloc(sizeof(esp_now_msg_send_t));
    sent_msgs->max_retry = 0;
    sent_msgs->msg_len = 0;
    sent_msgs->retry_times = 0;
    sent_msgs->sent_msg = NULL;
    sent_msgs_mutex = xSemaphoreCreateMutex();

    TimerHandle_t esp_now_send_timer = xTimerCreate("esp_now_send_timer", 100 / portTICK_PERIOD_MS, pdTRUE,
            NULL, esp_now_send_timer_cb);
    xTimerStart(esp_now_send_timer, portMAX_DELAY);

    return ESP_OK;
}

esp_err_t group_control_deinit(void)
{
    ESP_ERROR_CHECK(esp_event_handler_instance_unregister(IP_EVENT, IP_EVENT_STA_GOT_IP, &ip_event_handler));
    esp_now_unregister_send_cb();

    if (group_control_handle) {
        vTaskDelete(group_control_handle);
        group_control_handle = NULL;
    }

    if (espnow_recv_queue) {
        vSemaphoreDelete(espnow_recv_queue);
        espnow_recv_queue = NULL;
    }
    return ESP_OK;
}
