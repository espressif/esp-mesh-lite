#include "uart_bridge.h"
#include "driver/uart.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include <string.h>

static const char *TAG = "uart_bridge";
static int s_uart = CONFIG_UART_BRIDGE_PORT;

esp_err_t uart_bridge_init(void)
{
    const uart_config_t cfg = {
        .baud_rate = CONFIG_UART_BRIDGE_BAUD,
        .data_bits = UART_DATA_8_BITS,
        .parity    = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE
    };
    ESP_ERROR_CHECK(uart_driver_install(s_uart, 2048, 2048, 0, NULL, 0));
    ESP_ERROR_CHECK(uart_param_config(s_uart, &cfg));
    ESP_ERROR_CHECK(uart_set_pin(s_uart,
                                 CONFIG_UART_BRIDGE_TX_PIN,
                                 CONFIG_UART_BRIDGE_RX_PIN,
                                 UART_PIN_NO_CHANGE,
                                 UART_PIN_NO_CHANGE));
    ESP_LOGI(TAG, "UART%d @ %d, TX=%d RX=%d",
             s_uart, CONFIG_UART_BRIDGE_BAUD,
             CONFIG_UART_BRIDGE_TX_PIN, CONFIG_UART_BRIDGE_RX_PIN);
    return ESP_OK;
}

void uart_bridge_write_line(const char *line)
{
    if (!line) return;
    uart_write_bytes(s_uart, line, strlen(line));
    uart_write_bytes(s_uart, "\r\n", 2);
}
