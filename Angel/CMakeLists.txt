cmake_minimum_required(VERSION 3.20.0)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED True)

project(Angel)

file(GLOB SOURCES CONFIGURE_DEPENDS Angel/*.cpp)
add_executable(${PROJECT_NAME} ${SOURCES})

message("Building: ${CMAKE_BUILD_TYPE}")

# Configure testing
if(BUILD_WITH_TESTS)
        include(CTest)
        enable_testing()
endif()

#
# Add subdirectories
#
set(DEPENDENCIES_DIR "../Dependencies")
if (NOT TARGET gtest)
        add_subdirectory("${DEPENDENCIES_DIR}/googletest/" ${CMAKE_BINARY_DIR}/googletest)
endif()

add_subdirectory(Interpreter)
add_subdirectory(Logic)
add_subdirectory(Engine)

# Copy data files
configure_file("Engine/angel.bnf" "angel.bnf" COPYONLY)

target_link_libraries(${PROJECT_NAME}
        Engine)
