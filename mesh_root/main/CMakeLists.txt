idf_component_register(
  SRCS
    "no_router.c"
    "root_udp.c"
    "ui_display.c"
    "uart_bridge.c"
  INCLUDE_DIRS "."
  REQUIRES esp_timer
)
