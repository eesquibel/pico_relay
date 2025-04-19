# Generated Cmake Pico project file

cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Initialise pico_sdk from installed location
# (note this can come from environment, CMake cache etc)

# == DO NOT EDIT THE FOLLOWING LINES for the Raspberry Pi Pico VS Code Extension to work ==
if(WIN32)
    set(USERHOME $ENV{USERPROFILE})
else()
    set(USERHOME $ENV{HOME})
endif()
set(sdkVersion 2.1.1)
set(toolchainVersion 14_2_Rel1)
set(picotoolVersion 2.1.1)
set(picoVscode ${USERHOME}/.pico-sdk/cmake/pico-vscode.cmake)
if (EXISTS ${picoVscode})
    include(${picoVscode})
endif()
# ====================================================================================
set(PICO_BOARD pico2_w CACHE STRING "Board type")

# Pull in Raspberry Pi Pico SDK (must be before project)
include(pico_sdk_import.cmake)

project(pico_relay C CXX ASM)

# Initialise the Raspberry Pi Pico SDK
pico_sdk_init()

# Add executable. Default name is the project name, version 0.1

add_executable(pico_relay
    sntp.c
    mqtt.c
    home-assistant.c
    pico_relay.c
)

pico_set_program_name(pico_relay "pico_relay")
pico_set_program_version(pico_relay "0.1")

# Modify the below lines to enable/disable output over UART/USB
pico_enable_stdio_uart(pico_relay 1)
# pico_enable_stdio_usb(pico_relay 1)

# Add the standard library to the build
target_link_libraries(pico_relay pico_stdlib)

# Add the standard include files to the build
target_include_directories(pico_relay PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}
)

# Add any user requested libraries
target_link_libraries(pico_relay pico_cyw43_arch_lwip_threadsafe_background)
target_link_libraries(pico_relay pico_lwip)
target_link_libraries(pico_relay pico_lwip_mqtt)

# json-maker library
add_subdirectory(libs/json-maker)
target_include_directories(pico_relay PRIVATE "./libs/json-maker/src/include")
target_link_libraries(pico_relay json_maker)

target_compile_definitions(pico_lwip_sntp INTERFACE
    SNTP_STARTUP_DELAY=0
    SNTP_SET_SYSTEM_TIME=sntp_set_system_time
)

set(DEVICE_ID "pico_relay_1")

# WiFi credentials
set(WIFI_SSID "$ENV{WIFI_SSID}")
set(WIFI_PASSWORD "$ENV{WIFI_PASSWORD}")

# MQTT credentials
set(MQTT_BROKER "$ENV{MQTT_BROKER}")
set(MQTT_USERNAME "$ENV{MQTT_USERNAME}")
set(MQTT_PASSWORD "$ENV{MQTT_PASSWORD}")

# MQTT topics
set(MQTT_PUBLISH_TOPIC "homeassistant/valve/${DEVICE_ID}/set")
set(MQTT_SUBSCRIBE_TOPIC "homeassistant/valve/${DEVICE_ID}/state")
set(MQTT_CONFIG_TOPIC "homeassistant/valve/${DEVICE_ID}/config")

# SNTP server
# set(SNTP_SERVER "")

if(NOT DEFINED SNTP_SERVER AND DEFINED MQTT_BROKER)
    message("Using MQTT broker for SNTP server: ${MQTT_BROKER}")
    set(SNTP_SERVER "${MQTT_BROKER}")
endif()

target_compile_definitions(pico_relay PRIVATE
    DEVICE_ID="${DEVICE_ID}"
    WIFI_SSID="${WIFI_SSID}"
    WIFI_PASSWORD="${WIFI_PASSWORD}"
    MQTT_BROKER="${MQTT_BROKER}"
    MQTT_USERNAME="${MQTT_USERNAME}"
    MQTT_PASSWORD="${MQTT_PASSWORD}"
    MQTT_PUBLISH_TOPIC="${MQTT_PUBLISH_TOPIC}"
    MQTT_SUBSCRIBE_TOPIC="${MQTT_SUBSCRIBE_TOPIC}"
    MQTT_CONFIG_TOPIC="${MQTT_CONFIG_TOPIC}"
)

if(DEFINED SNTP_SERVER)
    message("Adding SNTP support, server: ${SNTP_SERVER}")

    target_compile_definitions(pico_relay PRIVATE
        SNTP_SERVER="${SNTP_SERVER}"
    )

    target_precompile_headers(pico_lwip_sntp INTERFACE
        "sntp.h"
    )

    target_link_libraries(pico_relay pico_lwip_sntp)
    target_link_libraries(pico_relay pico_aon_timer)
else()
    message("SNTP server not set, skipping support")
endif()

pico_add_extra_outputs(pico_relay)
