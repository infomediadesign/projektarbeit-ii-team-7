set(engine_input_source_files
  asset.c
  argparse.c
  input_state.c
  input.c
)

set(engine_input_header_files
  asset.h
  argparse.h
  input_state.h
  input.h
)

add_library(input ${engine_input_source_files} ${engine_input_header_files})
target_link_libraries(input state glfw)
