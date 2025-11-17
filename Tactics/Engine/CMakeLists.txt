cmake_minimum_required(VERSION 3.20.0)
file(GLOB_RECURSE Engine_SRC CONFIGURE_DEPENDS "source/*.cpp" )
project(Engine)
add_library(${PROJECT_NAME} ${Engine_SRC})
target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_23)
set(DEPENDENCIES_DIR ../../Dependencies)


find_package(OpenGL REQUIRED COMPONENTS OpenGL) #EGL
find_package(GLEW REQUIRED)
find_package(GLUT REQUIRED)
add_subdirectory("${DEPENDENCIES_DIR}/Stb" "${CMAKE_CURRENT_BINARY_DIR}/Stb")
set(JSON_BuildTests OFF CACHE INTERNAL "")
add_subdirectory("${DEPENDENCIES_DIR}/json/" ${CMAKE_BINARY_DIR}/json)

target_include_directories(${PROJECT_NAME} PUBLIC "include" PRIVATE
     "${DEPENDENCIES_DIR}/libxml/include/libxml2"
     ${GLEW_INCLUDE_DIRS})

# For some reason  find_package(OpenGL REQUIRED COMPONENTS ... EGL) doesn't find EGL
# so add it manually
set(EGL_INCLUDE_DIR "${CMAKE_PREFIX_PATH}/include")
#set(EGL_LIBRARY "${CMAKE_PREFIX_PATH}/lib/libEGL.a")
include_directories(${EGL_INCLUDE_DIR})
cmake_path(CONVERT "${CMAKE_PREFIX_PATH}/lib/libEGL.a" TO_CMAKE_PATH_LIST EGL_LIBRARY)


target_link_libraries(${PROJECT_NAME}
        Stb
        GLUT::GLUT
        nlohmann_json::nlohmann_json
#        OpenGL::EGL
        "${EGL_LIBRARY}"
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

