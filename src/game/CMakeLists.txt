cmake_minimum_required(VERSION 3.16)

if (NOT WIN32)
  set(CMAKE_C++_COMPILER /nix/store/8wk22c5p7igqr7s9nmlnfrraq2lj2313-gcc-wrapper-10.3.0/bin/g++)
endif()

enable_language(CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED 11)
set(CMAKE_BUILD_TYPE Debug)

set(game_source_files
  game.h
  game.cpp
)

include_directories("..")

add_library(game ${game_source_files})
target_link_libraries(game PRIVATE logic input state render)
