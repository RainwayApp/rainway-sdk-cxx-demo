cmake_minimum_required(VERSION 3.18.0)

project(cxx-demo)
set(PROJECT cxx-demo)
add_executable(${PROJECT} main.cpp)

# Rainway SDK headers use C++ 17 so we will too
set_target_properties(${PROJECT}
    PROPERTIES
    CXX_STANDARD 17
)

# Link with the Rainway Runtime
target_link_directories(${PROJECT} PRIVATE runtime)
target_link_libraries(${PROJECT} PRIVATE RainwayRuntime)

# Copy the necessary dlls to the output folder aswell
add_custom_command(
    TARGET ${PROJECT} 
    POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_if_different 
    "${PROJECT_SOURCE_DIR}/runtime/DesktopCapture.dll" $<TARGET_FILE_DIR:${PROJECT}>)

add_custom_command(
    TARGET ${PROJECT} 
    POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_if_different 
    "${PROJECT_SOURCE_DIR}/runtime/RainwayRuntime.dll" $<TARGET_FILE_DIR:${PROJECT}>)

if(NOT EXISTS ${CMAKE_SOURCE_DIR}/runtime)
    message("Downloading Rainway Runtime")
    file(DOWNLOAD https://sdk-builds.rainway.com/cpp/0.1.1.zip ${CMAKE_SOURCE_DIR}/runtime.zip SHOW_PROGRESS)

    make_directory(${CMAKE_SOURCE_DIR}/runtime/)

    execute_process(
        COMMAND ${CMAKE_COMMAND} -E tar xzf ${CMAKE_SOURCE_DIR}/runtime.zip 
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/runtime/)
endif()
