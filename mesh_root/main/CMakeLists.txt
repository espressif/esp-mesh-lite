idf_component_register(
    SRCS
        "no_router.c"
        "root_udp.c"
        "ui_display.c"
        "panel_rgb.c"       # (we’ll add next)
        "touch_gt911.c"     # (we’ll add next)
    INCLUDE_DIRS "."
    REQUIRES
        esp_timer
        esp_lcd
        esp_lcd_touch
        esp_lvgl_port
)
