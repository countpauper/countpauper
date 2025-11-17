cmake_minimum_required(VERSION 3.20.0)
enable_testing()

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED True)

project(EngineTest CXX)
add_compile_options(-Wall -Wextra -pedantic -Werror)

file(GLOB SOURCES CONFIGURE_DEPENDS "*.cpp")
file(GLOB HEADERS "*.h")
set(EXE_NAME ${PROJECT_NAME})
add_executable(${EXE_NAME} ${SOURCES} ${HEADERS})


# Copy binary needed to run unittests
set(UNITTEST_BINARY_DIR "${CMAKE_BINARY_DIR}/unittests")
execute_process(COMMAND ${CMAKE_COMMAND} -E copy_if_different
    "${CMAKE_PREFIX_PATH}/bin/glew32.dll"
    "${UNITTEST_BINARY_DIR}"
)
execute_process(COMMAND ${CMAKE_COMMAND} -E copy_if_different
    "${CMAKE_SOURCE_DIR}/data/test.png"
    "${UNITTEST_BINARY_DIR}"
)

target_link_libraries(${EXE_NAME} PUBLIC
    Engine
    gtest_main
    gmock_main
)


add_test(
        NAME ${PROJECT_NAME}
        COMMAND ${EXE_NAME}
)
