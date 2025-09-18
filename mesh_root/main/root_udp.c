#include "root_udp.h"

#include <string.h>
#include <sys/socket.h>
#include <stdbool.h>
#include <unistd.h>
#include <stdio.h>

#include "lwip/sockets.h"
#include "lwip/inet.h"

#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_mesh_lite.h"
#include "esp_mac.h"

#include "ui_display.h"
#include "uart_bridge.h"

static const char *TAG = "root_udp";

static bool find_mac_by_ip(uint32_t ip_be, uint8_t out_mac[6], int *out_level)
{
    if (out_mac) memset(out_mac, 0, 6);
    if (out_level) *out_level = -1;

    uint32_t n = 0;
    const node_info_list_t *node = esp_mesh_lite_get_nodes_list(&n);
    while (node) {
        if (node->node && node->node->ip_addr == ip_be) {
            if (out_mac) memcpy(out_mac, node->node->mac_addr, 6);
            if (out_level) *out_level = node->node->level;
            return true;
        }
        node = node->next;
    }
    return false;
}

static void udp_task(void *arg)
{
    uint16_t port = (uint16_t)(uintptr_t)arg;

    int sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (sock < 0) {
        ESP_LOGE(TAG, "socket() failed");
        vTaskDelete(NULL);
        return;
    }

    struct sockaddr_in addr = {0};
    addr.sin_family = AF_INET;
    addr.sin_port   = htons(port);
    addr.sin_addr.s_addr = htonl(INADDR_ANY);

    if (bind(sock, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        ESP_LOGE(TAG, "bind() failed");
        close(sock);
        vTaskDelete(NULL);
        return;
    }

    ESP_LOGI(TAG, "listening on UDP :%u", (unsigned)port);

    uint8_t buf[256];

    for (;;) {
        struct sockaddr_in from = {0};
        socklen_t fromlen = sizeof(from);
        int n = recvfrom(sock, buf, sizeof(buf) - 1, 0,
                         (struct sockaddr *)&from, &fromlen);
        if (n <= 0) continue;

        buf[n] = '\0';

        uint8_t mac[6] = {0};
        int level = -1;
        bool known = find_mac_by_ip(from.sin_addr.s_addr, mac, &level);

        char ipstr[16] = {0};
        inet_ntop(AF_INET, &from.sin_addr, ipstr, sizeof(ipstr));

        char msg[256];
        size_t payload_len = (size_t)n;

        int hdr = known
            ? snprintf(msg, sizeof(msg),
                       "[%s lvl%d %02x:%02x:%02x:%02x:%02x:%02x] ",
                       ipstr, level,
                       mac[0], mac[1], mac[2], mac[3], mac[4], mac[5])
            : snprintf(msg, sizeof(msg), "[%s] ", ipstr);

        if (hdr < 0) hdr = 0;
        if ((size_t)hdr >= sizeof(msg)) hdr = (int)sizeof(msg) - 1;

        size_t cap = sizeof(msg) - (size_t)hdr - 1;
        if (payload_len > cap) payload_len = cap;

        memcpy(msg + hdr, (const char *)buf, payload_len);
        msg[hdr + payload_len] = '\0';

        ui_display_append(msg);
        uart_bridge_send_line(msg);
        ESP_LOGI(TAG, "%s", msg);
    }
}

int root_udp_start(uint16_t port)
{
    if (xTaskCreate(udp_task, "root_udp", 4096, (void *)(uintptr_t)port, 5, NULL) != pdPASS) {
        ESP_LOGE(TAG, "failed to create udp_task");
        return -1;
    }
    return 0;
}
