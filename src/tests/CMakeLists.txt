cmake_minimum_required(VERSION 3.10)

function(splice str sep head tail)
  string(LENGTH "${str}" strlen)
  string(LENGTH "${sep}" seplen)
  string(FIND "${str}" "${sep}" i)
  math(EXPR j "${i}+${seplen}")
  string(SUBSTRING "${str}" 0 "${i}" h)
  string(SUBSTRING "${str}" "${j}" "${strlen}" t)
  set(${head} "${h}" PARENT_SCOPE)
  set(${tail} "${t}" PARENT_SCOPE)
endfunction()

function(parsetest test str rest)
  splice("${str}" "\n" name     str)
  splice("${str}" "\n" input    str)
  splice("${str}" "\n" strings  str)
  splice("${str}" "\n" types    str)
  splice("${str}" "\n" _        str)

  #message(STATUS "name   : ${name}")
  #message(STATUS "input  : ${input}")
  #message(STATUS "strings: ${strings}")
  #message(STATUS "types  : ${types}")
  #message(STATUS "_      : ${_}")

  string(REGEX REPLACE "([^\\])\" \"" "\\1\"\n\"" strings "${strings}")
  string(REGEX REPLACE "(\"\") \"" "\\1\n\"" strings "${strings}")
  set(strings "${strings}\n")

  string(REGEX REPLACE " " "\n" types "${types}")
  set(types  "${types}\n")


  set(count 0)
  splice("${strings}"  "\n" string strings)
  splice("${types}"    "\n" type   types)
  while(("${string}" STRGREATER "") OR ("${type}" STRGREATER ""))
    string(PREPEND type "token_")
    string(TOUPPER "${type}" type)
    set(attesi "${attesi}\t{.type = ${type}, .size = sizeof(${string})-1, .data = ${string}},\n")

    splice("${strings}" "\n" string strings)
    splice("${types}" "\n" type types)
    math(EXPR count "${count}+1")
  endwhile()

  set(${test} "
    {
      .nome = \"${name}\",
      .code = CODE_NOT_RUN,
      .puppet = {
        .codice = {
          .size = sizeof(${input})-1,
          .data = ${input},
        },
      },
      .attesi_n = ${count},
      .attesi   = {
          ${attesi}
        }
    },
  " PARENT_SCOPE)
  set(${rest} "${str}" PARENT_SCOPE)
endfunction()

file(READ "${CMAKE_CURRENT_LIST_DIR}/testcases.txt" testcases)

while("${testcases}" STRGREATER "")
  parsetest(test "${testcases}" testcases)

  set(g_tests "${g_tests}${test}")
endwhile()

set(testcases "

typedef enum {
  CODE_OK,
  CODE_NOT_RUN,
  CODE_NULL_RESULT,
  CODE_STR_NULL_RESULT,
  CODE_TOKEN_STR_LEN_DIFFER,
  CODE_TOKEN_LEN_DIFFER,
  CODE_TOKEN_STR_DIFFER,
} TestCode;

typedef struct {
  TokenType type;
  u32       size;
  char     *data;
} TokenPattern;

typedef struct {
  char        *nome;
  TestCode     code;
  CodeTokens   puppet;
  u32          attesi_n;
  TokenPattern attesi[256];
} TestCase;

TestCase tokenizer_tests[] = {
  ${g_tests}
};

")
file(WRITE "${CMAKE_CURRENT_LIST_DIR}/testcases.c" "${testcases}")

add_executable(run_tests "${CMAKE_CURRENT_LIST_DIR}/main.c")
target_link_libraries(run_tests haskellformat)
