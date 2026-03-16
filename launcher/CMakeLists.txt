list(APPEND files App.cpp)
list(APPEND files App.h)
list(APPEND files main.cpp)

add_executable(launcher ${files})

set_property(TARGET launcher PROPERTY ENABLE_EXPORTS 1)

target_link_libraries(launcher PUBLIC engine)
target_link_libraries(launcher PUBLIC SDL3::SDL3)

target_include_directories(launcher PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/..")
target_include_directories(launcher PUBLIC "${sdl_vendored_SOURCE_DIR}/include")