cmake_minimum_required(VERSION 3.20.0)
file(GLOB SRC *.c *.cpp)

get_filename_component(MODULE ${CMAKE_CURRENT_LIST_DIR} NAME)

add_library(${MODULE} ${SRC})
target_compile_features(${MODULE} PRIVATE cxx_std_17)
target_include_directories(${MODULE} PUBLIC .)