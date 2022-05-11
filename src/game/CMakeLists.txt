cmake_minimum_required(VERSION 3.16)

enable_language(CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED 20)
set(CMAKE_BUILD_TYPE Debug)

set(game_source_files
  entities/entity.h
  entities/entity.cpp
  game.h
  game.cpp
  interface.h
  interface.cpp
)

include_directories("..")
include_directories("${GLFW_DIR}/include")
include_directories("${GLAD_DIR}/include")
include_directories("${VULKAN_HEADERS_DIR}/include")
include_directories("${STB_DIR}")

add_library(game ${game_source_files})
target_link_libraries(game PRIVATE logic input state render glfw)
