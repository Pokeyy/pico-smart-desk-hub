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

project(ST7735_PICO C CXX ASM)

# Initialise the Raspberry Pi Pico SDK
pico_sdk_init()

# Include Wi-Fi credentials
include(wifi_config.cmake OPTIONAL)

if (NOT DEFINED WIFI_SSID)
    set(WIFI_SSID "")
endif()

if (NOT DEFINED WIFI_PASSWORD)
    set(WIFI_PASSEORD "")
endif()

# FreeRTOS
set(FREERTOS_KERNEL_PATH ${CMAKE_CURRENT_LIST_DIR}/FreeRTOS-Kernel)
include(FreeRTOS_Kernel_import.cmake)


add_executable(ST7735_PICO 
    src/main.c 
    src/drivers/ST7735_TFT.c 
    src/drivers/fonts.c 
    src/weather/weather.c
    src/lib/http_client_util.c
    src/lib/cJSON.c
    src/wifi/wifi.c
    src/display/display_tasks.c
    src/clock/clock.c
    src/ble/ble.c
) 

#target_compile_definitions(ST7735_PICO PRIVATE PICO_ENTER_USB_BOOT_ON_EXIT=1)

pico_set_program_name(ST7735_PICO "ST7735_PICO")
pico_set_program_version(ST7735_PICO "0.1")

# Modify the below lines to enable/disable output over UART/USB
pico_enable_stdio_uart(ST7735_PICO 1)
pico_enable_stdio_usb(ST7735_PICO 1)


target_compile_definitions(ST7735_PICO PRIVATE
        WIFI_SSID=\"${WIFI_SSID}\"
        WIFI_PASSWORD=\"${WIFI_PASSWORD}\"
        )

pico_btstack_make_gatt_header(ST7735_PICO PRIVATE
    "${CMAKE_CURRENT_LIST_DIR}/src/ble/profile.gatt"
)

# Add the standard library to the build
target_link_libraries(ST7735_PICO
        pico_stdlib
        pico_util
        hardware_spi
        pico_aon_timer
        pico_cyw43_arch_lwip_threadsafe_background
        pico_lwip_http
        pico_lwip_mbedtls
        pico_mbedtls
        FreeRTOS-Kernel-Heap4
        pico_lwip_sntp
        pico_btstack_ble
        pico_btstack_cyw43
        )


pico_add_extra_outputs(ST7735_PICO)


target_include_directories(ST7735_PICO PRIVATE 
    include
    src
    src/config
)


# Ignore warnings from lwip code
set_source_files_properties(
        ${PICO_LWIP_PATH}/src/apps/altcp_tls/altcp_tls_mbedtls.c
        PROPERTIES
        COMPILE_OPTIONS "-Wno-unused-result"
        )


