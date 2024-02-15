cmake_minimum_required(VERSION 3.20.0)
file(GLOB Engine_SRC "source/*.cpp")
add_library(Engine ${Engine_SRC})
target_compile_features(Engine PRIVATE cxx_std_17)
set(DEPENDENCIES_DIR ../../Dependencies)

project(Engine)
find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)
find_package(GLUT REQUIRED)
add_subdirectory("${DEPENDENCIES_DIR}/Stb" "${CMAKE_CURRENT_BINARY_DIR}/Stb")

target_include_directories(Engine PUBLIC "include" PRIVATE 
     "${DEPENDENCIES_DIR}/libxml/include/libxml2" 
     ${GLEW_INCLUDE_DIRS})

target_link_libraries(Engine 
        Stb
        GLUT::GLUT
        ${OPENGL_LIBRARIES}
        ${GLEW_LIBRARIES})

if(BUILD_TESTS)
        add_subdirectory(unittests)
endif()