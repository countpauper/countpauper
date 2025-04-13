cmake_minimum_required(VERSION 3.20.0)
enable_testing()

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED True)

project(LogicTest CXX)
add_compile_options(-Wall -Wextra -pedantic -Werror)

file(GLOB SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_LIST_DIR}/*.cpp")
file(GLOB HEADERS "*.h")
set(EXE_NAME ${PROJECT_NAME})
add_executable(${EXE_NAME} ${SOURCES} ${HEADERS})

target_link_libraries(${EXE_NAME} PUBLIC
    Logic
    gtest_main
    gmock_main
)

add_test(
        NAME ${PROJECT_NAME}
        COMMAND ${EXE_NAME}
)