#include "root_udp.h"

#include <string.h>
#include <sys/socket.h>
#include "lwip/sockets.h"
#include "lwip/inet.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_mesh_lite.h"

static const char *TAG = "root_udp";

static void find_mac_by_ip(uint32_t ip_be, uint8_t out_mac[6], int *out_level)
{
    // ip_be is in network byte order; Mesh-Lite stores ip_addr in BE as well.
    // Default to zeros if not found.
    memset(out_mac, 0, 6);
    if (out_level) *out_level = -1;

    uint32_t n = 0;
    const node_info_list_t *node = esp_mesh_lite_get_nodes_list(&n);
    while (node) {
        if (node->node && node->node->ip_addr == ip_be) {
            memcpy(out_mac, node->node->mac_addr, 6);
            if (out_level) *out_level = node->node->level;
            return;
        }
        node = node->next;
    }
}

static void udp_task(void *arg)
{
    const uint16_t port = (uint16_t)(uintptr_t)arg;

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
        buf[n] = 0;

        // Lookup sender MAC by IP in Mesh-Lite node list
        uint8_t mac[6];
        int level = -1;
        find_mac_by_ip(from.sin_addr.s_addr, mac, &level);

        char ipstr[16];
        inet_ntop(AF_INET, &from.sin_addr, ipstr, sizeof(ipstr));

        if (mac[0] || mac[1] || mac[2] || mac[3] || mac[4] || mac[5]) {
            ESP_LOGI(TAG, "[from %s lvl%d " MACSTR "] %s",
                     ipstr, level, MAC2STR(mac), (char *)buf);
        } else {
            // Unknown in node list (still log)
            ESP_LOGI(TAG, "[from %s] %s", ipstr, (char *)buf);
        }

        // Later we can push this to a queue for the LCD UI.
    }
}

int root_udp_start(uint16_t port)
{
    BaseType_t ok = xTaskCreate(udp_task, "root_udp", 4096, (void *)(uintptr_t)port, 5, NULL);
    return ok == pdPASS ? 0 : -1;
}
