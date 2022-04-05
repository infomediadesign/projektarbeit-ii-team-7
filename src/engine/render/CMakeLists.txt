set(engine_render_source_files
  render.c
  window.c
)

set(engine_render_header_files
  render.h
  window.h
)

add_library(render ${engine_render_source_files} ${engine_render_header_files})
target_link_libraries(render state)
