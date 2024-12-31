cmake_minimum_required(VERSION 3.12)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

add_library(UI_LIBRARY INTERFACE)
target_include_directories(UI_LIBRARY  INTERFACE ${CMAKE_CURRENT_LIST_DIR}/include)
target_link_libraries(UI_LIBRARY  INTERFACE 
SHARPDISP
BITMAPSHAPES
BITMAPTEXT
BITMAPIMAGE
button_handler
)
target_sources(UI_LIBRARY  INTERFACE
    ${CMAKE_CURRENT_LIST_DIR}/src/ui_menu.c
)