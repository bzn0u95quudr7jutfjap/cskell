cmake_minimum_required(VERSION 3.5)
project(haskellinator)

set(compileflags -xc -Isrc)

set(functions resize      at        push        pop)
set(args      stack,cap   stack,i   stack,data  stack)

set(types String Stack_String Stack_Token)

foreach(f a IN ZIP_LISTS functions args)
  set(line "")
  foreach(t IN LISTS types)
    list(APPEND line "${t}:${f}_${t}")
  endforeach()
  list(JOIN line ", " line_str)
  add_compile_options("-D${f}(${a})=_Generic(*stack, ${line_str})(${a})")
  list(APPEND compileflags "-D${f}(${a})=_Generic(*stack, ${line_str})(${a})")
endforeach()

set(cffile "${PROJECT_SOURCE_DIR}/compile_flags.txt")
if(NOT EXISTS ${cffile})
  list(JOIN compileflags "\n" compileflags)
  write_file(${cffile} "${compileflags}")
  message(STATUS "Generato il ${cffile}")
endif()
#message(FATAL_ERROR "${compileflags}")

include_directories(src/)

set(options -g -fsanitize=address)
add_compile_options(${options})
add_link_options(${options})

add_library(haskellformat SHARED src/token.c src/string_class.c)
add_library(testing       SHARED src/test.c)

add_executable(${PROJECT_NAME} src/main.c)
target_link_libraries(${PROJECT_NAME} haskellformat)

add_executable(run_tests src/test_main.c)
target_link_libraries(run_tests haskellformat testing)

add_executable(tokenizer src/main_tokenizer.c)
target_link_libraries(tokenizer haskellformat testing)
