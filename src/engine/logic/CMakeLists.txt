set(engine_logic_source_files
  logic.c
)

set(engine_logic_header_files
  logic.h
)

add_library(logic ${engine_logic_source_files} ${engine_logic_header_files})
target_link_libraries(logic state)
