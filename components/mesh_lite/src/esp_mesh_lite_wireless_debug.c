/*
 * SPDX-FileCopyrightText: 2023-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <string.h>
#include <inttypes.h>
#include <stdarg.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "esp_mac.h"
#include "esp_wifi.h"
#include "esp_rom_crc.h"
#include "esp_mesh_lite.h"
#include "esp_mesh_lite_log.h"

#include "argtable3/argtable3.h"
#include "esp_console.h"

#define WIRELESS_DEBUG_VERSION                           (0x01)
#define CRC_INIT_VALUE                                   (0xFFFFFFFF)

typedef struct {
    uint8_t version: 4;                   // version
    uint8_t channel: 4;                   // the channel of sender
    uint8_t mesh_id;                      // Mesh ID of ESPNOW data
    uint32_t seq;                         // Magic number which is used to determine which device to send unicast ESPNOW data.
    bool is_rsp_payload;                  // Is it the payload of the response?
    uint8_t payload[0];                   // Real payload of ESPNOW data.
} __attribute__((packed)) wireless_debug_data_t;

typedef struct {
    uint32_t crc32;
    char data[0];
} __attribute__((packed)) wireless_debug_log_t;

static const char *TAG = "Mesh-Lite-Wireless-Debug";

#define LOG_COLOR_LEN                   (8)
#define WIRELESS_DEBUG_QUEUE_SIZE       (10)
#define RESPONSE_DELAY_TIME_TIME_OUT    (3000)

static char *output_buffer = NULL;
static char *command_payload = NULL;
static wireless_debug_log_t *debug_log_buffer = NULL;

typedef struct {
    struct arg_int *channel;
    struct arg_str *mac;
    struct arg_int *delay_time_ms;
    struct arg_end *end;
} base_args_t;

typedef struct {
    struct arg_int *level;
    struct arg_int *onoff;
    base_args_t base_args;
} core_log_args_t;

static base_args_t base_args;
static core_log_args_t core_log_args;
static esp_mesh_lite_wireless_debug_cb_list_t cb_list;

static uint8_t last_dst_mac[6];
static uint8_t last_response_channel;

static TaskHandle_t mesh_lite_wireless_debug_task_handle = NULL;
static QueueHandle_t mesh_lite_wireless_debug_queue_handle = NULL;

static esp_err_t wireless_debug_espnow_create_peer(uint8_t *dst_mac, uint8_t channel)
{
    esp_err_t ret = ESP_FAIL;
    esp_now_peer_info_t *peer = malloc(sizeof(esp_now_peer_info_t));
    if (peer == NULL) {
        ESP_LOGE(TAG, "Malloc peer information fail");
        return ESP_ERR_NO_MEM;
    }
    memset(peer, 0, sizeof(esp_now_peer_info_t));

    esp_now_get_peer(dst_mac, peer);
    peer->channel = channel;
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

static bool str_2_mac(uint8_t *str, uint8_t *dest)
{
    uint8_t loop = 0;
    uint8_t tmp = 0;
    uint8_t *src_p = str;

    if (strlen((char *)src_p) != 17) { // must be like 12:34:56:78:90:AB
        return false;
    }

    for (loop = 0; loop < 17 ; loop++) {
        if (loop % 3 == 2) {
            if (src_p[loop] != ':') {
                return false;
            }
            continue;
        }

        if ((src_p[loop] >= '0') && (src_p[loop] <= '9')) {
            tmp = tmp * 16 + src_p[loop] - '0';
        } else if ((src_p[loop] >= 'A') && (src_p[loop] <= 'F')) {
            tmp = tmp * 16 + src_p[loop] - 'A' + 10;
        } else if ((src_p[loop] >= 'a') && (src_p[loop] <= 'f')) {
            tmp = tmp * 16 + src_p[loop] - 'a' + 10;
        } else {
            return false;
        }

        if (loop % 3 == 1) {
            *dest++ = tmp;
            tmp = 0;
        }
    }

    return true;
}

static void wireless_debug_log_writev(esp_log_level_t level, const char *tag, const char *format, ...)
{
    char letter = 'I';
    char log_color[LOG_COLOR_LEN] = {0};
    memset(log_color, 0, sizeof(log_color));
    switch (level) {
    case ESP_LOG_ERROR:
        letter = 'E';
#ifdef CONFIG_LOG_COLORS
        memcpy(log_color, "\033[0;" "31" "m", LOG_COLOR_LEN);
#endif
        break;
    case ESP_LOG_WARN:
        letter = 'W';
#ifdef CONFIG_LOG_COLORS
        memcpy(log_color, "\033[0;" "33" "m", LOG_COLOR_LEN);
#endif
        break;
    case ESP_LOG_INFO:
        letter = 'I';
#ifdef CONFIG_LOG_COLORS
        memcpy(log_color, "\033[0;" "32" "m", LOG_COLOR_LEN);
#endif
        break;
    case ESP_LOG_DEBUG:
        letter = 'D';
#ifdef CONFIG_LOG_COLORS
        memcpy(log_color, "\033[0;" "36" "m", LOG_COLOR_LEN);
#endif
        break;
    case ESP_LOG_VERBOSE:
        letter = 'V';
        break;
    default:
        break;
    }
    va_list list;
    va_start(list, format);

    size_t new_format_length = strlen(tag) + strlen(format) + 30;
    char new_format[new_format_length];
    snprintf(new_format, new_format_length, "%s%c (%"PRIu32") [%s]: %s " LOG_RESET_COLOR "\n", log_color, letter, esp_log_timestamp(), tag, format);
    vsnprintf(debug_log_buffer->data, ESPNOW_PAYLOAD_MAX_LEN, new_format, list);

    va_end(list);

    debug_log_buffer->crc32 = esp_rom_crc32_le(CRC_INIT_VALUE, (uint8_t*)debug_log_buffer->data, strlen(debug_log_buffer->data));

    esp_err_t ret = wireless_debug_espnow_create_peer(last_dst_mac, last_response_channel);
    if (ret == ESP_OK) {
        ret = esp_mesh_lite_espnow_send(ESPNOW_DATA_TYPE_WIRELESS_LOG, last_dst_mac, (uint8_t*)debug_log_buffer, sizeof(wireless_debug_log_t) + strlen(debug_log_buffer->data));
        if (ret != ESP_OK) {
            ESP_LOGE(TAG, "Wireless log Send error: %d\r\n", ret);
        }
    }
}

static int wireless_debug_cmd_discover(int argc, char **argv)
{
    int nerrors = arg_parse(argc, argv, (void **) &base_args);
    if (nerrors != 0) {
        arg_print_errors(stderr, base_args.end, argv[0]);
        return 1;
    }

    if (base_args.channel->count > 0) {
        uint8_t channel = base_args.channel->ival[0];
        if ((channel > 0) && (channel <= 14)) {
            last_response_channel = base_args.channel->ival[0];
            ESP_LOGI(TAG, "channel: %d", channel);
        }
    }

    if (base_args.mac->count > 0) {
        str_2_mac((uint8_t*)base_args.mac->sval[0], last_dst_mac);
        ESP_LOGI(TAG, "mac:" MACSTR "",  MAC2STR(last_dst_mac));
    } else {
        ESP_LOGW(TAG, "Not provided mac, use last mac "MACSTR"", MAC2STR(last_dst_mac));
    }

    uint8_t sta_mac[6];
    esp_wifi_get_mac(WIFI_IF_STA, sta_mac);

    if (memcmp(sta_mac, last_dst_mac, sizeof(sta_mac)) == 0) {
        wifi_ap_record_t ap_info;
        esp_wifi_sta_get_ap_info(&ap_info);
        snprintf(output_buffer, ESPNOW_PAYLOAD_MAX_LEN, "discover:"MACSTR",%d.", MAC2STR(sta_mac), ap_info.primary);

        if (base_args.delay_time_ms->count > 0) {
            uint32_t delay_time_ms = base_args.delay_time_ms->ival[0] < RESPONSE_DELAY_TIME_TIME_OUT ? base_args.delay_time_ms->ival[0] : RESPONSE_DELAY_TIME_TIME_OUT;
            vTaskDelay(delay_time_ms / portTICK_PERIOD_MS);
        }
    } else {
        ESP_LOGW(TAG, "STA MAC does not match with last_dst_mac");
    }

    return 0;
}

static int wireless_debug_cmd_wifi_error(int argc, char **argv)
{
    int nerrors = arg_parse(argc, argv, (void **) &base_args);
    if (nerrors != 0) {
        arg_print_errors(stderr, base_args.end, argv[0]);
        return 1;
    }

    if (base_args.channel->count > 0) {
        uint8_t channel = base_args.channel->ival[0];
        if ((channel > 0) && (channel <= 14)) {
            last_response_channel = base_args.channel->ival[0];
            ESP_LOGI(TAG, "channel: %d", channel);
        }
    }

    if (base_args.mac->count > 0) {
        str_2_mac((uint8_t*)base_args.mac->sval[0], last_dst_mac);
        ESP_LOGI(TAG, "mac:" MACSTR "",  MAC2STR(last_dst_mac));
    } else {
        ESP_LOGW(TAG, "Not provided mac, use last mac "MACSTR"", MAC2STR(last_dst_mac));
    }

    uint8_t sta_mac[6];
    esp_wifi_get_mac(WIFI_IF_STA, sta_mac);

    if (memcmp(sta_mac, last_dst_mac, sizeof(sta_mac)) == 0) {
        if (base_args.delay_time_ms->count > 0) {
            uint32_t delay_time_ms = base_args.delay_time_ms->ival[0] < RESPONSE_DELAY_TIME_TIME_OUT ? base_args.delay_time_ms->ival[0] : RESPONSE_DELAY_TIME_TIME_OUT;
            vTaskDelay(delay_time_ms / portTICK_PERIOD_MS);
        }

        if (cb_list.wifi_error_cb) {
            char *error_info = cb_list.wifi_error_cb();
            snprintf(output_buffer, ESPNOW_PAYLOAD_MAX_LEN, "wifi_error:%s", error_info);
        }
    } else {
        ESP_LOGW(TAG, "STA MAC does not match with last_dst_mac");
    }

    return 0;
}

static int wireless_debug_cmd_cloud_error(int argc, char **argv)
{
    int nerrors = arg_parse(argc, argv, (void **) &base_args);
    if (nerrors != 0) {
        arg_print_errors(stderr, base_args.end, argv[0]);
        return 1;
    }

    if (base_args.channel->count > 0) {
        uint8_t channel = base_args.channel->ival[0];
        if ((channel > 0) && (channel <= 14)) {
            last_response_channel = base_args.channel->ival[0];
            ESP_LOGI(TAG, "channel: %d", channel);
        }
    }

    if (base_args.mac->count > 0) {
        str_2_mac((uint8_t*)base_args.mac->sval[0], last_dst_mac);
        ESP_LOGI(TAG, "mac:" MACSTR "",  MAC2STR(last_dst_mac));
    } else {
        ESP_LOGW(TAG, "Not provided mac, use last mac "MACSTR"", MAC2STR(last_dst_mac));
    }

    uint8_t sta_mac[6];
    esp_wifi_get_mac(WIFI_IF_STA, sta_mac);

    if (memcmp(sta_mac, last_dst_mac, sizeof(sta_mac)) == 0) {
        if (base_args.delay_time_ms->count > 0) {
            uint32_t delay_time_ms = base_args.delay_time_ms->ival[0] < RESPONSE_DELAY_TIME_TIME_OUT ? base_args.delay_time_ms->ival[0] : RESPONSE_DELAY_TIME_TIME_OUT;
            vTaskDelay(delay_time_ms / portTICK_PERIOD_MS);
        }

        if (cb_list.cloud_error_cb) {
            char *error_info = cb_list.cloud_error_cb();
            snprintf(output_buffer, ESPNOW_PAYLOAD_MAX_LEN, "cloud_error:%s", error_info);
        }
    } else {
        ESP_LOGW(TAG, "STA MAC does not match with last_dst_mac");
    }

    return 0;
}

static int wireless_debug_cmd_core_log(int argc, char **argv)
{
    int nerrors = arg_parse(argc, argv, (void **) &core_log_args);
    if (nerrors != 0) {
        arg_print_errors(stderr, core_log_args.base_args.end, argv[0]);
        return 1;
    }

    uint8_t onoff = 0;
    uint8_t level = 0;

    if (core_log_args.base_args.channel->count > 0) {
        uint8_t channel = core_log_args.base_args.channel->ival[0];
        if ((channel > 0) && (channel <= 14)) {
            last_response_channel = core_log_args.base_args.channel->ival[0];
            ESP_LOGI(TAG, "channel: %d", channel);
        }
    }

    if (core_log_args.base_args.mac->count > 0) {
        str_2_mac((uint8_t*)core_log_args.base_args.mac->sval[0], last_dst_mac);
        ESP_LOGI(TAG, "mac: "MACSTR"", MAC2STR(last_dst_mac));
    } else {
        ESP_LOGW(TAG, "Not provided mac, use last mac "MACSTR"", MAC2STR(last_dst_mac));
    }

    if (core_log_args.onoff->count > 0) {
        onoff = core_log_args.onoff->ival[0];
        ESP_LOGI(TAG, "onoff: %d", onoff);
    }

    if (core_log_args.level->count > 0) {
        level = core_log_args.level->ival[0];
        ESP_LOGI(TAG, "level: %d", level);
    }

    if ((onoff != 0) && (onoff != 1)) {
        return -1;
    }

    if (level == 1) {
        esp_mesh_lite_set_wireless_debug_log_writev(wireless_debug_log_writev);
    } else if (level == 0) {
        esp_mesh_lite_set_wireless_debug_log_writev(NULL);
    } else {
        return -1;
    }

    esp_mesh_lite_core_log_enable(onoff);

    snprintf(output_buffer, ESPNOW_PAYLOAD_MAX_LEN, "core_log:OK");
    return 0;
}

static int wireless_debug_cmd_reboot(int argc, char **argv)
{
    esp_restart();
    return 0;
}

static void initialize_console(void)
{
    /* Initialize the console */
    esp_console_config_t console_config = {
        .max_cmdline_args = 10,
        .max_cmdline_length = ESPNOW_PAYLOAD_MAX_LEN,
#if CONFIG_LOG_COLORS
        .hint_color = atoi(LOG_COLOR_CYAN)
#endif
    };
    ESP_ERROR_CHECK(esp_console_init(&console_config));

    esp_console_cmd_t command;

    base_args.channel = arg_int1("c", "channel", "<channel>", "Which channel we should respond");
    base_args.mac = arg_str1(NULL, "mac", "<mac>", "the device mac to be discovered");
    base_args.delay_time_ms = arg_int1(NULL, "delay", "<delay>", "Reply needs to be delayed(ms)");
    base_args.end = arg_end(0);
    memset(&command, 0x0, sizeof(command));
    command.command = "discover";
    command.help = "Discover the device info";
    command.func = &wireless_debug_cmd_discover;
    command.argtable = &base_args;
    esp_console_cmd_register(&command);

    base_args.channel = arg_int1("c", "channel", "<channel>", "Which channel we should respond");
    base_args.mac = arg_str1(NULL, "mac", "<mac>", "the device mac to be discovered");
    base_args.delay_time_ms = arg_int1(NULL, "delay", "<delay>", "Reply needs to be delayed(ms)");
    base_args.end = arg_end(0);
    memset(&command, 0x0, sizeof(command));
    command.command = "wifi_error";
    command.help = "Print the last reason error that device failed to connect wifi";
    command.func = &wireless_debug_cmd_wifi_error;
    command.argtable = &base_args;
    esp_console_cmd_register(&command);

    base_args.channel = arg_int1("c", "channel", "<channel>", "Which channel we should respond");
    base_args.mac = arg_str1(NULL, "mac", "<mac>", "the device mac to be discovered");
    base_args.delay_time_ms = arg_int1(NULL, "delay", "<delay>", "Reply needs to be delayed(ms)");
    base_args.end = arg_end(0);
    memset(&command, 0x0, sizeof(command));
    command.command = "cloud_error";
    command.help = "Print the last reason error that device failed to connect cloud";
    command.func = &wireless_debug_cmd_cloud_error;
    command.argtable = &base_args;
    esp_console_cmd_register(&command);

    core_log_args.onoff = arg_int1(NULL, "onoff", "<Close(0)|On(1)>", "Open/Close core log");
    core_log_args.level = arg_int1(NULL, "level", "<0|1>", "core log level");
    core_log_args.base_args.channel = arg_int1("c", "channel", "<channel>", "Which channel we should respond");
    core_log_args.base_args.mac = arg_str1(NULL, "mac", "<mac>", "Which device we should respond");
    core_log_args.base_args.delay_time_ms = arg_int1(NULL, "delay", "<delay>", "Reply needs to be delayed(ms)");
    core_log_args.base_args.end = arg_end(0);
    memset(&command, 0x0, sizeof(command));
    command.command = "core_log";
    command.help = "Open/Close mesh lite core log";
    command.func = &wireless_debug_cmd_core_log;
    command.argtable = &core_log_args;
    esp_console_cmd_register(&command);

    memset(&command, 0x0, sizeof(command));
    command.command = "reboot";
    command.help = "Reboot the device";
    command.func = &wireless_debug_cmd_reboot;
    esp_console_cmd_register(&command);
}

