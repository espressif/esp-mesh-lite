// ui_display.c
#include "ui_display.h"
#include "esp_log.h"

static const char *TAG = "ui";

void ui_display_init(void)
{
    ESP_LOGI(TAG, "UI shim initialized (log-only)");
}

void ui_display_append(const char *line)
{
    if (line) ESP_LOGI(TAG, "%s", line);
}
