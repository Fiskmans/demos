

list(APPEND files ChaosModule.cpp)
list(APPEND files ChaosModule.h)
list(APPEND files ChaoticPath.h)
list(APPEND files LorenzSystem.cpp)
list(APPEND files LorenzSystem.h)
list(APPEND files ModuleMain.cpp)

add_library(module_chaos SHARED ${files})

target_link_libraries(module_chaos PRIVATE engine)
target_link_libraries(module_chaos PRIVATE fisk_tools)

target_include_directories(module_chaos PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/../..")
target_include_directories(module_chaos PRIVATE "${sdl_vendored_SOURCE_DIR}/include")
target_include_directories(module_chaos PRIVATE "${fisk_tools_SOURCE_DIR}")


add_custom_command( TARGET  module_chaos POST_BUILD
                    COMMAND mkdir   -p ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/shaders/chaos)

add_custom_command( TARGET  module_chaos POST_BUILD
                    COMMAND glslc   -fshader-stage=vertex
                                    ${CMAKE_CURRENT_SOURCE_DIR}/shaders/Vertex.glsl
                                    -o ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/shaders/chaos/Vertex.spv)

add_custom_command( TARGET  module_chaos POST_BUILD
                    COMMAND glslc   -fshader-stage=fragment
                                    ${CMAKE_CURRENT_SOURCE_DIR}/shaders/Fragment.glsl
                                    -o ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/shaders/chaos/Fragment.spv)