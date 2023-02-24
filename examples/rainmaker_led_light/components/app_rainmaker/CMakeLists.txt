idf_component_register(SRCS "app_rainmaker.c" "app_rainmaker_ota.c" "app_rainmaker_ota_topic.c"
                       INCLUDE_DIRS "."
                       REQUIRES app_wifi app_insights app_light esp_rainmaker json mesh_lite json_parser esp_https_ota bt nvs_flash app_update)

target_add_binary_data(${COMPONENT_TARGET} "server.crt" TEXT)
