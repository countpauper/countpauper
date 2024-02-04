cmake_minimum_required(VERSION 3.20.0)
file(GLOB Engine_SRC "*.cpp")
add_library(Engine ${Engine_SRC})
target_compile_features(Engine PRIVATE cxx_std_17)
set(DEPENDENCIES_DIR ../../Dependencies)
target_include_directories(Engine PRIVATE 
     "${DEPENDENCIES_DIR}/libxml/include/libxml2" 
     "${DEPENDENCIES_DIR}/glew/include")

add_subdirectory("${DEPENDENCIES_DIR}/Stb" "${CMAKE_BINARY_DIR}/Stb")

target_link_libraries(Engine Stb)
