project(miniflow_bin C)

set(engine_source_files
  platform.c
  platform.h
  main.c
)

add_subdirectory(state ../../build/state)
add_subdirectory(input ../../build/input)

add_executable(miniflow ${engine_source_files})

target_link_libraries(miniflow input state)
