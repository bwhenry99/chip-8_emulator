project(rlimgui)

add_library(rlimgui)
file(GLOB ${PROJECT_NAME}_SRCS CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")
target_sources(rlimgui PRIVATE ${${PROJECT_NAME}_SRCS})
target_include_directories(rlimgui PUBLIC "${CMAKE_CURRENT_SOURCE_DIRECTORY}/")
target_link_libraries(${PROJECT_NAME} PUBLIC imgui raylib)