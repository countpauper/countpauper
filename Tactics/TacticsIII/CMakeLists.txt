cmake_minimum_required(VERSION 3.20.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

project(tactics)

file(GLOB SOURCES *.cpp)
add_executable(${PROJECT_NAME} ${SOURCES})

#
# Add subdirectories
#
set(DEPENDENCIES_DIR "../../Dependencies")
if(BUILD_TESTS)
        add_subdirectory("${DEPENDENCIES_DIR}/googletest/" ${CMAKE_BINARY_DIR}/googletest)
endif()

include(CTest)
enable_testing()

add_subdirectory(../Engine ${CMAKE_CURRENT_BINARY_DIR}/Engine)
add_subdirectory(Game)

target_link_libraries(${PROJECT_NAME}
     Game
     Engine)
