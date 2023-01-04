# The following five lines of boilerplate have to be in your project's
# CMakeLists in this exact order for cmake to work correctly
cmake_minimum_required(VERSION 3.5)

add_definitions(-w)
add_compile_options(-fdiagnostics-color=always)

# Add git describe to version
EXECUTE_PROCESS(
    COMMAND git describe --always --dirty
    OUTPUT_VARIABLE IMAGE_VERSION_GIT_HEAD_VERSION
    OUTPUT_STRIP_TRAILING_WHITESPACE
    ERROR_VARIABLE GET_GIT_VERSION_FAILED
)
string(CONCAT PROJECT_V "v1.0.0" "-beta_" ${IMAGE_VERSION_GIT_HEAD_VERSION})
set(PROJECT_VER ${PROJECT_V})

include($ENV{IDF_PATH}/tools/cmake/project.cmake)
project(led_light)
