idf_component_register(SRCS "app_bridge.c" "app_mesh_lite_comm.c"
                       INCLUDE_DIRS "."
                       PRIV_REQUIRES iot_bridge mesh_lite app_rainmaker)
