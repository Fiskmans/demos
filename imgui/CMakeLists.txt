FetchContent_MakeAvailable(imgui)

list(APPEND files "${imgui_SOURCE_DIR}/imconfig.h")
list(APPEND files "${imgui_SOURCE_DIR}/imgui.h" "${imgui_SOURCE_DIR}/imgui.cpp")
list(APPEND files "${imgui_SOURCE_DIR}/imgui_demo.cpp")
list(APPEND files "${imgui_SOURCE_DIR}/imgui_draw.cpp")
list(APPEND files "${imgui_SOURCE_DIR}/imgui_internal.h")
list(APPEND files "${imgui_SOURCE_DIR}/imgui_tables.cpp")
list(APPEND files "${imgui_SOURCE_DIR}/imgui_widgets.cpp")
list(APPEND files "${imgui_SOURCE_DIR}/imstb_rectpack.h")
list(APPEND files "${imgui_SOURCE_DIR}/imstb_textedit.h")
list(APPEND files "${imgui_SOURCE_DIR}/imstb_truetype.h")
list(APPEND files "imgui_tree.h")

list(APPEND files "${imgui_SOURCE_DIR}/misc/cpp/imgui_stdlib.cpp")
list(APPEND files "${imgui_SOURCE_DIR}/misc/cpp/imgui_stdlib.h")

list(APPEND files "${imgui_SOURCE_DIR}/backends/imgui_impl_sdl3.cpp")
list(APPEND files "${imgui_SOURCE_DIR}/backends/imgui_impl_sdl3.h")
list(APPEND files "${imgui_SOURCE_DIR}/backends/imgui_impl_sdlgpu3.cpp")
list(APPEND files "${imgui_SOURCE_DIR}/backends/imgui_impl_sdlgpu3.h")

add_library(imgui_wrapper ${files})

target_compile_definitions(imgui_wrapper PRIVATE SDL_ENABLE_OLD_NAMES)

target_include_directories(imgui_wrapper PRIVATE "${sdl_vendored_SOURCE_DIR}/include")
target_include_directories(imgui_wrapper PUBLIC ${imgui_SOURCE_DIR})
target_include_directories(imgui_wrapper PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
