set(srcs "src/esp_mesh_lite_espnow.c")
set(require_components "json" "esp_wifi" "app_update" "wifi_provisioning" "qrcode" "nvs_flash")

if (CONFIG_MESH_LITE_ENABLE)
    list(APPEND srcs "src/esp_mesh_lite.c" "src/esp_mesh_lite_port.c" "src/esp_mesh_lite_log.c")
    list(APPEND srcs "src/wifi_prov/wifi_prov_mgr.c" "src/wifi_prov/zero_provisioning.c")

    set(OLD_PROV_MGR "${IDF_PATH}/components/wifi_provisioning/src/")
    set(NEW_PROV_MGR "${CMAKE_CURRENT_LIST_DIR}/src/wifi_prov/manager.c")

    execute_process(COMMAND ${CMAKE_COMMAND} -E copy ${NEW_PROV_MGR} ${OLD_PROV_MGR})
endif()

idf_component_register(SRCS "${srcs}"
                       INCLUDE_DIRS include
                       REQUIRES ${require_components})

execute_process(COMMAND
                git apply ${CMAKE_CURRENT_LIST_DIR}/src/wifi_prov/wifi_provisioning.patch
                WORKING_DIRECTORY $ENV{IDF_PATH})

string(CONCAT LIBS "esp_mesh_lite_" "${IDF_TARGET}")

add_library(${LIBS} STATIC IMPORTED)
set_property(TARGET ${LIBS} PROPERTY IMPORTED_LOCATION ${CMAKE_CURRENT_SOURCE_DIR}/lib/lib${LIBS}.a)
target_link_libraries(${COMPONENT_LIB} INTERFACE ${LIBS})
set_property(TARGET ${LIBS} APPEND PROPERTY INTERFACE_LINK_LIBRARIES ${COMPONENT_LIB})

include(package_manager)
if (CONFIG_MESH_LITE_ENABLE)
    cu_pkg_define_version(${CMAKE_CURRENT_LIST_DIR})
endif()
