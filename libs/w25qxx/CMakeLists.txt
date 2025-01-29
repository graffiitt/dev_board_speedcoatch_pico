cmake_minimum_required(VERSION 3.12)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

add_library(W25QXX INTERFACE)
target_include_directories(W25QXX  INTERFACE ${CMAKE_CURRENT_LIST_DIR}/include)
target_sources(W25QXX  INTERFACE
    ${CMAKE_CURRENT_LIST_DIR}/src/w25qxx.c
)