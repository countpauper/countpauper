cmake_minimum_required(VERSION 3.20.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

project(tactics)

file(GLOB MAIN_SOURCES *.cpp)
add_executable(${PROJECT_NAME} ${MAIN_SOURCES})

include(CTest)
enable_testing()

add_subdirectory(game)
add_subdirectory(../Engine ${CMAKE_BINARY_DIR}/Engine)

#
# Add subdirectories
#
set(DEPENDENCIES_DIR "../../Dependencies")
if(BUILD_TESTS)
        add_subdirectory("${DEPENDENCIES_DIR}/googletest/")
endif()

# add_subdirectory(game)
#target_link_libraries(${PROJECT_NAME}
#     engine)
