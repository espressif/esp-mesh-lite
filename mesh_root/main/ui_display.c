#include "ui_display.h"
#include "esp_log.h"

// BSP + LVGL
#include "bsp/esp-bsp.h"
#include "lvgl.h"

static const char *TAG = "ui";

void ui_display_init(void)
{
    // Bring up the RGB panel + LVGL (BSP handles pins, clocks, buffers)
    esp_err_t err = bsp_display_start();
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "bsp_display_start failed: %d", err);
        return;
    }

    // Optional: set log level lower to keep frame rate nice
    esp_log_level_set("lvgl", ESP_LOG_WARN);

    // Simple "Hello, World" label
    lv_obj_t *label = lv_label_create(lv_scr_act());
    lv_label_set_text(label, "Hello, World");
    lv_obj_center(label);

    ESP_LOGI(TAG, "UI ready");
}

void ui_display_append(const char *line)
{
    // For now, just log—later we’ll add an LVGL scrolling list
    if (!line) line = "";
    ESP_LOGI(TAG, "%s", line);
}
