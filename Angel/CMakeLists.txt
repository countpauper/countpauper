cmake_minimum_required(VERSION 3.20.0)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED True)

project(angel)

file(GLOB SOURCES CONFIGURE_DEPENDS *.cpp)
add_executable(${PROJECT_NAME} ${SOURCES})

#
# Add subdirectories
#
set(DEPENDENCIES_DIR "../Dependencies")
if (NOT TARGET gtest)
        add_subdirectory("${DEPENDENCIES_DIR}/googletest/" ${CMAKE_BINARY_DIR}/googletest)
endif()

include(CTest)
enable_testing()

add_subdirectory(Parser)
add_subdirectory(Logic)

target_link_libraries(${PROJECT_NAME}
     Parser
     Logic)