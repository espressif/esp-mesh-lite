idf_component_register(
    SRCS "no_router.c" "root_udp.c" "ui_display.c"
    INCLUDE_DIRS "."
    REQUIRES lvgl esp_lcd
)
