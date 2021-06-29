cmake_minimum_required(VERSION 3.18.0)

project(cxx-demo)
set(BINARY cxx-demo)
add_executable(${BINARY} main.cpp)

# Rainway SDK headers use C++ 17 so we will too
set_target_properties(${BINARY}
    PROPERTIES
    CXX_STANDARD 17
)

# Link with the Rainway Runtime
target_link_directories(${BINARY} PRIVATE runtime)
target_link_libraries(${BINARY} PRIVATE RainwayRuntime)

# Copy the necessary dlls to the output folder aswell
add_custom_command(
    TARGET ${BINARY} 
    POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_if_different 
    "${PROJECT_SOURCE_DIR}/runtime/DesktopCapture.dll" $<TARGET_FILE_DIR:${BINARY}>)

add_custom_command(
    TARGET ${BINARY} 
    POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_if_different 
    "${PROJECT_SOURCE_DIR}/runtime/RainwayRuntime.dll" $<TARGET_FILE_DIR:${BINARY}>)
