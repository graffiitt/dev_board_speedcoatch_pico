cmake_minimum_required(VERSION 3.12)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

add_library(mpu INTERFACE) 

target_include_directories(mpu  INTERFACE 
    ${CMAKE_CURRENT_LIST_DIR}/include
)
target_link_libraries(mpu  INTERFACE 
    hardware_dma
    hardware_i2c
)
target_sources(mpu  INTERFACE
    ${CMAKE_CURRENT_LIST_DIR}/src/mpu.c
)
