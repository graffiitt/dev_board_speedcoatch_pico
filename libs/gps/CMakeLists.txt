cmake_minimum_required(VERSION 3.12)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

add_library(gps INTERFACE) 

target_include_directories(gps  INTERFACE 
    ${CMAKE_CURRENT_LIST_DIR}/include
)
target_link_libraries(gps  INTERFACE 
    hardware_uart
)
target_sources(gps  INTERFACE
    ${CMAKE_CURRENT_LIST_DIR}/src/gps.c
    ${CMAKE_CURRENT_LIST_DIR}/src/uart_parser.c
)
