set(srcs "src/wifi_prov_mgr.c")
set(requires "wifi_provisioning" "qrcode" "esp_wifi" "mesh_lite")

idf_component_register(SRCS "${srcs}"
                       INCLUDE_DIRS "include"
                       REQUIRES "${requires}")
