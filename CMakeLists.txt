cmake_minimum_required(VERSION 3.5)

project(2048-AI)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Config
set(BUILD_WITH_SANITIZERS OFF)
set(BUILD_EMIT_DEBUG_INFO ON)
set(BUILD_OPTIMIZATION_LEVEL 3)

if(BUILD_WITH_SANITIZERS)
  add_compile_options(-fsanitize=undefined,address)
  add_link_options(-fsanitize=undefined,address)
endif()

if(BUILD_EMIT_DEBUG_INFO)
  add_compile_options(-g)
  add_link_options(-g)
endif()

add_compile_options(-O${BUILD_OPTIMIZATION_LEVEL})

add_subdirectory(src)
