cmake_minimum_required(VERSION 2.8)

project(cli_example)
set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -std=c++11")
add_executable(${PROJECT_NAME} "main.cpp" "main.h")

if (WIN32)
    target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR}/lib/thirdParty/nlohmann/)
    target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR}/lib/hs_datalog/)
    target_link_libraries(${PROJECT_NAME} ${PROJECT_SOURCE_DIR}/lib/hs_datalog/libhs_datalog.dll)
    set_target_properties(${PROJECT_NAME} PROPERTIES LINKER_LANGUAGE CXX)

    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${PROJECT_SOURCE_DIR}/lib/hs_datalog/libhs_datalog.dll"
        "${CMAKE_BINARY_DIR}/libhs_datalog.dll")

endif (WIN32)


if (UNIX)

    target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR}/lib/thirdParty/nlohmann/)
    target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR}/lib/hs_datalog/)
    target_link_libraries(${PROJECT_NAME} ${PROJECT_SOURCE_DIR}/lib/hs_datalog/libhs_datalog.so)
    target_link_libraries(${PROJECT_NAME} pthread)
    set_target_properties(${PROJECT_NAME} PROPERTIES LINKER_LANGUAGE CXX)

endif (UNIX)




