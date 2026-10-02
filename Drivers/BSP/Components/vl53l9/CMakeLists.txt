cmake_minimum_required(VERSION 3.27)
message(STATUS "driver_vl53l9")

add_library(driver_vl53l9 OBJECT)
target_sources(driver_vl53l9 PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/vl53l9.c)
target_include_directories(driver_vl53l9 PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_compile_options(driver_vl53l9 PRIVATE -Wall -Wextra)

if(CMAKE_BUILD_TYPE MATCHES "Coverage")
    target_compile_options(driver_vl53l9 PRIVATE --coverage)
endif()
