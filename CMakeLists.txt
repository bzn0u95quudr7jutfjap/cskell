cmake_minimum_required(VERSION 3.5)
project(haskellinator)

include_directories(inc/)

set(C_SOURCES 
  src/string_class.c
  src/stack_string.c
)

add_compile_options("-g")

add_executable(${PROJECT_NAME} src/main.c ${C_SOURCES})

add_executable(run_tests test/test.c ${C_SOURCES})
