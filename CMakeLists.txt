cmake_minimum_required(VERSION 3.16)

project(miniflow C)

set(LUAJIT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/vendor/luajit)

include(./vendor/luajit-cmake/LuaJIT.cmake)

add_subdirectory(./src/engine ../build/engine)
