set(srcs)
set(require_components "json" "esp_wifi")

if (CONFIG_MESH_LITE_ENABLE)
    list(APPEND srcs "src/esp_mesh_lite.c")

    set(OLD_PROV_MGR "${IDF_PATH}/components/wifi_provisioning/src/")

    if(IDF_VERSION_MAJOR STREQUAL "4")
    set(NEW_PROV_MGR "${CMAKE_CURRENT_LIST_DIR}/src/wifi_prov/v4.4/manager.c")
    elseif(IDF_VERSION_MAJOR STREQUAL "5")
    set(NEW_PROV_MGR "${CMAKE_CURRENT_LIST_DIR}/src/wifi_prov/v5.0/manager.c")
    endif()

    execute_process(COMMAND cp ${NEW_PROV_MGR} ${OLD_PROV_MGR} OUTPUT_VARIABLE info
            RESULT_VARIABLE exit_code
            OUTPUT_STRIP_TRAILING_WHITESPACE)
endif()

idf_component_register (SRCS "${srcs}"
                        INCLUDE_DIRS include
                        REQUIRES ${require_components})

string(CONCAT LIBS "esp_mesh_lite_" "${IDF_TARGET}")

add_library(${LIBS} STATIC IMPORTED)
set_property(TARGET ${LIBS} PROPERTY IMPORTED_LOCATION ${CMAKE_CURRENT_SOURCE_DIR}/lib/lib${LIBS}.a)
target_link_libraries(${COMPONENT_LIB} INTERFACE ${LIBS})
set_property(TARGET ${LIBS} APPEND PROPERTY INTERFACE_LINK_LIBRARIES ${COMPONENT_LIB})