set(engine_input_source_files
  argparse.c
)

set(engine_input_header_files
  argparse.h
)

add_library(input ${engine_input_source_files} ${engine_input_header_files})
target_link_libraries(input state)
