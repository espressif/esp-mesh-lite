/*
 * SPDX-FileCopyrightText: 2022-2024 Espressif Systems (Shanghai) CO LTD
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
#include "esp_mac.h"
#include "esp_bridge.h"
#include "esp_mesh_lite.h"
#include "mesh_lite.pb-c.h"

static const char *TAG = "Mesh-Lite";

#if CONFIG_MESH_LITE_NODE_INFO_REPORT

#define MAX_RETRY  5

static uint32_t nodes_num = 0;
static node_info_list_t *node_info_list = NULL;
static SemaphoreHandle_t node_info_mutex;

static esp_err_t esp_mesh_lite_node_info_update(uint8_t level, uint8_t* mac, uint32_t ip_addr);
static esp_err_t esp_mesh_lite_update_nodes_info_to_children(void);

const node_info_list_t *esp_mesh_lite_get_nodes_list(uint32_t *size)
{
    if (size) {
        *size = nodes_num;
    }
    return node_info_list;
}

esp_err_t esp_mesh_lite_report_info(void)
{
    uint8_t mac[6];
    esp_netif_ip_info_t ip_addr;
    esp_netif_t *external_netif = esp_netif_get_handle_from_ifkey("WIFI_STA_DEF");

    esp_wifi_get_mac(WIFI_IF_STA, mac);
    esp_netif_get_ip_info(external_netif, &ip_addr);

    if (esp_mesh_lite_get_level() < ROOT) {
        return ESP_OK;
    }

    if (esp_mesh_lite_get_level() == ROOT) {
        esp_mesh_lite_node_info_update(ROOT, mac, ip_addr.ip.addr);
        return ESP_OK;
    }

    MeshLite__NodeData req;
    mesh_lite__node_data__init(&req);
    req.node_level = esp_mesh_lite_get_level();
    req.node_ip = ip_addr.ip.addr;
    req.node_mac.len = ETH_HWADDR_LEN;
    req.node_mac.data = mac;
    uint32_t outlen = mesh_lite__node_data__get_packed_size(&req);
    uint8_t *outdata = malloc(outlen);
    mesh_lite__node_data__pack(&req, outdata);

    esp_mesh_lite_msg_config_t config = {
        .raw_msg = {
            .msg_id = MESH_LITE_MSG_ID_REPORT_NODE_INFO,
            .expect_resp_msg_id = MESH_LITE_MSG_ID_REPORT_NODE_INFO_RESP,
            .max_retry = 3,
            .data = outdata,
            .size = outlen,
            .raw_resend = esp_mesh_lite_send_raw_msg_to_root,
        },
    };
    esp_mesh_lite_send_msg(ESP_MESH_LITE_RAW_MSG, &config);
    free(outdata);

    return ESP_OK;
}

static esp_err_t esp_mesh_lite_node_info_update(uint8_t level, uint8_t* mac, uint32_t ip_addr)
{
    xSemaphoreTake(node_info_mutex, portMAX_DELAY);
    node_info_list_t* new = node_info_list;

    while (new) {
        if (!memcmp(new->node->mac_addr, mac, ETH_HWADDR_LEN)) {
            new->ttl = (CONFIG_MESH_LITE_REPORT_INTERVAL + MESH_LITE_REPORT_INTERVAL_BUFFER);
            if ((new->node->level != level) || (new->node->ip_addr != ip_addr)) {
                new->node->level = level;
                new->node->ip_addr = ip_addr;
            } else {
                xSemaphoreGive(node_info_mutex);
                return ESP_ERR_DUPLICATE_ADDITION;
            }
            xSemaphoreGive(node_info_mutex);
            esp_event_post(ESP_MESH_LITE_EVENT, ESP_MESH_LITE_EVENT_NODE_CHANGE, new->node, sizeof(esp_mesh_lite_node_info_t), 0);
            return ESP_OK;
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

    memcpy(new->node->mac_addr, mac, ETH_HWADDR_LEN);
    new->node->level = level;
    new->node->ip_addr = ip_addr;
    new->ttl = (CONFIG_MESH_LITE_REPORT_INTERVAL + MESH_LITE_REPORT_INTERVAL_BUFFER);

    new->next = node_info_list;
    node_info_list = new;
    nodes_num++;

    xSemaphoreGive(node_info_mutex);
    esp_event_post(ESP_MESH_LITE_EVENT, ESP_MESH_LITE_EVENT_NODE_JOIN, new->node, sizeof(esp_mesh_lite_node_info_t), 0);
    return ESP_OK;
}

static esp_err_t mesh_lite_report_nodes_handler(uint8_t *data, uint32_t len, uint8_t **out_data, uint32_t* out_len, uint32_t seq)
{
    MeshLite__NodeData* req = NULL;
    esp_err_t ret = ESP_FAIL;

    *out_len = 0;
    if (esp_mesh_lite_get_level() != ROOT) {
        return ESP_ERR_NOT_SUPPORTED;
    }

    req = mesh_lite__node_data__unpack(NULL, len, data);

    if (req) {
        if (req->node_mac.len > 0) {
            if ((req->node_level > 0) && (req->node_ip > 0)) {
                ret = esp_mesh_lite_node_info_update(req->node_level, req->node_mac.data, req->node_ip);
                if (ret == ESP_OK) {
                    esp_mesh_lite_update_nodes_info_to_children();
                } else if (ret == ESP_ERR_DUPLICATE_ADDITION) {
                    ret = ESP_OK;
                }
            }
        }
        mesh_lite__node_data__free_unpacked(req, NULL);
    }

    return ret;
}

static esp_err_t mesh_lite_update_nodes_list(uint8_t *data, uint32_t len, uint8_t **out_data, uint32_t* out_len, uint32_t seq)
{
    esp_err_t ret = ESP_OK;
    MeshLite__Data* req = NULL;

    *out_len = 0;
    if (esp_mesh_lite_get_level() <= ROOT) {
        return ESP_ERR_NOT_SUPPORTED;
    }

    req = mesh_lite__data__unpack(NULL, len, data);
    if (req) {
        if (req->n_nodes > 0) {
            MeshLite__NodeData** node_data = req->nodes;
            xSemaphoreTake(node_info_mutex, portMAX_DELAY);
            node_info_list_t* current = node_info_list;
            while (current) {
                current->ttl = MESH_LITE_REPORT_INTERVAL_BUFFER;
                current = current->next;
            }
            xSemaphoreGive(node_info_mutex);
            for (uint32_t loop = 0; loop < req->n_nodes; loop++) {
                if (node_data[loop]->node_mac.len > 0) {
                    ret = esp_mesh_lite_node_info_update(node_data[loop]->node_level, node_data[loop]->node_mac.data, node_data[loop]->node_ip);
                    if ((ret != ESP_ERR_DUPLICATE_ADDITION) && (ret != ESP_OK)) {
                        ret = ESP_FAIL;
                        break;
                    }
                }
            }
            xSemaphoreTake(node_info_mutex, portMAX_DELAY);
            current = node_info_list;
            node_info_list_t* prev = NULL;

            while (current) {
                if (current->ttl <= MESH_LITE_REPORT_INTERVAL_BUFFER) {
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
                    nodes_num--;
                    continue;
                }
                prev = current;
                current = current->next;
            }
            xSemaphoreGive(node_info_mutex);
        }
        mesh_lite__data__free_unpacked(req, NULL);
    }

    esp_mesh_lite_msg_config_t config = {
        .raw_msg = {
            .msg_id = MESH_LITE_MSG_ID_UPDATE_NODES_LIST,
            .expect_resp_msg_id = 0,
            .max_retry = 0,
            .data = data,
            .size = len,
            .raw_resend = esp_mesh_lite_send_broadcast_raw_msg_to_child,
        },
    };
    esp_mesh_lite_send_msg(ESP_MESH_LITE_RAW_MSG, &config);
    return ret;
}

static esp_err_t mesh_lite_report_nodes_resp_handler(uint8_t *data, uint32_t len, uint8_t **out_data, uint32_t* out_len, uint32_t seq)
{
    return ESP_OK;
}

static const esp_mesh_lite_raw_msg_action_t raw_msgs_action[] = {
    /* Report information to the root node */
    {MESH_LITE_MSG_ID_REPORT_NODE_INFO, MESH_LITE_MSG_ID_REPORT_NODE_INFO_RESP, mesh_lite_report_nodes_handler},
    {MESH_LITE_MSG_ID_REPORT_NODE_INFO_RESP, 0, mesh_lite_report_nodes_resp_handler},

    {MESH_LITE_MSG_ID_UPDATE_NODES_LIST, 0, mesh_lite_update_nodes_list},
    {0, 0, NULL}
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
            nodes_num--;
            continue;
        } else {
            current->ttl--;
        }
        prev = current;
        current = current->next;
    }
    xSemaphoreGive(node_info_mutex);
}

