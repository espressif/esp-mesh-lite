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
#include "driver/gpio.h"   // RX pull-up

#define FORCE_ROOT 0   // child build

/* ---- UART config (sender TX=17 -> leaf RX=16) ---- */
#define UART_PORT     UART_NUM_2
#define UART_TX_PIN   17          // not used by leaf for TX, but set anyway
#define UART_RX_PIN   16          // connect sender TX here
#define UART_BAUD     9600        // MUST match the sender
#define UART_RXBUF_SZ (16 * 1024)
#define LINE_MAX      256         // larger line buffer

/* ---- UDP port used by root listener ---- */
#define UDP_PORT      3333

/* ---- Debug: dump every UART chunk (hex + ascii) ---- */
#define UART_DUMP     1
/* Limit dump length so logs don’t explode */
#define DUMP_MAX      64

static const char *TAG = "no_router";

/* ---------- periodic mesh info print ---------- */
static void print_system_info_timercb(TimerHandle_t timer)
{
    uint8_t primary = 0, sta_mac[6] = {0};
    wifi_ap_record_t ap_info = {0};
    wifi_second_chan_t second = 0;
    wifi_sta_list_t wifi_sta_list = {0};

    if (esp_mesh_lite_get_level() > 1) {
        esp_wifi_sta_get_ap_info(&ap_info);
    }
    esp_wifi_get_mac(ESP_IF_WIFI_STA, sta_mac);
    esp_wifi_ap_get_sta_list(&wifi_sta_list);
    esp_wifi_get_channel(&primary, &second);

    ESP_LOGI(TAG,
             "System information, channel: %d, layer: %d, self mac: " MACSTR
             ", parent bssid: " MACSTR ", parent rssi: %d, free heap: %" PRIu32,
             primary, esp_mesh_lite_get_level(), MAC2STR(sta_mac), MAC2STR(ap_info.bssid),
             (ap_info.rssi != 0 ? ap_info.rssi : -120), esp_get_free_heap_size());

    uint32_t size = 0;
    const node_info_list_t *node = esp_mesh_lite_get_nodes_list(&size);
    printf("MeshLite nodes %lu:\r\n", (unsigned long)size);
    for (uint32_t i = 0; (i < size) && node; i++) {
        struct in_addr ip;
        ip.s_addr = node->node->ip_addr;
        printf("%lu: %d, " MACSTR ", %s\r\n",
               (unsigned long)i + 1, node->node->level,
               MAC2STR(node->node->mac_addr), inet_ntoa(ip));
        node = node->next;
    }
}

/* ---------- storage & Wi-Fi init ---------- */
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
    // Station
    wifi_config_t wifi_config;
    memset(&wifi_config, 0, sizeof(wifi_config));
    esp_bridge_wifi_set_config(WIFI_IF_STA, &wifi_config);

    // SoftAP (for lower-level children, if any)
    wifi_config_t ap_cfg = {
        .ap = {
            .ssid     = CONFIG_BRIDGE_SOFTAP_SSID,
            .password = CONFIG_BRIDGE_SOFTAP_PASSWORD,
            .channel  = CONFIG_MESH_CHANNEL,
        },
    };
    esp_bridge_wifi_set_config(WIFI_IF_AP, &ap_cfg);
}

static void app_wifi_set_softap_info(void)
{
    char ssid[33];
    char psw[64];
    uint8_t mac[6];
    size_t ssid_sz = sizeof(ssid);
    size_t psw_sz  = sizeof(psw);
    esp_wifi_get_mac(WIFI_IF_AP, mac);
    memset(ssid, 0, sizeof(ssid));
    memset(psw,  0, sizeof(psw));

    if (esp_mesh_lite_get_softap_ssid_from_nvs(ssid, &ssid_sz) != ESP_OK) {
#ifdef CONFIG_BRIDGE_SOFTAP_SSID_END_WITH_THE_MAC
        snprintf(ssid, sizeof(ssid), "%.25s_%02x%02x%02x",
                 CONFIG_BRIDGE_SOFTAP_SSID, mac[3], mac[4], mac[5]);
#else
        snprintf(ssid, sizeof(ssid), "%.32s", CONFIG_BRIDGE_SOFTAP_SSID);
#endif
    }
    if (esp_mesh_lite_get_softap_psw_from_nvs(psw, &psw_sz) != ESP_OK) {
        strlcpy(psw, CONFIG_BRIDGE_SOFTAP_PASSWORD, sizeof(psw));
    }
    esp_mesh_lite_set_softap_info(ssid, psw);
}

