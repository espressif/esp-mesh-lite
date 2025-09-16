#include "ui_display.h"
#include "esp_log.h"
#include "lvgl.h"

static const char *TAG = "ui";

void ui_display_init(void)
{
    // Minimal LVGL init; no display/touch driver yet.
    lv_init();
    ESP_LOGI(TAG, "UI shim initialized (no LCD bound yet)");
}

void ui_display_append(const char *line)
{
    if (!line) line = "";
    // For now, just log it. Later this will render to the LCD.
    ESP_LOGI(TAG, "%s", line);
}