static void esp_mesh_lite_wireless_debug_task(void *pvParameter)
{
    esp_mesh_lite_espnow_event_t evt;

    mesh_lite_wireless_debug_queue_handle = xQueueCreate(WIRELESS_DEBUG_QUEUE_SIZE, sizeof(esp_mesh_lite_espnow_event_t));
    if (mesh_lite_wireless_debug_queue_handle == NULL) {
        ESP_LOGE(TAG, "Create mutex fail");
        vTaskDelete(NULL);
        return;
    }

    initialize_console();

    while (xQueueReceive(mesh_lite_wireless_debug_queue_handle, &evt, portMAX_DELAY) == pdTRUE) {
        switch (evt.id) {
        case ESPNOW_RECV_CB:
            espnow_recv_cb_t *recv_cb = &evt.info.recv_cb;
            wireless_debug_data_t *data = (wireless_debug_data_t *)recv_cb->data;

            memset(command_payload, 0x0, ESPNOW_PAYLOAD_MAX_LEN);
            if (data->is_rsp_payload) {
                memcpy(command_payload, data->payload, recv_cb->data_len - sizeof(wireless_debug_data_t));
                if (cb_list.recv_resp_data_cb) {
                    cb_list.recv_resp_data_cb(command_payload, strlen(command_payload));
                }
            } else {
                int ret;
                memset(output_buffer, 0x0, ESPNOW_PAYLOAD_MAX_LEN);
                memcpy(command_payload, data->payload, recv_cb->data_len - sizeof(wireless_debug_data_t));
                ESP_LOGI(TAG, "recv cmd:%s", command_payload);
                esp_err_t err = esp_console_run(command_payload, &ret);

                if (err == ESP_OK) {
                    if (strlen(output_buffer)) {
                        wireless_debug_data_t *rsp_data = malloc(sizeof(wireless_debug_data_t) + strlen(output_buffer) + 1);
                        if (rsp_data) {
                            memcpy(rsp_data->payload, output_buffer, strlen(output_buffer));
                            rsp_data->payload[strlen(output_buffer)] = '\0';
                            ESP_LOGI(TAG, "response data:%s", rsp_data->payload);
                            if (last_response_channel) {
                                rsp_data->channel = last_response_channel;
                            }
                            rsp_data->version = WIRELESS_DEBUG_VERSION;
                            rsp_data->mesh_id = esp_mesh_lite_get_mesh_id();
                            rsp_data->is_rsp_payload = true;
                            ret = wireless_debug_espnow_create_peer(recv_cb->mac_addr, last_response_channel);
                            if (ret == ESP_OK) {
                                ret = esp_mesh_lite_espnow_send(ESPNOW_DATA_TYPE_WIRELESS_DEBUG, recv_cb->mac_addr, (const uint8_t*)rsp_data, sizeof(wireless_debug_data_t) + strlen(output_buffer));
                                if (ret != ESP_OK) {
                                    ESP_LOGE(TAG, "Send error: %d [%s %d]", ret, __func__, __LINE__);
                                }
                            }
                            free(rsp_data);
                        }
                    }
                    ESP_LOGI(TAG, "Run command\n");
                } else if (err == ESP_ERR_NOT_FOUND) {
                    ESP_LOGI(TAG, "Unrecognized command\n");
                } else if (err == ESP_ERR_INVALID_ARG) {
                    // command was empty
                    ESP_LOGI(TAG, "command was empty\n");
                } else if (err == ESP_OK && ret != ESP_OK) {
                    ESP_LOGI(TAG, "Command returned non-zero error code: 0x%x (%s)\n", ret, esp_err_to_name(ret));
                } else if (err != ESP_OK) {
                    ESP_LOGI(TAG, "Internal error: %s\n", esp_err_to_name(err));
                }
            }
            free(recv_cb->data);
            recv_cb->data = NULL;
            break;
        default:
            ESP_LOGE(TAG, "Callback type error: %d", evt.id);
            break;
        }
    }
    esp_console_deinit();
    mesh_lite_wireless_debug_task_handle = NULL;
    vTaskDelete(mesh_lite_wireless_debug_task_handle);
}