static esp_err_t esp_mesh_lite_update_nodes_info_to_children(void)
{
    MeshLite__Data req;
    uint32_t total_num = 0;

    mesh_lite__data__init(&req);
    const node_info_list_t *node = esp_mesh_lite_get_nodes_list(&total_num);
    if (total_num > 0) {
        uint32_t loop = 0;
        req.nodes = malloc(total_num * sizeof(MeshLite__NodeData*));
        for (loop = 0; (loop < total_num) && (node != NULL); loop++) {
            req.nodes[loop] = malloc(sizeof(MeshLite__NodeData));
            mesh_lite__node_data__init(req.nodes[loop]);
            req.nodes[loop]->node_level = node->node->level;
            req.nodes[loop]->node_ip = node->node->ip_addr;
            req.nodes[loop]->node_mac.len = ETH_HWADDR_LEN;
            req.nodes[loop]->node_mac.data = malloc(ETH_HWADDR_LEN);
            memcpy(req.nodes[loop]->node_mac.data, node->node->mac_addr, ETH_HWADDR_LEN);
            node = node->next;
        }
        req.n_nodes = loop;
        size_t outlen = mesh_lite__data__get_packed_size(&req);
        uint8_t* outdata = malloc(outlen);
        mesh_lite__data__pack(&req, outdata);
        for (uint32_t i = 0; i < loop; i++) {
            free(req.nodes[i]->node_mac.data);
            free(req.nodes[i]);
        }
        free(req.nodes);

        esp_mesh_lite_msg_config_t config = {
            .raw_msg = {
                .msg_id = MESH_LITE_MSG_ID_UPDATE_NODES_LIST,
                .expect_resp_msg_id = 0,
                .max_retry = 3,
                .data = outdata,
                .size = outlen,
                .raw_resend = esp_mesh_lite_send_broadcast_raw_msg_to_child,
            },
        };
        esp_mesh_lite_send_msg(ESP_MESH_LITE_RAW_MSG, &config);
        free(outdata);
    }
    return ESP_OK;
}

