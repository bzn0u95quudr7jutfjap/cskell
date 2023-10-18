cmake_minimum_required(VERSION 3.5)
project(STYLE-FORMATTER)

set(C_SOURCES 
  src/main.c
)
add_executable(${PROJECT_NAME} ${C_SOURCES})
