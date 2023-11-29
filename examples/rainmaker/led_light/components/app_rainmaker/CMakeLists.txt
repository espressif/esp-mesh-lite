idf_component_register(SRCS "app_rainmaker.c" "app_rainmaker_ota.c" "app_rainmaker_ota_topic.c"
                       INCLUDE_DIRS "."
                       REQUIRES json
                                nvs_flash
                                app_update
                                json_parser
                                esp_https_ota
                                esp_rainmaker
                                app_light
                                app_insights
                                mesh_lite
                                group_control)

target_add_binary_data(${COMPONENT_TARGET} "server.crt" TEXT)
target_link_libraries(${COMPONENT_LIB} INTERFACE "-Wl,--wrap=esp_rmaker_handle_set_params")
