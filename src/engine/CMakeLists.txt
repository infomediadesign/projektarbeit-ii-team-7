cmake_minimum_required(VERSION 3.16)

project(miniflow_bin C CXX)

set(CMAKE_C_STANDARD 11)
set(CMAKE_BUILD_TYPE Debug)
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

if (NOT WIN32)
  set(GLFW_USE_WAYLAND ON CACHE BOOL "" FORCE)
endif()

find_package(Threads REQUIRED)

set(engine_source_files
  platform.c
  platform.h
  util.h
  crc64.c
  crc64.h
  main.c
)

set(GLAD_DIR ../../vendor/glad)
set(VULKAN_HEADERS_DIR ../../vendor/vulkan-headers)

include_directories("..")
include_directories("${GLAD_DIR}/include")
include_directories("${VULKAN_HEADERS_DIR}/include")

add_subdirectory(../../vendor/glfw ../../build/glfw)
add_subdirectory(state ../../build/state)
add_subdirectory(input ../../build/input)
add_subdirectory(logic ../../build/logic)
add_subdirectory(render ../../build/render)
add_subdirectory(../game ../../build/game)

set_target_properties(game PROPERTIES
  IMPORTED_LOCATION ../../build/game/libgame.a
  IMPORTED_LINK_INTERFACE_LANGUAGES "CXX"
)

add_executable(miniflow ${engine_source_files})

target_link_libraries(miniflow PRIVATE logic input state render glfw Threads::Threads game)

if (APPLE)
  target_link_libraries(${PROJECT_NAME} "-framework IOKit")
  target_link_libraries(${PROJECT_NAME} "-framework Cocoa")
  target_link_libraries(${PROJECT_NAME} "-framework OpenGL")
endif()
