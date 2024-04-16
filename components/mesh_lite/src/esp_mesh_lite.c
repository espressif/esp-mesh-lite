/*
 * SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <string.h>
#include <stdlib.h>
#include "esp_log.h"
#include "esp_wifi.h"

#include "freertos/task.h"
#include "freertos/timers.h"
#include "freertos/FreeRTOS.h"

#if ESP_IDF_VERSION >= ESP_IDF_VERSION_VAL(4, 4, 0)
#include "esp_mac.h"
#endif

#include "esp_bridge.h"

#include "esp_mesh_lite.h"

static const char *TAG = "Mesh-Lite";

#if CONFIG_MESH_LITE_NODE_INFO_REPORT

#define MAX_RETRY  5

typedef struct node_info_list {
    esp_mesh_lite_node_info_t* node;
    uint32_t ttl;
    struct node_info_list* next;
} node_info_list_t;

static node_info_list_t* node_info_list = NULL;
static SemaphoreHandle_t node_info_mutex;

esp_err_t esp_mesh_lite_report_info(void)
{
    uint8_t mac[6];
    cJSON *item = NULL;
    char mac_str[MAC_MAX_LEN];

    esp_wifi_get_mac(WIFI_IF_STA, mac);
    snprintf(mac_str, sizeof(mac_str), MACSTR, MAC2STR(mac));

    item = cJSON_CreateObject();
    if (item) {
        cJSON_AddNumberToObject(item, "level", esp_mesh_lite_get_level());
        cJSON_AddStringToObject(item, "mac", mac_str);
        esp_mesh_lite_try_sending_msg("report_info", "report_info_ack", MAX_RETRY, item, &esp_mesh_lite_send_msg_to_root);
        cJSON_Delete(item);
    }

    return ESP_OK;
}

static esp_err_t esp_mesh_lite_node_info_add(uint8_t level, char* mac)
{
    xSemaphoreTake(node_info_mutex, portMAX_DELAY);
    node_info_list_t* new = node_info_list;

    while (new) {
        if (!strncmp(new->node->mac, mac, (MAC_MAX_LEN - 1))) {
            if (new->node->level != level) {
                new->node->level = level;
                esp_event_post(ESP_MESH_LITE_EVENT, ESP_MESH_LITE_EVENT_NODE_CHANGE, new->node, sizeof(esp_mesh_lite_node_info_t), 0);
            }
            new->ttl = (CONFIG_MESH_LITE_REPORT_INTERVAL + 10);
            xSemaphoreGive(node_info_mutex);
            return ESP_ERR_DUPLICATE_ADDITION;
        }
        new = new->next;
    }

    /* not found, create a new */
    new = (node_info_list_t*)malloc(sizeof(node_info_list_t));
    if (new == NULL) {
        ESP_LOGE(TAG, "node info add fail(no mem)");
        xSemaphoreGive(node_info_mutex);
        return ESP_ERR_NO_MEM;
    }

    new->node = (esp_mesh_lite_node_info_t*)malloc(sizeof(esp_mesh_lite_node_info_t));
    if (new->node == NULL) {
        free(new);
        ESP_LOGE(TAG, "node info add fail(no mem)");
        xSemaphoreGive(node_info_mutex);
        return ESP_ERR_NO_MEM;
    }

    memcpy(new->node->mac, mac, MAC_MAX_LEN);
    new->node->level = level;
    new->ttl = (CONFIG_MESH_LITE_REPORT_INTERVAL + 10);

    new->next = node_info_list;
    node_info_list = new;

    esp_event_post(ESP_MESH_LITE_EVENT, ESP_MESH_LITE_EVENT_NODE_JOIN, new->node, sizeof(esp_mesh_lite_node_info_t), 0);
    xSemaphoreGive(node_info_mutex);
    return ESP_OK;
}

static cJSON* report_info_process(cJSON *payload, uint32_t seq)
{
    cJSON *found = NULL;

    found = cJSON_GetObjectItem(payload, "level");
    uint8_t level = found->valueint;
    found = cJSON_GetObjectItem(payload, "mac");

    esp_mesh_lite_node_info_add(level, found->valuestring);
    return NULL;
}

static cJSON* report_info_ack_process(cJSON *payload, uint32_t seq)
{
    return NULL;
}

static const esp_mesh_lite_msg_action_t node_report_action[] = {
    /* Report information to the root node */
    {"report_info", "report_info_ack", report_info_process},
    {"report_info_ack", NULL, report_info_ack_process},

    {NULL, NULL, NULL} /* Must be NULL terminated */
};

