cmake_minimum_required(VERSION 3.16)

enable_language(CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED 20)

set(game_source_files
  ${JSON_DIR}/simdjson.h
  ${JSON_DIR}/simdjson.cpp
  vector.h
  vector.cpp
  level.h
  level.cpp
  animation.h
  animation.cpp
  entities/entity.h
  entities/entity.cpp
  entities/player.h
  entities/player.cpp
  entities/entity_manager.h
  entities/entity_manager.cpp
  controllers/controller.h
  controllers/battle_controller.h
  controllers/battle_controller.cpp
  controllers/dungeon_controller.h
  controllers/dungeon_controller.cpp
  controllers/menu_controller.h
  controllers/menu_controller.cpp
  controllers/overworld_controller.h
  controllers/overworld_controller.cpp
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
include_directories("${JSON_DIR}")
include_directories("${LUA_DIR}")

add_library(game ${game_source_files})
target_link_libraries(game PRIVATE logic input state render glfw libluajit)
