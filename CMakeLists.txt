cmake_minimum_required(VERSION 3.10)
project(cskell)

include_directories("${CMAKE_CURRENT_LIST_DIR}/src/")
include_directories("${CMAKE_CURRENT_LIST_DIR}/src/tests/")

set(options
  --std=c23
  -Wall
  -Werror
  -Wpedantic
  -include${CMAKE_CURRENT_LIST_DIR}/inc/int.h
  -include${CMAKE_CURRENT_LIST_DIR}/inc/cskell.h

  #Debug
  -g
  -fsanitize=address

  #Release
  #-fsanitize=safe-stack
)

add_compile_options(${options})
add_link_options(${options})

add_subdirectory("src/tests/")

list(JOIN options "\n" compileflags)
file(WRITE "${CMAKE_CURRENT_LIST_DIR}/compile_flags.txt" "${compileflags}")

add_library(haskellformat SHARED
  src/token.c
  src/format.c
  src/io.c
)

add_executable(${PROJECT_NAME} src/main.c)
target_link_libraries(${PROJECT_NAME} haskellformat)
