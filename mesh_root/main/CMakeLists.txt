# Minimal component for the mesh_root app.
# We keep ui_display.c as a stub (it only logs), so the build stays simple.

idf_component_register(
  SRCS
    "no_router.c"
    "root_udp.c"
    "ui_display.c"
    "uart_bridge.c"
  INCLUDE_DIRS "."
  REQUIRES
    esp_wifi
    esp_event
    esp_netif
    nvs_flash
    driver
    esp_timer
    mesh_lite
)
