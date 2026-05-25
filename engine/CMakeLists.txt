FetchContent_MakeAvailable(imgui)

list(APPEND files Engine.cpp)
list(APPEND files Engine.h)
list(APPEND files Module.cpp)
list(APPEND files Module.h)
list(APPEND files SDLHelper.cpp)
list(APPEND files SDLHelper.h)

find_package(OpenGL REQUIRED)

add_library(engine "${files}")

target_link_libraries(engine PUBLIC SDL3::SDL3)
target_link_libraries(engine PUBLIC fisk_tools)
target_link_libraries(engine PUBLIC imgui_wrapper)
target_link_libraries(engine PUBLIC OpenGL::GL)

target_include_directories(engine PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/..")
target_include_directories(engine PUBLIC "${sdl_vendored_SOURCE_DIR}/include")
target_include_directories(engine PUBLIC "${fisk_tools_SOURCE_DIR}/include")
target_include_directories(engine PUBLIC ${imgui_SOURCE_DIR}) 