static void report_timer_cb(TimerHandle_t timer)
{
    esp_mesh_lite_report_info();

    if (esp_mesh_lite_get_level() == ROOT) {
        esp_mesh_lite_update_nodes_info_to_children();
    }
}

static void esp_mesh_lite_event_got_ip_handler(void *arg, esp_event_base_t event_base,
                                               int32_t event_id, void *event_data)
{
    esp_mesh_lite_report_info();
}

static void esp_mesh_lite_event_ap_sta_ip_assigned_handler(void *arg, esp_event_base_t event_base,
                                                           int32_t event_id, void *event_data)
{
    esp_mesh_lite_report_info();
}
#endif // CONFIG_MESH_LITE_NODE_INFO_REPORT

uint32_t esp_mesh_lite_get_mesh_node_number(void)
{
#ifdef CONFIG_MESH_LITE_NODE_INFO_REPORT
    return nodes_num;
#else
    return 0;
#endif
}

static void esp_mesh_lite_event_sta_lost_ip_handler(void *arg, esp_event_base_t event_base,
                                                    int32_t event_id, void *event_data)
{
    wifi_ap_record_t ap_info;
    if (esp_wifi_sta_get_ap_info(&ap_info) == ESP_OK) {
        ESP_LOGW(TAG, "STA lost IP, reconnecting");
        esp_mesh_lite_connect();
    }
}

