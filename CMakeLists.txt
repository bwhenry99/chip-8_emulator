cmake_minimum_required(VERSION 4.4)

project(CHIP-8)

set(CMAKE_CXX_STANDARD 20)

add_executable(CHIP-8)

file(GLOB ${CMAKE_PROJECT_NAME}_SRCS CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")
file(GLOB ${CMAKE_PROJECT_NAME}_HDRS CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/src/*.hpp")

target_sources(${CMAKE_PROJECT_NAME} PRIVATE ${${CMAKE_PROJECT_NAME}_SRCS} PUBLIC ${${CMAKE_PROJECT_NAME}_HDRS})

target_include_directories("${CMAKE_PROJECT_NAME}" PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/src")
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/third_party/raylib-6.0_win64_msvc16/")
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/third_party/imgui-1.89.7-docking/")
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/third_party/rlImgui/")
target_link_libraries("${CMAKE_PROJECT_NAME}" PRIVATE raylib imgui rlimgui)

add_custom_command(
TARGET ${CMAKE_PROJECT_NAME} POST_BUILD
COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_CURRENT_SOURCE_DIR}/third_party/raylib-6.0_win64_msvc16/lib/raylib.dll" $<TARGET_FILE_DIR:${CMAKE_PROJECT_NAME}>
COMMAND_EXPAND_LISTS
)