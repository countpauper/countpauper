cmake_minimum_required(VERSION 3.20.0)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED True)

project(tactics)

file(GLOB SOURCES CONFIGURE_DEPENDS *.cpp)
add_executable(${PROJECT_NAME} ${SOURCES})

# Infer the MinGW/MSYS2 path from the compiler
get_filename_component(MINGW_BIN_DIR ${CMAKE_C_COMPILER} DIRECTORY)
get_filename_component(MINGW_PATH ${MINGW_BIN_DIR} DIRECTORY)  # Path to /mingw64

# Set CMAKE_PREFIX_PATH to the inferred path
set(CMAKE_PREFIX_PATH ${MINGW_PATH})
message(STATUS "Using MinGW path: ${MINGW_PATH}")

#
# Add subdirectories
#
set(DEPENDENCIES_DIR "../../Dependencies")
if(BUILD_TESTS)
        if (NOT TARGET gtest)
                add_subdirectory("${DEPENDENCIES_DIR}/googletest/" ${CMAKE_BINARY_DIR}/googletest)
        endif()
endif()

include(CTest)
enable_testing()

add_subdirectory(../Engine ${CMAKE_CURRENT_BINARY_DIR}/Engine)
add_subdirectory(Game)


target_link_libraries(${PROJECT_NAME}
     Game
     Engine)

message("Copy bins to ${CMAKE_BINARY_DIR}")
execute_process(COMMAND ${CMAKE_COMMAND} -E copy_if_different
    "${CMAKE_PREFIX_PATH}/bin/libfreeglut.dll"
    "${CMAKE_BINARY_DIR}"
)