/* ---------- UART init ---------- */
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
    ESP_ERROR_CHECK(uart_driver_install(UART_PORT, UART_RXBUF_SZ, 0, 0, NULL, 0));
    ESP_ERROR_CHECK(uart_param_config(UART_PORT, &cfg));
    ESP_ERROR_CHECK(uart_set_pin(UART_PORT, UART_TX_PIN, UART_RX_PIN,
                                 UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE));

    // Keep RX line pulled high when idle to reduce noise
    gpio_config_t io = {
        .pin_bit_mask = 1ULL << UART_RX_PIN,
        .mode         = GPIO_MODE_INPUT,
        .pull_up_en   = 1,
        .pull_down_en = 0,
        .intr_type    = GPIO_INTR_DISABLE
    };
    gpio_config(&io);

    ESP_LOGI(TAG, "UART ready @%d (TX=%d RX=%d)", UART_BAUD, UART_TX_PIN, UART_RX_PIN);
}

/* ---- helper: dump a UART chunk as hex & ascii ---- */
static inline void dump_uart(const uint8_t *b, int n)
{
#if UART_DUMP
    int m = (n > DUMP_MAX) ? DUMP_MAX : n;

    char hex[3 * DUMP_MAX + 1];
    int p = 0;
    for (int i = 0; i < m && p < (int)sizeof(hex) - 3; ++i) {
        p += snprintf(&hex[p], sizeof(hex) - p, "%02X ", b[i]);
    }
    hex[p] = 0;

    char asc[DUMP_MAX + 1];
    for (int i = 0; i < m; ++i) {
        uint8_t c = b[i];
        asc[i] = (c >= 32 && c <= 126) ? (char)c : '.';
    }
    asc[m] = 0;

    ESP_LOGI(TAG, "UART RX (%d): %s%s", n, hex, (n > m) ? "..." : "");
    ESP_LOGI(TAG, "UART RX ASCII : %s%s", asc, (n > m) ? "..." : "");
#endif
}

/* ---------- UART -> UDP forwarder (robust line builder) ---------- */
static void child_uart_forward_task(void *arg)
{
    // UDP socket (broadcast to root listener)
    int sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (sock < 0) {
        ESP_LOGE(TAG, "UDP socket create failed");
        vTaskDelete(NULL);
        return;
    }
    int yes = 1;
    setsockopt(sock, SOL_SOCKET, SO_BROADCAST, &yes, sizeof(yes));

    struct sockaddr_in dest = {0};
    dest.sin_family      = AF_INET;
    dest.sin_port        = htons(UDP_PORT);
    dest.sin_addr.s_addr = inet_addr("255.255.255.255");

    uint8_t  line[LINE_MAX];
    size_t   n = 0;
    uint32_t last_byte_ms = 0;

    for (;;) {
        uint8_t buf[64];
        int r = uart_read_bytes(UART_PORT, buf, sizeof(buf), pdMS_TO_TICKS(50));
        uint32_t now = (uint32_t)(esp_timer_get_time() / 1000); // ms

        if (r > 0) {
            dump_uart(buf, r);          // <--- print what we just read
            last_byte_ms = now;

            for (int i = 0; i < r; ++i) {
                uint8_t c = buf[i];

                if (c == '\n') {                   // end-of-line
                    while (n && (line[n-1] == '\r' || line[n-1] == '\n')) n--;
                    line[n] = '\0';
                    if (n > 0) {
                        ESP_LOGI(TAG, "UART line -> mesh: %s", (char *)line);
                        sendto(sock, line, n, 0, (struct sockaddr *)&dest, sizeof(dest));
                    }
                    n = 0;
                    continue;
                }

                // accept printable ASCII and CR; drop noise bytes
                bool printable = (c >= 32 && c <= 126) || c == '\r';
                if (!printable) {
                    if (n) { ESP_LOGW(TAG, "UART non-ASCII, dropping partial"); n = 0; }
                    continue;
                }

                if (n < sizeof(line) - 1) {
                    line[n++] = c;
                } else {
                    ESP_LOGW(TAG, "UART line overflow, dropping partial");
                    n = 0;   // reset if somehow a line gets too long
                }
            }
        } else {
            // Idle timeout: if we started a line but no newline for 300 ms, drop it
            if (n && (now - last_byte_ms) > 300) {
                ESP_LOGW(TAG, "UART idle timeout, dropping partial");
                n = 0;
            }
            vTaskDelay(pdMS_TO_TICKS(5));
        }
    }
}

/* --------------------------- app_main --------------------------- */
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
    cfg.join_mesh_without_configured_wifi = true;   // child can join with no external router
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

    TimerHandle_t t = xTimerCreate("print_system_info",
                                   10000 / portTICK_PERIOD_MS, true, NULL,
                                   print_system_info_timercb);
    xTimerStart(t, 0);
}
