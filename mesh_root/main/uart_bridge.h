#pragma once
#include "esp_err.h"
esp_err_t uart_bridge_init(void);
void      uart_bridge_write_line(const char *line);
