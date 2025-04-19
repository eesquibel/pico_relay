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
    mqtt.c
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
target_link_libraries(pico_relay pico_lwip_sntp)
target_link_libraries(pico_relay pico_aon_timer)

target_compile_definitions(pico_lwip_sntp INTERFACE
    SNTP_STARTUP_DELAY=0
    SNTP_SET_SYSTEM_TIME=sntp_set_system_time
)

target_precompile_headers(pico_lwip_sntp INTERFACE
    "sntp.h"
)

set(DEVICE_ID "pico_relay_1")

# WiFi credentials
# set(WIFI_SSID "")
# set(WIFI_PASSWORD "")

# MQTT credentials
# set(MQTT_BROKER "")
# set(MQTT_USERNAME "")
# set(MQTT_PASSWORD "")

# MQTT topics
set(MQTT_PUBLISH_TOPIC "homeassistant/switch/${DEVICE_ID}/set")
set(MQTT_SUBSCRIBE_TOPIC "homeassistant/switch/${DEVICE_ID}/state")

# SNTP server
set(SNTP_SERVER "${MQTT_BROKER}")

target_compile_definitions(pico_relay PRIVATE
    DEVICE_ID="${DEVICE_ID}"
    WIFI_SSID="${WIFI_SSID}"
    WIFI_PASSWORD="${WIFI_PASSWORD}"
    MQTT_BROKER="${MQTT_BROKER}"
    MQTT_USERNAME="${MQTT_USERNAME}"
    MQTT_PASSWORD="${MQTT_PASSWORD}"
    MQTT_PUBLISH_TOPIC="${MQTT_PUBLISH_TOPIC}"
    MQTT_SUBSCRIBE_TOPIC="${MQTT_SUBSCRIBE_TOPIC}"
)

if(DEFINED ENV{SNTP_SERVER})
    message("Using SNTP server from environment variable: $ENV{SNTP_SERVER}")
    
    add_executable(pico_relay
        mqtt.c
    )

    target_compile_definitions(pico_relay PRIVATE
        SNTP_SERVER="$ENV{SNTP_SERVER}"
    )
else()
    message("SNTP server not set, skipping support")
endif()

pico_add_extra_outputs(pico_relay)
