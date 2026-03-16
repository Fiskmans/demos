
cmake_minimum_required(VERSION 3.31)

project(demos)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIGURATION>")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIGURATION>")


Include(FetchContent)

FetchContent_Declare(
  sdl_vendored
  GIT_REPOSITORY https://github.com/libsdl-org/SDL.git 
  GIT_TAG        0246598d6241c25ddb5c8311fe95318ca8511956
)

FetchContent_Declare(
  fisk_tools
  GIT_REPOSITORY https://github.com/Fiskmans/fisk_tools.git
  GIT_TAG        fc7dfbceaefd9e0a864081a6c53216fc15d84b6a
)

FetchContent_Declare(
  imgui
  GIT_REPOSITORY https://github.com/ocornut/imgui.git
  GIT_TAG        29fadad1939ab62adcc5cd9339db7a784de90120
  CONFIGURE_COMMAND ""
  BUILD_COMMAND ""
)


option(FISK_DEMOS_VENDORED "Use vendored libraries" OFF)

if(FISK_DEMOS_VENDORED)
FetchContent_MakeAvailable(sdl_vendored)
else()
find_package(SDL3 REQUIRED CONFIG REQUIRED COMPONENTS SDL3-shared)
endif()

FetchContent_MakeAvailable(fisk_tools)

add_subdirectory(launcher)
add_subdirectory(engine)
add_subdirectory(imgui)
add_subdirectory(modules)