esp_err_t esp_mesh_lite_wireless_debug_send_command(uint8_t *dst_mac, char *command, size_t command_len, uint8_t channel)
{
    if (mesh_lite_wireless_debug_task_handle == NULL) {
        return ESP_FAIL;
    }

    esp_err_t ret = wireless_debug_espnow_create_peer(dst_mac, channel);
    if (ret != ESP_OK) {
        return ret;
    }

    uint16_t length = sizeof(wireless_debug_data_t) + command_len + 1;
    wireless_debug_data_t *pbuf = (wireless_debug_data_t *)malloc(length);
    if (pbuf == NULL) {
        ESP_LOGE(TAG, "Malloc unicast buff fail");
        return ESP_ERR_NO_MEM;
    }
    memset(pbuf, 0, length);
    pbuf->version = WIRELESS_DEBUG_VERSION;
    pbuf->channel = 11;
    pbuf->is_rsp_payload = false;
    pbuf->mesh_id = esp_mesh_lite_get_mesh_id();
    strcpy((char*)pbuf->payload, command);
    ESP_LOGI(TAG, "send command: %s", pbuf->payload);

    ret = esp_mesh_lite_espnow_send(ESPNOW_DATA_TYPE_WIRELESS_DEBUG, dst_mac, (const uint8_t *)pbuf, length);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Send error");
    }
    free(pbuf);
    return ret;
}

