

list(APPEND files ChaosModule.cpp)
list(APPEND files ChaosModule.h)
list(APPEND files LorenzAttractor.cpp)
list(APPEND files LorenzAttractor.h)
list(APPEND files ModuleMain.cpp)

add_library(module_chaos SHARED ${files})

target_link_libraries(module_chaos PRIVATE engine)
target_link_libraries(module_chaos PRIVATE fisk_tools)

target_include_directories(module_chaos PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/../..")
target_include_directories(module_chaos PRIVATE "${sdl_vendored_SOURCE_DIR}/include")
target_include_directories(module_chaos PRIVATE "${fisk_tools_SOURCE_DIR}")