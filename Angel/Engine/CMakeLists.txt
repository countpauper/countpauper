set(MODULE_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/include/")
file(GLOB MODULE_SOURCES CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)
get_filename_component(MODULE ${CMAKE_CURRENT_LIST_DIR} NAME)

add_library(${MODULE} STATIC ${MODULE_SOURCES})
target_include_directories(${MODULE} PUBLIC ${MODULE_INCLUDE_DIR})

add_subdirectory(unittests)

target_link_libraries(${MODULE} PUBLIC
     Parser
     Logic)