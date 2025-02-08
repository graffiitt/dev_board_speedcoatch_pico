cmake_minimum_required(VERSION 3.12)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

add_library(bluetooth INTERFACE)

target_include_directories(bluetooth  INTERFACE 
    ${CMAKE_CURRENT_LIST_DIR}/include
    # ${CMAKE_CURRENT_LIST_DIR}/../../configs
)
target_link_libraries(bluetooth  INTERFACE 
    pico_btstack_ble
    pico_btstack_cyw43
    pico_cyw43_arch_sys_freertos
)
target_sources(bluetooth  INTERFACE
    ${CMAKE_CURRENT_LIST_DIR}/src/ble_hrt_client.c
    ${CMAKE_CURRENT_LIST_DIR}/src/bluetooth_core.c
)
pico_btstack_make_gatt_header(bluetooth INTERFACE "${CMAKE_CURRENT_LIST_DIR}/mygatt.gatt")

target_compile_definitions(bluetooth INTERFACE
    CYW43_TASK_STACK_SIZE=1024
    CYW43_LWIP=0
)