project(miniflow_bin C)

set(CMAKE_C_STANDARD 11)

include(../../cmake/raylib.cmake)

find_package(Threads REQUIRED)

set(engine_source_files
  platform.c
  platform.h
  util.c
  util.h
  main.c
)

add_subdirectory(state ../../build/state)
add_subdirectory(input ../../build/input)
add_subdirectory(logic ../../build/logic)
add_subdirectory(render ../../build/render)

add_executable(miniflow ${engine_source_files})

target_link_libraries(miniflow PRIVATE logic input state render raylib Threads::Threads)

if (APPLE)
  target_link_libraries(${PROJECT_NAME} "-framework IOKit")
  target_link_libraries(${PROJECT_NAME} "-framework Cocoa")
  target_link_libraries(${PROJECT_NAME} "-framework OpenGL")
endif()
