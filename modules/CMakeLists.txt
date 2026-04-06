add_subdirectory(chaos)
add_subdirectory(triangle)

add_dependencies(launcher module_chaos)
add_dependencies(launcher module_triangle)