static void root_timer_cb(TimerHandle_t timer)
{
    if (esp_mesh_lite_get_level() > ROOT) {
        return;
    }

    if (xSemaphoreTake(node_info_mutex, 0) != pdTRUE) {
        return;
    }

    node_info_list_t* current = node_info_list;
    node_info_list_t* prev = NULL;

    while (current) {
        if (current->ttl == 0) {
            esp_event_post(ESP_MESH_LITE_EVENT, ESP_MESH_LITE_EVENT_NODE_LEAVE, current->node, sizeof(esp_mesh_lite_node_info_t), 0);
            if (node_info_list == current) {
                node_info_list = current->next;
                free(current->node);
                free(current);
                current = node_info_list;
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
    xSemaphoreGive(node_info_mutex);
}

static void report_timer_cb(TimerHandle_t timer)
{
    if (esp_mesh_lite_get_level() > ROOT) {
        esp_mesh_lite_report_info();
    }
}

static void esp_mesh_lite_event_ip_event_handler(void *arg, esp_event_base_t event_base,
                                                 int32_t event_id, void *event_data)
{
    if (esp_mesh_lite_get_level() > 1) {
        esp_mesh_lite_report_info();
    }
}
#endif /* MESH_LITE_NODE_INFO_REPORT */

uint8_t esp_mesh_lite_get_child_node_number(void)
{
    uint8_t node_number = 0;
#ifdef CONFIG_MESH_LITE_NODE_INFO_REPORT
    node_info_list_t* current = node_info_list;

    if (xSemaphoreTake(node_info_mutex, portMAX_DELAY) != pdTRUE) {
        return node_number;
    }

    while (current) {
        node_number++;
        current = current->next;
    }
    xSemaphoreGive(node_info_mutex);
#endif /* MESH_LITE_NODE_INFO_REPORT */
    return node_number;
}

static void esp_mesh_lite_event_ip_changed_handler(void *arg, esp_event_base_t event_base,
                                                  int32_t event_id, void *event_data)
{
    switch(event_id) {
        case ESP_MESH_LITE_EVENT_CORE_STARTED:
            ESP_LOGI(TAG, "Mesh-Lite connecting");
            esp_mesh_lite_connect();
            break;
        case ESP_MESH_LITE_EVENT_CORE_INHERITED_NET_SEGMENT_CHANGED:
            ESP_LOGI(TAG, "netif network segment conflict check");
            if (esp_mesh_lite_get_level() > CONFIG_MESH_LITE_MAXIMUM_LEVEL_ALLOWED) {
                ESP_LOGW(TAG, "The maximum level has been exceeded, and the Wi-Fi connection has been disconnected to search for a new parent node.");
                esp_mesh_lite_connect();
            }
            esp_bridge_netif_network_segment_conflict_update(NULL);
            break;
        case ESP_MESH_LITE_EVENT_CORE_ROUTER_INFO_CHANGED:
            break;
        case ESP_MESH_LITE_EVENT_CORE_STATUS_FAIL:
            // ESP_LOGE(TAG, "Mesh-Lite Core is in abnormal state, preparing to reboot the device.");
            // assert(0);
            break;
        case ESP_MESH_LITE_EVENT_OTA_START:
            ESP_LOGI(TAG, "OTA Start\r\n");
            break;
        case ESP_MESH_LITE_EVENT_OTA_FINISH: {
            esp_mesh_lite_event_ota_finish_t *event = (esp_mesh_lite_event_ota_finish_t*)event_data;
            if (event->reason == ESP_MESH_LITE_EVENT_OTA_SUCCESS) {
                ESP_LOGI(TAG, "LAN OTA Success!");
                esp_restart();
            } else if (event->reason == ESP_MESH_LITE_EVENT_OTA_REJECTED) {
                ESP_LOGE(TAG, "LAN OTA Rejected\r\n");
            } else {
                ESP_LOGE(TAG, "LAN OTA Fail! Reason: %d\r\n", event->reason);
            }
            break;
        }
        case ESP_MESH_LITE_EVENT_OTA_PROGRESS: {
            esp_mesh_lite_event_ota_progress_t *event = (esp_mesh_lite_event_ota_progress_t*)event_data;
            ESP_LOGI(TAG, "LAN OTA Percentage: %d%%", event->percentage);
            break;
        }
    }
}

void esp_mesh_lite_init(esp_mesh_lite_config_t* config)
{
    ESP_LOGI(TAG, "esp-mesh-lite component version: %d.%d.%d", MESH_LITE_VER_MAJOR, MESH_LITE_VER_MINOR, MESH_LITE_VER_PATCH);

    esp_bridge_network_segment_check_register(esp_mesh_lite_network_segment_is_used);
    esp_event_handler_instance_register(ESP_MESH_LITE_EVENT, ESP_EVENT_ANY_ID, &esp_mesh_lite_event_ip_changed_handler, NULL, NULL);

    esp_mesh_lite_espnow_init();

    esp_mesh_lite_core_init(config);
#if CONFIG_MESH_LITE_NODE_INFO_REPORT
    esp_event_handler_instance_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &esp_mesh_lite_event_ip_event_handler, NULL, NULL);

    node_info_mutex = xSemaphoreCreateMutex();
    esp_mesh_lite_msg_action_list_register(node_report_action);

    TimerHandle_t report_timer = xTimerCreate("report_timer", CONFIG_MESH_LITE_REPORT_INTERVAL * 1000 / portTICK_PERIOD_MS, 
                                              pdTRUE, NULL, report_timer_cb);
    TimerHandle_t root_timer = xTimerCreate("root_timer", 1 * 1000 / portTICK_PERIOD_MS, 
                                              pdTRUE, NULL, root_timer_cb);
    xTimerStart(report_timer, portMAX_DELAY);
    xTimerStart(root_timer, portMAX_DELAY);
#endif /* MESH_LITE_NODE_INFO_REPORT */
}
