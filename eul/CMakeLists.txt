cmake_minimum_required(VERSION 3.20.0)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED True)
project(eul)

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


set(MODULE_INCLUDE_DIR "include/")
file(GLOB MODULE_SOURCES CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)
get_filename_component(MODULE ${CMAKE_CURRENT_LIST_DIR} NAME)

add_library(${MODULE} STATIC ${MODULE_SOURCES})
target_include_directories(${MODULE} PUBLIC ${MODULE_INCLUDE_DIR})

add_subdirectory(unittests)