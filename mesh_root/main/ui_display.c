#include "ui_display.h"
#include "esp_log.h"
#include "lvgl.h"

static const char *TAG = "ui";

static lv_disp_t *s_disp;

void ui_display_init(void) {
    ESP_LOGI(TAG, "UI init start");

    // --- LVGL init ---
    lv_init();

    // --- RGB panel config (fill in from Waveshare docs) ---
    esp_lcd_rgb_panel_config_t panel_config = {
        .clk_src = LCD_CLK_SRC_PLL160M,
        .timings = {
            .pclk_hz = 16000000, // adjust per datasheet
            .h_res = 800,
            .v_res = 480,
            .hsync_pulse_width = 10,
            .hsync_back_porch = 20,
            .hsync_front_porch = 20,
            .vsync_pulse_width = 10,
            .vsync_back_porch = 10,
            .vsync_front_porch = 10,
            .flags.pclk_active_neg = true,
        },
        .data_width = 16, // check your board
        .bits_per_pixel = 16,
        // TODO: assign your GPIOs for RGB data and sync pins here
    };

    esp_lcd_panel_handle_t panel_handle = NULL;
    ESP_ERROR_CHECK(esp_lcd_new_rgb_panel(&panel_config, &panel_handle));
    ESP_ERROR_CHECK(esp_lcd_panel_reset(panel_handle));
    ESP_ERROR_CHECK(esp_lcd_panel_init(panel_handle));

    // --- LVGL display driver ---
    lv_disp_draw_buf_t draw_buf;
    static lv_color_t buf1[800*40];
    static lv_color_t buf2[800*40];
    lv_disp_draw_buf_init(&draw_buf, buf1, buf2, 800*40);

    lv_disp_drv_t disp_drv;
    lv_disp_drv_init(&disp_drv);
    disp_drv.hor_res = 800;
    disp_drv.ver_res = 480;
    disp_drv.flush_cb = NULL; // TODO: hook to esp_lcd_panel_draw_bitmap
    disp_drv.draw_buf = &draw_buf;
    s_disp = lv_disp_drv_register(&disp_drv);

    // --- HELLO WORLD ---
    lv_obj_t *label = lv_label_create(lv_disp_get_scr_act(s_disp));
    lv_label_set_text(label, "HELLO WORLD");
    lv_obj_center(label);

    ESP_LOGI(TAG, "UI init done");
}

void ui_display_append(const char *line) {
    // for now just log
    ESP_LOGI(TAG, "%s", line);
}idf_component_register(
    SRCS "no_router.c" "root_udp.c" "ui_display.c"
    INCLUDE_DIRS "."
)
