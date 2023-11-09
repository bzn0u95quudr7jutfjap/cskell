cmake_minimum_required(VERSION 3.5)
project(haskellinator)

set(C_SOURCES 
  src/main.c
  src/string_class.c
  src/stack_string.c
)

add_compile_options("-g")

add_executable(${PROJECT_NAME} ${C_SOURCES})
