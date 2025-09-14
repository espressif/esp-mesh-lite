/*
 * Leaf node: UART (GPIO16/17) -> UDP :3333 over Mesh-Lite
 * SPDX-License-Identifier: Apache-2.0
*/

#include <inttypes.h>
#include <sys/socket.h>
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/timers.h"
#include "nvs_flash.h"
#include "esp_wifi.h"
#include "esp_mac.h"
#include "esp_bridge.h"
#include "esp_mesh_lite.h"
#include "lwip/inet.h"
#include "lwip/sockets.h"
#include "driver/uart.h"

#define FORCE_ROOT 0   // child

// UART2 on S3 default pins (change if you wired differently)
#define UART_PORT     UART_NUM_2
#define UART_TX_PIN   17
#define UART_RX_PIN   16
#define UART_BAUD     9600
#define UART_RXBUF_SZ (16 * 1024)
#define LINE_MAX      128

#define UDP_PORT      3333

static const char *TAG = "no_router";

/* ---------- periodic mesh info print ---------- */
static void print_system_info_timercb(TimerHandle_t timer)
{
    uint8_t primary=0, sta_mac[6]={0};
    wifi_ap_record_t ap_info={0};
    wifi_second_chan_t second=0;
    wifi_sta_list_t wifi_sta_list={0};

    if (esp_mesh_lite_get_level() > 1) esp_wifi_sta_get_ap_info(&ap_info);
    esp_wifi_get_mac(ESP_IF_WIFI_STA, sta_mac);
    esp_wifi_ap_get_sta_list(&wifi_sta_list);
    esp_wifi_get_channel(&primary, &second);

    ESP_LOGI(TAG, "System information, channel: %d, layer: %d, self mac: " MACSTR
             ", parent bssid: " MACSTR ", parent rssi: %d, free heap: %" PRIu32,
             primary, esp_mesh_lite_get_level(), MAC2STR(sta_mac), MAC2STR(ap_info.bssid),
             (ap_info.rssi != 0 ? ap_info.rssi : -120), esp_get_free_heap_size());

    uint32_t size=0; const node_info_list_t *node=esp_mesh_lite_get_nodes_list(&size);
    printf("MeshLite nodes %lu:\r\n", (unsigned long)size);
    for (uint32_t i=0; (i<size) && node; i++) {
        struct in_addr ip; ip.s_addr = node->node->ip_addr;
        printf("%lu: %d, " MACSTR ", %s\r\n", (unsigned long)i+1, node->node->level,
               MAC2STR(node->node->mac_addr), inet_ntoa(ip));
        node = node->next;
    }
}

static esp_err_t esp_storage_init(void)
{
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    return ret;
}

static void wifi_init(void)
{
    wifi_config_t wifi_config; memset(&wifi_config, 0, sizeof(wifi_config));
    esp_bridge_wifi_set_config(WIFI_IF_STA, &wifi_config);

    wifi_config_t ap_cfg = {
        .ap = {
            .ssid     = CONFIG_BRIDGE_SOFTAP_SSID,
            .password = CONFIG_BRIDGE_SOFTAP_PASSWORD,
            .channel  = CONFIG_MESH_CHANNEL,
        },
    };
    esp_bridge_wifi_set_config(WIFI_IF_AP, &ap_cfg);
}

void app_wifi_set_softap_info(void)
{
    char ssid[33]; char psw[64]; uint8_t mac[6];
    size_t ssid_sz = sizeof(ssid), psw_sz = sizeof(psw);
    esp_wifi_get_mac(WIFI_IF_AP, mac);
    memset(ssid,0,sizeof(ssid)); memset(psw,0,sizeof(psw));

    if (esp_mesh_lite_get_softap_ssid_from_nvs(ssid,&ssid_sz) != ESP_OK) {
#ifdef CONFIG_BRIDGE_SOFTAP_SSID_END_WITH_THE_MAC
        snprintf(ssid, sizeof(ssid), "%.25s_%02x%02x%02x",
                 CONFIG_BRIDGE_SOFTAP_SSID, mac[3], mac[4], mac[5]);
#else
        snprintf(ssid, sizeof(ssid), "%.32s", CONFIG_BRIDGE_SOFTAP_SSID);
#endif
    }
    if (esp_mesh_lite_get_softap_psw_from_nvs(psw,&psw_sz) != ESP_OK) {
        strlcpy(psw, CONFIG_BRIDGE_SOFTAP_PASSWORD, sizeof(psw));
    }
    esp_mesh_lite_set_softap_info(ssid, psw);
}

