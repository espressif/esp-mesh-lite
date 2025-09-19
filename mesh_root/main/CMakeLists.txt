idf_component_register(
  SRCS
    "no_router.c"
    "root_udp.c"
    "uart_bridge.c"
    "ui_display.c"
  INCLUDE_DIRS "."
  REQUIRES
    esp_wifi
    esp_event
    esp_netif
    nvs_flash
    driver
    esp_timer
    esp_mesh_lite
    esp_bridge      # <--- add this
)
