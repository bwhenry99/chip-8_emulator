add_library(raylib INTERFACE)

target_include_directories(raylib INTERFACE include)
target_link_directories(raylib INTERFACE lib)
target_link_libraries(raylib INTERFACE raylibdll)
