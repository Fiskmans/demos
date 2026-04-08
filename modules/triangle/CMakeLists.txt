

list(APPEND files TriangleModule.cpp)
list(APPEND files TriangleModule.h)
list(APPEND files ModuleMain.cpp)
list(APPEND files shaders/Vertex.glsl)
list(APPEND files shaders/Fragment.glsl)

add_library(module_triangle SHARED ${files})

target_link_libraries(module_triangle PRIVATE engine)
target_link_libraries(module_triangle PRIVATE fisk_tools)

target_include_directories(module_triangle PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/../..")
target_include_directories(module_triangle PRIVATE "${sdl_vendored_SOURCE_DIR}/include")
target_include_directories(module_triangle PRIVATE "${fisk_tools_SOURCE_DIR}")


add_custom_command( TARGET  module_triangle POST_BUILD
                    COMMAND mkdir   -p ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/shaders/triangle)

add_custom_command( TARGET  module_triangle POST_BUILD
                    COMMAND glslc   -fshader-stage=vertex
                                    ${CMAKE_CURRENT_SOURCE_DIR}/shaders/Vertex.glsl
                                    -o ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/shaders/triangle/Vertex.spv)

add_custom_command( TARGET  module_triangle POST_BUILD
                    COMMAND glslc   -fshader-stage=fragment
                                    ${CMAKE_CURRENT_SOURCE_DIR}/shaders/Fragment.glsl
                                    -o ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/shaders/triangle/Fragment.spv)