static esp_err_t wireless_log_process_cb(const esp_now_recv_info_t *recv_info, const uint8_t *data, int len)
{
    if (mesh_lite_wireless_debug_task_handle == NULL) {
        return ESP_FAIL;
    }

    if (recv_info == NULL || data == NULL || len <= 0) {
        ESP_LOGD(TAG, "Receive cb arg error");
        return ESP_FAIL;
    }

    wireless_debug_log_t *wireless_debug_log = (wireless_debug_log_t *)data;
    size_t data_len = strlen(wireless_debug_log->data);
    if (wireless_debug_log->crc32 != esp_rom_crc32_le(CRC_INIT_VALUE, (uint8_t*)wireless_debug_log->data, data_len)) {
        return ESP_FAIL;
    }

    if (cb_list.recv_debug_log_cb) {
        cb_list.recv_debug_log_cb(recv_info, (uint8_t*)wireless_debug_log->data, data_len);
    }

    return ESP_OK;
}

static esp_err_t wireless_debug_data_parse(const uint8_t *data, uint16_t data_len)
{
    wireless_debug_data_t *wireless_debug_data = (wireless_debug_data_t *)data;

    if ((data_len < sizeof(wireless_debug_data_t)) || (data_len > ESPNOW_PAYLOAD_MAX_LEN)) {
        ESP_LOGD(TAG, "Received the wrong data len, len:%d", data_len);
        return ESP_FAIL;
    }

    if (wireless_debug_data->version != WIRELESS_DEBUG_VERSION) {
        return ESP_FAIL;
    }

    if (wireless_debug_data->mesh_id != esp_mesh_lite_get_mesh_id()) {
        return ESP_FAIL;
    }

    return ESP_OK;
}