static void esp_mesh_lite_event_ip_changed_handler(void *arg, esp_event_base_t event_base,
                                                   int32_t event_id, void *event_data)
{
    switch (event_id) {
    case ESP_MESH_LITE_EVENT_CORE_STARTED:
        ESP_LOGI(TAG, "Mesh-Lite connecting");
        esp_mesh_lite_connect();
        break;
    case ESP_MESH_LITE_EVENT_CORE_INHERITED_NET_SEGMENT_CHANGED:
        ESP_LOGI(TAG, "netif network segment conflict check");
        if (esp_mesh_lite_get_level() > CONFIG_MESH_LITE_MAXIMUM_LEVEL_ALLOWED) {
            esp_wifi_deauth_sta(0);
            ESP_LOGW(TAG, "The Mesh connection for the current node has exceeded the maximum limit, deauthenticating child node and disconnecting Wi-Fi to search for a new parent node.");
            esp_mesh_lite_connect();
        }
        esp_bridge_netif_network_segment_conflict_update(NULL);
        break;
    case ESP_MESH_LITE_EVENT_CORE_ROUTER_INFO_CHANGED:
        break;
    case ESP_MESH_LITE_EVENT_OTA_START:
        ESP_LOGI(TAG, "OTA Start");
        break;
    case ESP_MESH_LITE_EVENT_OTA_FINISH: {
        esp_mesh_lite_event_ota_finish_t *event = (esp_mesh_lite_event_ota_finish_t*)event_data;
        if (event->reason == ESP_MESH_LITE_EVENT_OTA_SUCCESS) {
            ESP_LOGI(TAG, "LAN OTA Success!");
#ifdef CONFIG_OTA_AUTO_RESTART
            esp_restart();
#endif
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
    esp_event_handler_instance_register(IP_EVENT, IP_EVENT_STA_LOST_IP, &esp_mesh_lite_event_sta_lost_ip_handler, NULL, NULL);

    esp_mesh_lite_espnow_init();

    esp_mesh_lite_core_init(config);
#if CONFIG_MESH_LITE_NODE_INFO_REPORT
    esp_event_handler_instance_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &esp_mesh_lite_event_got_ip_handler, NULL, NULL);
    esp_event_handler_instance_register(IP_EVENT, IP_EVENT_AP_STAIPASSIGNED, &esp_mesh_lite_event_ap_sta_ip_assigned_handler, NULL, NULL);

    node_info_mutex = xSemaphoreCreateMutex();

    esp_mesh_lite_raw_msg_action_list_register(raw_msgs_action);

    TimerHandle_t report_timer = xTimerCreate("report_timer", CONFIG_MESH_LITE_REPORT_INTERVAL * 1000 / portTICK_PERIOD_MS,
                                              pdTRUE, NULL, report_timer_cb);
    TimerHandle_t root_timer = xTimerCreate("root_timer", 1 * 1000 / portTICK_PERIOD_MS,
                                            pdTRUE, NULL, root_timer_cb);
    xTimerStart(report_timer, portMAX_DELAY);
    xTimerStart(root_timer, portMAX_DELAY);

    uint8_t mac[6];
    esp_wifi_get_mac(WIFI_IF_STA, mac);
    esp_mesh_lite_node_info_update(0, mac, 0);
#endif /* MESH_LITE_NODE_INFO_REPORT */

#if CONFIG_MESH_LITE_WIRELESS_DEBUG
    esp_mesh_lite_wireless_debug_init();
#endif

#if CONFIG_OTA_AUTO_CANCEL_ROLLBACK
    const esp_partition_t *running = esp_ota_get_running_partition();
    esp_ota_img_states_t ota_state;
    if (esp_ota_get_state_partition(running, &ota_state) == ESP_OK) {
        if (ota_state == ESP_OTA_IMG_PENDING_VERIFY) {
            esp_ota_mark_app_valid_cancel_rollback();
        }
    }
#endif
}
