cmake_minimum_required(VERSION 3.5)
project(haskellinator)

include_directories(inc/)

file(GLOB c_src src/*.c)
file(GLOB c_test test/*.c)

list(FILTER c_src EXCLUDE REGEX "src\\/main\\.c")

add_compile_options(-g)
add_link_options(-g)

add_compile_options(-fsanitize=address)
add_link_options(-fsanitize=address)

add_executable(${PROJECT_NAME} src/main.c ${c_src})

add_executable(run_tests ${c_test} ${c_src})
