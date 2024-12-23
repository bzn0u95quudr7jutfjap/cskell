cmake_minimum_required(VERSION 3.10)
project(cskell)

include_directories("${CMAKE_CURRENT_LIST_DIR}/src/")
include_directories("${CMAKE_CURRENT_LIST_DIR}/src/stack/")
include_directories("${CMAKE_CURRENT_LIST_DIR}/src/tests/")
#include_directories("${CMAKE_CURRENT_LIST_DIR}/src/types/")

set(options
  --std=c23
  -g
  -fsanitize=address
  -Wall
  -Werror
  -Wpedantic
  #"-include${CMAKE_CURRENT_LIST_DIR}/src/types/types.h"
  "-include${CMAKE_CURRENT_LIST_DIR}/src/stack/stack.h"
  #"-include${CMAKE_CURRENT_LIST_DIR}/src/test.h"
)

add_compile_options(${options})
add_link_options(${options})

add_subdirectory("src/stack/")
add_subdirectory("src/tests/")
#add_subdirectory("src/types/")

list(JOIN options "\n" compileflags)
file(WRITE "${CMAKE_CURRENT_LIST_DIR}/compile_flags.txt" "${compileflags}")

add_library(haskellformat SHARED
  src/string_class.c
  src/token.c
  src/format.c
  src/io.c
)
target_link_libraries(haskellformat stack)

add_executable(${PROJECT_NAME} src/main.c)
target_link_libraries(${PROJECT_NAME} haskellformat)
