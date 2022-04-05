set(engine_state_source_files
  state.c
)

set(engine_state_header_files
  state.h
)

add_library(state ${engine_state_source_files} ${engine_state_header_files})
