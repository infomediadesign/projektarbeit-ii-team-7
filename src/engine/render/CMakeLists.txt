set(engine_render_source_files
  state.c
  geyser.c
  render.c
)

set(engine_render_header_files
  state.h
  geyser.h
  render.h
)

set(GLAD_DIR ../../../vendor/glad)
set(GLAD_VK_FILES ${GLAD_DIR}/src/vulkan.c ${GLAD_DIR}/include/glad/vulkan.h)

include_directories("${GLAD_DIR}/include")

add_library(render ${engine_render_source_files} ${engine_render_header_files} ${GLAD_VK_FILES})
target_link_libraries(render state glfw)
