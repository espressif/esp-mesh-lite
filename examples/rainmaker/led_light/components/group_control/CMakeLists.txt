idf_component_register(SRCS "app_espnow.c"
                       INCLUDE_DIRS "."
                       REQUIRES esp_wifi wifi_provisioning json mesh_lite app_bridge app_wifi)
