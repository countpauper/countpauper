cmake_minimum_required(VERSION 3.20.0)
enable_testing()

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED True)

project(GameTest CXX)
add_compile_options(-Wall -Wextra -pedantic -Werror)

file(GLOB SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_LIST_DIR}/*.cpp")
file(GLOB HEADERS "*.h")
set(EXE_NAME ${PROJECT_NAME})
add_executable(${EXE_NAME} ${SOURCES} ${HEADERS})

target_link_libraries(${EXE_NAME} PUBLIC
    Game
    gtest_main
    gmock_main
)

# Copy binary needed to run unittests
set(UNITTEST_BINARY_DIR "${CMAKE_BINARY_DIR}/Game/unittests")
message("Copy ${CMAKE_PREFIX_PATH}/bin/glew32.dll to ${UNITTEST_BINARY_DIR}")
execute_process(COMMAND ${CMAKE_COMMAND} -E copy_if_different
    "${CMAKE_PREFIX_PATH}/bin/glew32.dll"
    "${UNITTEST_BINARY_DIR}"
)

add_test(
        NAME ${PROJECT_NAME}
        COMMAND ${EXE_NAME}
)
