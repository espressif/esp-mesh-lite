#include "ui_display.h"
#include "lvgl.h"
#include "esp_log.h"

static const char *TAG = "ui_display";
static lv_obj_t *log_label;

void ui_display_init(void)
{
    lv_init();

    // Initialize display and touch (Waveshare BSP handles drivers)
    lv_display_t *disp = lv_display_create(800, 480); // your panel size
    lv_theme_t *theme = lv_theme_default_init(disp,
                        lv_palette_main(LV_PALETTE_BLUE),
                        lv_palette_main(LV_PALETTE_RED),
                        false, LV_FONT_DEFAULT);

    lv_disp_set_theme(disp, theme);

    // Create a full-screen black background
    lv_obj_t *scr = lv_obj_create(NULL);
    lv_obj_set_style_bg_color(scr, lv_color_black(), 0);
    lv_scr_load(scr);

    // Create a label for logs
    log_label = lv_label_create(scr);
    lv_obj_set_style_text_color(log_label, lv_color_white(), 0);
    lv_label_set_text(log_label, "UI ready...\n");
}

void ui_display_append(const char *msg)
{
    if (!log_label) return;

    // Append new line of text
    const char *old = lv_label_get_text(log_label);
    static char buf[2048]; // ring buffer later
    snprintf(buf, sizeof(buf), "%s\n%s", old, msg);
    lv_label_set_text(log_label, buf);
}