/* ---------------------- UART + forward ---------------------- */
static void child_uart_init(void)
{
    const uart_config_t cfg = {
        .baud_rate  = UART_BAUD,
        .data_bits  = UART_DATA_8_BITS,
        .parity     = UART_PARITY_DISABLE,
        .stop_bits  = UART_STOP_BITS_1,
        .flow_ctrl  = UART_HW_FLOWCTRL_DISABLE,
        .source_clk = UART_SCLK_DEFAULT,
    };
    ESP_ERROR_CHECK(uart_driver_install(UART_PORT, UART_RXBUF_SZ, 0, 20, NULL, 0));
    ESP_ERROR_CHECK(uart_param_config(UART_PORT, &cfg));
    ESP_ERROR_CHECK(uart_set_pin(UART_PORT, UART_TX_PIN, UART_RX_PIN,
                                 UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE));
    // newline detection for whole-line reads
    ESP_ERROR_CHECK(uart_enable_pattern_det_baud_intr(UART_PORT, '\n', 1, 9, 0, 0));
    ESP_ERROR_CHECK(uart_pattern_queue_reset(UART_PORT, 20));
    ESP_LOGI(TAG, "UART ready @%d TX=%d RX=%d", UART_BAUD, UART_TX_PIN, UART_RX_PIN);
}

static void child_uart_forward_task(void *arg)
{
    int sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (sock < 0) { ESP_LOGE(TAG, "UDP socket create failed"); vTaskDelete(NULL); return; }
    int yes=1; setsockopt(sock, SOL_SOCKET, SO_BROADCAST, &yes, sizeof(yes));

    struct sockaddr_in dest = {0};
    dest.sin_family = AF_INET;
    dest.sin_port   = htons(UDP_PORT);
    dest.sin_addr.s_addr = inet_addr("255.255.255.255");

    uint8_t line[LINE_MAX];

    for (;;) {
        int pos = uart_pattern_pop_pos(UART_PORT);
        if (pos >= 0) {
            int to_read = pos + 1;
            if (to_read > (int)sizeof(line)-1) to_read = sizeof(line)-1;

            int n = uart_read_bytes(UART_PORT, line, to_read, pdMS_TO_TICKS(20));
            if (n <= 0) continue;

            // Trim CR/LF and null-terminate so it's a real C string
            while (n && (line[n-1] == '\n' || line[n-1] == '\r')) n--;
            line[n] = '\0';

            if (n == 0) continue;

            // SHOW exactly what we will send over the mesh
            ESP_LOGI(TAG, "UART line -> mesh: %s", (char*)line);

            // forward single line
            sendto(sock, line, n, 0, (struct sockaddr*)&dest, sizeof(dest));
        } else {
            // safety drain if buffer is getting full
            size_t pending=0; uart_get_buffered_data_len(UART_PORT,&pending);
            if (pending > (UART_RXBUF_SZ*3/4)) {
                uint8_t dump[256];
                uart_read_bytes(UART_PORT, dump, sizeof(dump), pdMS_TO_TICKS(10));
                ESP_LOGW(TAG, "UART near overflow, draining (%u)", (unsigned)pending);
            }
            vTaskDelay(pdMS_TO_TICKS(5));
        }
    }
}
/* ------------------------------------------------------------ */

void app_main(void)
{
    esp_log_level_set("*", ESP_LOG_INFO);
    esp_storage_init();

    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());

    esp_bridge_create_all_netif();
    wifi_init();

    esp_mesh_lite_config_t cfg = ESP_MESH_LITE_DEFAULT_INIT();
    cfg.join_mesh_ignore_router_status = true;
#if FORCE_ROOT
    cfg.join_mesh_without_configured_wifi = false;
#else
    cfg.join_mesh_without_configured_wifi = true;   // child can join with no router
#endif
    esp_mesh_lite_init(&cfg);
    app_wifi_set_softap_info();

#if FORCE_ROOT
    ESP_LOGI(TAG, "Root node");
    esp_mesh_lite_set_allowed_level(1);
#else
    ESP_LOGI(TAG, "Child node");
    esp_mesh_lite_set_disallowed_level(1);
#endif

    esp_mesh_lite_start();

#if !FORCE_ROOT
    child_uart_init();
    xTaskCreate(child_uart_forward_task, "child_uart_fwd", 4096, NULL, 5, NULL);
#endif

    TimerHandle_t t = xTimerCreate("print_system_info", 10000 / portTICK_PERIOD_MS, true, NULL, print_system_info_timercb);
    xTimerStart(t, 0);
}