static esp_err_t wireless_debug_process_cb(const esp_now_recv_info_t *recv_info, const uint8_t *data, int len)
{
    if (mesh_lite_wireless_debug_task_handle == NULL) {
        return ESP_FAIL;
    }

    esp_mesh_lite_espnow_event_t evt;
    espnow_recv_cb_t *recv_cb = &evt.info.recv_cb;
    uint8_t *mac_addr = (uint8_t *)recv_info->src_addr;

    if (mac_addr == NULL || data == NULL || len <= 0) {
        ESP_LOGD(TAG, "Receive cb arg error");
        return ESP_FAIL;
    }

    if (wireless_debug_data_parse(data, len) != ESP_OK) {
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

    if (!mesh_lite_wireless_debug_queue_handle || xQueueSend(mesh_lite_wireless_debug_queue_handle, &evt, ESPNOW_MAXDELAY) != pdTRUE) {
        ESP_LOGW(TAG, "Send receive queue fail");
        free(recv_cb->data);
        recv_cb->data = NULL;
        return ESP_FAIL;
    }

    return ESP_OK;
}

void esp_mesh_lite_wireless_debug_cb_register(esp_mesh_lite_wireless_debug_cb_list_t *cb)
{
    cb_list.wifi_error_cb = cb->wifi_error_cb;
    cb_list.cloud_error_cb = cb->cloud_error_cb;
    cb_list.recv_resp_data_cb = cb->recv_resp_data_cb;
    cb_list.recv_debug_log_cb = cb->recv_debug_log_cb;
}

void esp_mesh_lite_wireless_debug_cb_unregister(esp_mesh_lite_wireless_debug_cb_list_t *cb)
{
    cb_list.wifi_error_cb = NULL;
    cb_list.cloud_error_cb = NULL;
    cb_list.recv_resp_data_cb = NULL;
    cb_list.recv_debug_log_cb = NULL;
}

void esp_mesh_lite_wireless_debug_init(void)
{
    if (mesh_lite_wireless_debug_task_handle != NULL) {
        return;
    }

    output_buffer = (char*)malloc(ESPNOW_PAYLOAD_MAX_LEN);
    command_payload = (char*)malloc(ESPNOW_PAYLOAD_MAX_LEN);
    debug_log_buffer = (wireless_debug_log_t*)malloc(ESPNOW_PAYLOAD_MAX_LEN);
    memset(output_buffer, 0x0, ESPNOW_PAYLOAD_MAX_LEN);
    memset(command_payload, 0x0, ESPNOW_PAYLOAD_MAX_LEN);
    memset(debug_log_buffer, 0x0, ESPNOW_PAYLOAD_MAX_LEN);
    memset(last_dst_mac, 0x0, 6);
    last_response_channel = 0;

    esp_mesh_lite_espnow_recv_cb_register(ESPNOW_DATA_TYPE_WIRELESS_LOG, wireless_log_process_cb);
    esp_mesh_lite_espnow_recv_cb_register(ESPNOW_DATA_TYPE_WIRELESS_DEBUG, wireless_debug_process_cb);

    xTaskCreate(esp_mesh_lite_wireless_debug_task, "mesh_lite_wireless_debug", 1024 * 6, NULL, 2, &mesh_lite_wireless_debug_task_handle);
}

void esp_mesh_lite_wireless_debug_deinit(void)
{
    if (mesh_lite_wireless_debug_task_handle != NULL) {
        vTaskDelete(mesh_lite_wireless_debug_task_handle);
        mesh_lite_wireless_debug_task_handle = NULL;
    }

    if (output_buffer != NULL) {
        free(output_buffer);
        output_buffer = NULL;
    }

    if (command_payload != NULL) {
        free(command_payload);
        command_payload = NULL;
    }

    if (debug_log_buffer != NULL) {
        free(debug_log_buffer);
        debug_log_buffer = NULL;
    }

    esp_mesh_lite_espnow_recv_cb_unregister(ESPNOW_DATA_TYPE_WIRELESS_LOG);
    esp_mesh_lite_espnow_recv_cb_unregister(ESPNOW_DATA_TYPE_WIRELESS_DEBUG);
}
