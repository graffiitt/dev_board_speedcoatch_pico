cmake_minimum_required(VERSION 3.13)

set(PICO_BOARD pico_w CACHE STRING "Board type")

set(CONFIG_PATH ${CMAKE_CURRENT_LIST_DIR}/configs)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

include(pico_sdk_import.cmake)
include(FreeRTOS_Kernel_import.cmake)

project(specter_board C CXX ASM)
pico_sdk_init()

message("Adding ${PROJECT_NAME} C files: " )
file(GLOB CPPFILE "src/*/*.c")
foreach(source ${CPPFILE})
    message("  - " ${source})
endforeach(source)

add_executable(${PROJECT_NAME}
src/main.c
src/battery.c
${CPPFILE}
)

target_link_libraries(${PROJECT_NAME}
    pico_stdlib
    hardware_rtc
    hardware_adc
    FreeRTOS-Kernel-Heap4
    
    SHARPDISP
    BITMAPSHAPES
    BITMAPTEXT
    BITMAPIMAGE
    LIBERATION_SANS_24
    LIBERATION_SANS_48
    LIBERATION_SANS_80
    
    UI_LIBRARY
    W25QXX
    bluetooth
    mpu
    gps
)

target_include_directories(${PROJECT_NAME} PRIVATE
    ${CONFIG_PATH}
    ${CMAKE_CURRENT_LIST_DIR}/inc
)

add_compile_options(-Wall
        -Wno-format          # int != int32_t as far as the compiler is concerned because gcc has int32_t as long int
        -Wno-unused-function # we have some for the docs that aren't called
        -Wno-maybe-uninitialized
    )

add_subdirectory(libs)

pico_add_extra_outputs(${PROJECT_NAME})
pico_enable_stdio_usb(${PROJECT_NAME} 0)
pico_enable_stdio_uart(${PROJECT_NAME} 0)
