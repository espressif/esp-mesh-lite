#include "ui_display.h"
#include "esp_log.h"

static const char *TAG = "ui";

void ui_display_init(void) {
    // Placeholder: later we’ll init TFT/LVGL here.
    ESP_LOGI(TAG, "UI shim initialized (no LCD yet)");
}

void ui_display_append(const char *line) {
    if (!line) line = "";
    // Placeholder: later we’ll draw text to the LCD. For now, just log.
    ESP_LOGI(TAG, "%s", line);
}
