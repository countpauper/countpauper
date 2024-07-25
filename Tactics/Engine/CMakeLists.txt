cmake_minimum_required(VERSION 3.20.0)
file(GLOB_RECURSE Engine_SRC CONFIGURE_DEPENDS "source/*.cpp" )
add_library(Engine ${Engine_SRC})
target_compile_features(Engine PRIVATE cxx_std_23)
set(DEPENDENCIES_DIR ../../Dependencies)

project(Engine)
find_package(OpenGL REQUIRED COMPONENTS OpenGL EGL)
find_package(GLEW REQUIRED)
find_package(GLUT REQUIRED)
add_subdirectory("${DEPENDENCIES_DIR}/Stb" "${CMAKE_CURRENT_BINARY_DIR}/Stb")
set(JSON_BuildTests OFF CACHE INTERNAL "")
add_subdirectory("${DEPENDENCIES_DIR}/json/" ${CMAKE_BINARY_DIR}/json)

target_include_directories(Engine PUBLIC "include" PRIVATE
     "${DEPENDENCIES_DIR}/libxml/include/libxml2"
     ${GLEW_INCLUDE_DIRS})

target_link_libraries(Engine
        Stb
        GLUT::GLUT
        nlohmann_json::nlohmann_json
        OpenGL::EGL
        ${OPENGL_LIBRARIES}
        ${GLEW_LIBRARIES})

if(BUILD_TESTS)
        include(CTest)
        if (NOT TARGET gtest)
                add_subdirectory("${DEPENDENCIES_DIR}/googletest/" ${CMAKE_CURRENT_BINARY_DIR}/googletest)
        endif()
        enable_testing()
        add_subdirectory(unittests)
endif()
