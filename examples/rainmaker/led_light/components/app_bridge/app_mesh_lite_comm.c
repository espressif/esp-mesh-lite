/*
 * SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <string.h>
#include <stdlib.h>
#include "esp_log.h"
#include "esp_mac.h"
#include "esp_wifi.h"

#include "freertos/task.h"
#include "freertos/timers.h"
#include "freertos/FreeRTOS.h"

#include "esp_mesh_lite.h"
#include "app_bridge.h"

#define MAX_RETRY  5

typedef struct app_node_info_list {
    app_node_info_t* node;
    uint32_t ttl;
    struct app_node_info_list* next;
} app_node_info_list_t;

static const char* TAG = "Mesh-Lite";
static app_node_info_list_t* app_node_info_list = NULL;
static SemaphoreHandle_t report_child_info_mutex;

esp_err_t app_rmaker_mesh_lite_report_child_info(void)
{
    uint8_t mac[6];
    cJSON *item = NULL;
    char mac_str[MAC_MAX_LEN];

    esp_wifi_get_mac(WIFI_IF_STA, mac);
    snprintf(mac_str, sizeof(mac_str), MACSTR, MAC2STR(mac));

    char ip_str[IP_MAX_LEN];
    esp_netif_ip_info_t sta_ip;
    memset(&sta_ip, 0x0, sizeof(esp_netif_ip_info_t));
    esp_netif_get_ip_info(esp_netif_get_handle_from_ifkey("WIFI_STA_DEF"), &sta_ip);
    sprintf(ip_str, IPSTR, IP2STR(&sta_ip.ip));

    item = cJSON_CreateObject();
    if (item) {
        cJSON_AddNumberToObject(item, "level", esp_mesh_lite_get_level());
        cJSON_AddStringToObject(item, "mac", mac_str);
        cJSON_AddStringToObject(item, "ip", ip_str);
        esp_mesh_lite_msg_config_t config = {
            .json_msg = {
                .send_msg = "report_child_info",
                .expect_msg = "report_child_info_ack",
                .max_retry = MAX_RETRY,
                .req_payload = item,
                .resend = esp_mesh_lite_send_msg_to_parent,
            }
        };
        esp_mesh_lite_send_msg(ESP_MESH_LITE_JSON_MSG, &config);
        cJSON_Delete(item);
    }

    return ESP_OK;
}

static esp_err_t esp_mesh_lite_child_node_info_add(uint8_t level, char* mac, char* ip)
{
    xSemaphoreTake(report_child_info_mutex, portMAX_DELAY);
    app_node_info_list_t* new = app_node_info_list;

    while (new) {
        if (!strncmp(new->node->mac, mac, (MAC_MAX_LEN - 1))) {
            new->ttl = (120 + 10);
            xSemaphoreGive(report_child_info_mutex);
            return ESP_ERR_DUPLICATE_ADDITION;
        }
        new = new->next;
    }

    /* not found, create a new */
    new = (app_node_info_list_t*)malloc(sizeof(app_node_info_list_t));
    if (new == NULL) {
        ESP_LOGE(TAG, "node info add fail(no mem)");
        xSemaphoreGive(report_child_info_mutex);
        return ESP_ERR_NO_MEM;
    }

    new->node = (app_node_info_t*)malloc(sizeof(app_node_info_t));
    if (new->node == NULL) {
        free(new);
        ESP_LOGE(TAG, "node info add fail(no mem)");
        xSemaphoreGive(report_child_info_mutex);
        return ESP_ERR_NO_MEM;
    }

    memcpy(new->node->mac, mac, MAC_MAX_LEN);
    memcpy(new->node->ip, ip, IP_MAX_LEN);
    new->node->level = level;
    new->ttl = (120 + 10);

    new->next = app_node_info_list;
    app_node_info_list = new;

    esp_event_post(ESP_MESH_LITE_EVENT, ESP_MESH_LITE_EVENT_CHILD_NODE_JOIN, new->node, sizeof(app_node_info_t), 0);
    xSemaphoreGive(report_child_info_mutex);
    return ESP_OK;
}

static cJSON* report_child_info_process(cJSON *payload, uint32_t seq)
{
    cJSON *found = NULL;
    char* mac_str = NULL;

    found = cJSON_GetObjectItem(payload, "level");
    uint8_t level = found->valueint;
    found = cJSON_GetObjectItem(payload, "mac");
    mac_str = found->valuestring;
    found = cJSON_GetObjectItem(payload, "ip");

    esp_mesh_lite_child_node_info_add(level, mac_str, found->valuestring);
    return NULL;
}

static cJSON* report_child_info_ack_process(cJSON *payload, uint32_t seq)
{
    return NULL;
}

static const esp_mesh_lite_msg_action_t report_child_info_action[] = {
    /* Report information to the root node */
    {"report_child_info", "report_child_info_ack", report_child_info_process},
    {"report_child_info_ack", NULL, report_child_info_ack_process},

    {NULL, NULL, NULL} /* Must be NULL terminated */
};

static void root_timer_cb(TimerHandle_t timer)
{
    if (esp_mesh_lite_get_level() > ROOT) {
        return;
    }

    if (xSemaphoreTake(report_child_info_mutex, 0) != pdTRUE) {
        return;
    }

    app_node_info_list_t* current = app_node_info_list;
    app_node_info_list_t* prev = NULL;

    while (current) {
        if (current->ttl == 0) {
            esp_event_post(ESP_MESH_LITE_EVENT, ESP_MESH_LITE_EVENT_CHILD_NODE_LEAVE, current->node, sizeof(app_node_info_t), 0);
            if (app_node_info_list == current) {
                app_node_info_list = current->next;
                free(current->node);
                free(current);
                current = app_node_info_list;
            } else {
                prev->next = current->next;
                free(current->node);
                free(current);
                current = prev->next;
            }
            continue;
        } else {
            current->ttl--;
        }
        prev = current;
        current = current->next;
    }
    xSemaphoreGive(report_child_info_mutex);
}

static void report_timer_cb(TimerHandle_t timer)
{
    if (esp_mesh_lite_get_level() > ROOT) {
        app_rmaker_mesh_lite_report_child_info();
    }
}

void app_rmaker_mesh_lite_report_info_to_parent(void)
{
    report_child_info_mutex = xSemaphoreCreateMutex();
    esp_mesh_lite_msg_action_list_register(report_child_info_action);

    TimerHandle_t report_timer = xTimerCreate("report_timer", 120 * 1000 / portTICK_PERIOD_MS,
                                              pdTRUE, NULL, report_timer_cb);
    TimerHandle_t root_timer = xTimerCreate("root_timer", 1 * 1000 / portTICK_PERIOD_MS,
                                            pdTRUE, NULL, root_timer_cb);
    xTimerStart(report_timer, portMAX_DELAY);
    xTimerStart(root_timer, portMAX_DELAY);
}
