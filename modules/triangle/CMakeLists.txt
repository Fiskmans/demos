

list(APPEND files TriangleModule.cpp)
list(APPEND files TriangleModule.h)
list(APPEND files ModuleMain.cpp)

add_library(module_triangle SHARED ${files})

target_link_libraries(module_triangle PRIVATE engine)
target_link_libraries(module_triangle PRIVATE fisk_tools)

target_include_directories(module_triangle PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/../..")
target_include_directories(module_triangle PRIVATE "${sdl_vendored_SOURCE_DIR}/include")
target_include_directories(module_triangle PRIVATE "${fisk_tools_SOURCE_DIR}")