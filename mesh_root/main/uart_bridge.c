// uart_bridge.c
#include "uart_bridge.h"

#include <string.h>
#include "driver/uart.h"
#include "driver/gpio.h"
#include "esp_log.h"

static const char *TAG = "uart_bridge";

#ifndef CONFIG_UART_BRIDGE_PORT
#define CONFIG_UART_BRIDGE_PORT 1
#endif

#ifndef CONFIG_UART_BRIDGE_BAUD
#define CONFIG_UART_BRIDGE_BAUD 115200
#endif

#ifndef CONFIG_UART_BRIDGE_TX_PIN
#define CONFIG_UART_BRIDGE_TX_PIN 43
#endif

#ifndef CONFIG_UART_BRIDGE_RX_PIN
#define CONFIG_UART_BRIDGE_RX_PIN 44
#endif

#define UART_BUF_SIZE 256

void uart_bridge_init(void)
{
    const uart_port_t port = CONFIG_UART_BRIDGE_PORT;

    uart_config_t cfg = {
        .baud_rate = CONFIG_UART_BRIDGE_BAUD,
        .data_bits = UART_DATA_8_BITS,
        .parity    = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .source_clk = UART_SCLK_APB,
    };
    ESP_ERROR_CHECK(uart_driver_install(port, UART_BUF_SIZE, 0, 0, NULL, 0));
    ESP_ERROR_CHECK(uart_param_config(port, &cfg));
    ESP_ERROR_CHECK(uart_set_pin(port,
                                 CONFIG_UART_BRIDGE_TX_PIN,
                                 CONFIG_UART_BRIDGE_RX_PIN,
                                 UART_PIN_NO_CHANGE,
                                 UART_PIN_NO_CHANGE));
    ESP_LOGI(TAG, "UART%d @ %d, TX=%d RX=%d ready",
             (int)port, (int)CONFIG_UART_BRIDGE_BAUD,
             (int)CONFIG_UART_BRIDGE_TX_PIN, (int)CONFIG_UART_BRIDGE_RX_PIN);
}

void uart_bridge_send_line(const char *s)
{
    if (!s) return;
    const uart_port_t port = CONFIG_UART_BRIDGE_PORT;

    // write string + CRLF
    uart_write_bytes(port, s, strlen(s));
    const char crlf[2] = "\r\n";
    uart_write_bytes(port, crlf, 2);
}
