cmake_minimum_required(VERSION 3.10)

function(template_stack_functions_h typedefinition type stack outvar)
  set(${outvar} " 
    ${typedefinition}
    typedef struct {
      u32 capacity;
      u32 size;
      ${type} * data;
    } ${stack};
    ${stack} new_${stack}();
    u0 resize_${stack}(${stack} * stack, u32 capacity);
    ${type} * at_${stack}(${stack} * stack, u32 i);
    u0 push_${stack}(${stack} * stack, ${type} data);
    u0 pop_${stack}(${stack} * stack);
    u0 free_${stack}(${stack} * stack);
  " PARENT_SCOPE)
endfunction()

function(template_stack_functions_c type stack outvar)
  set(${outvar} "
    ${stack} new_${stack}(){
      return (${stack}){.capacity = 0, .size = 0, .data = NULL};
    }
    u0 resize_${stack}(${stack} * stack, u32 capacity){
      stack->data = realloc(stack->data,sizeof(*(stack->data)) * (stack->capacity = capacity));
      stack->size = stack->capacity < stack->size ? stack->capacity : stack->size;
    }
    ${type} * at_${stack}(${stack} * stack, u32 i){
      return i < stack->size?&stack->data[i]:(-i <= stack->size ? &stack->data[stack->size+i] : NULL);
    }
    u0 push_${stack}(${stack} * stack, ${type} data){
      if(stack->size >= stack->capacity){
        resize_${stack}(stack, stack->capacity * 1.5 + 1);
      }
      stack->data[stack->size++] = data;
    }
    u0 pop_${stack}(${stack} * stack){
      if(stack->size < stack->capacity / 3){
        resize_${stack}(stack, stack->capacity / 2);
      }
      stack->size += -(stack->size > 0);
    }
  " PARENT_SCOPE)
endfunction()

function(template_iter_functions_h type stack iter outvar)
  set(${outvar} "
    typedef struct {
      u32 i;
      ${stack} * stack;
    } ${iter};
    ${iter} ibegin_${stack}(${stack} * stack);
    u8 ieos_${iter}(${iter} * iter);
    ${type} * ipeekcur_${iter}(${iter} * iter, u32 o);
    ${type} * iseekcur_${iter}(${iter} * iter, u32 o);
  " PARENT_SCOPE)
endfunction()

function(template_iter_functions_c type stack iter outvar)
  set(${outvar} "
    ${iter} ibegin_${stack}(${stack} * stack){
      return (${iter}){.i = 0, .stack = stack};
    }
    u8 ieos_${iter}(${iter} * iter){
      return !(0 <= iter->i && iter->i < iter->stack->size);
    }
    ${type} * ipeekcur_${iter}(${iter} * iter, u32 o){
      return ieos_${iter}(iter) ? NULL : at_${stack}(iter->stack,iter->i + o);
    }
    ${type} * iseekcur_${iter}(${iter} * iter, u32 o){
      iter->i += o;
      return ipeekcur_${iter}(iter,0);
    }
  " PARENT_SCOPE)
endfunction()

function(template_generics funcs args types outvar)
  foreach(f a IN ZIP_LISTS funcs args)
    string(REPLACE "," ";" p "${a}")
    list(POP_FRONT p q)

    set(g "")
    foreach(t IN LISTS types)
      list(APPEND g "${t}:${f}_${t}")
    endforeach()
    list(JOIN g ", " g)

    set(generic "#define ${f}(${a}) _Generic((*(${q})), ${g})(${a})")
    list(APPEND generics "${generic}")
  endforeach()
  set(${outvar} "${generics}" PARENT_SCOPE)
endfunction()

function(template_stack_functions_ls types ls)
endfunction()

template_stack_functions_h(
  ""
  char
  String
  string_h
)
template_stack_functions_h(
  ""
  String
  StackString
  stack_string_h
)
template_stack_functions_h(
  "
  typedef enum {
    TOKEN_WORD = 1,
    TOKEN_STRING,
    TOKEN_CHAR,
    TOKEN_NUMBER,
    TOKEN_OPERATOR,
    TOKEN_SPECIAL,
    TOKEN_COMMENT_SL,
    TOKEN_COMMENT_ML,
    TOKEN_MACRO_BEGIN,
    TOKEN_MACRO_END,
    TOKEN_BLOCK_BEGIN,
    TOKEN_BLOCK_END,
    TOKEN_EXPR_BEGIN,
    TOKEN_EXPR_END,
    TOKEN_ARRAY_I_BEGIN,
    TOKEN_ARRAY_I_END,
    TOKEN_COMA,
    TOKEN_END_OF_LINE,
    TOKEN_OPERATOR_UNARY,
    TOKEN_OPERATOR_BINARY,
    TOKEN_OPERATOR_ATTRIBUTE,
    TOKEN_OPERATOR_PREPOSTFIX,
    TOKEN_OPERATOR_AMBIGUOUS,
  } TokenType;

  typedef struct {
    TokenType type;
    u32 begin;
    u32 size;
    u32 indentation;
    u8 newline_after : 2;
    u8 space_after : 1;
    u8 definition : 1;
  } Token;

  "
  Token
  StackToken
  stack_token_h
)

template_stack_functions_h(
  "
  typedef struct {
    u0 (*function)(Formatter *);
    Formatter input;
    u32 size;
    String *atteso;
  } TestTokenizer;

  typedef struct Test Test;

  typedef struct {
    u0 (*exec)(Test *);
    u0 (*print)(Test *);
    u0 (*free)(Test *);
  } TestVTable;

  typedef enum {
    CODE_OK,
    CODE_NOT_RUN,
    CODE_NULL_RESULT,
    CODE_STR_NULL_RESULT,
    CODE_TOKEN_STR_LEN_DIFFER,
    CODE_TOKEN_LEN_DIFFER,
    CODE_TOKEN_STR_DIFFER,
  } TestCode;

  struct Test {
    char *name;
    TestCode code;
    TestVTable vtable;
    union {
      TestTokenizer tokenizer;
    };
  };
  "
  Test
  StackTest
  StackTest_h
)

template_iter_functions_h(char String IterString iter_string_h)
template_iter_functions_h(Token Formatter IterFormatter iter_formatter_h)

template_stack_functions_c(char String string_c)
template_stack_functions_c(Token StackToken stack_token_c)

template_stack_functions_c(Test StackTest StackTest_c)

template_iter_functions_c(char String IterString iter_string_c)

template_generics(
  "at;resize;push;pop"
  "stack,i;stack,c;stack,data;stack"
  "String;StackToken"
  stack_macros
)

template_generics(
  "ibegin"
  "stack"
  "String;Formatter"
  ibegin_macros
)

template_generics(
  "ieos;ipeekcur;iseekcur"
  "iter;iter,o;iter,o"
  "IterString;IterFormatter"
  iter_macros
)

list(JOIN stack_macros "\n" stack_macros)
list(JOIN ibegin_macros "\n" ibegin_macros)
list(JOIN iter_macros "\n" iter_macros)
#message(STATUS "\n\n${s_stack_macros}\n\n${s_ibegin_macros}\n\n${s_iter_macros}\n\n")

set(stack_h "

#ifndef TEMPLATE_STACK_H
#define TEMPLATE_STACK_H

#ifndef NULL
#define NULL ((void *)0)
#endif

void * realloc(void *, unsigned long);

#include \"int.h\"

${string_h}
${stack_token_h}
${iter_string_h}
typedef struct {
  String codice;
  StackToken tokens;
} Formatter;
${iter_formatter_h}

${stack_macros}
${ibegin_macros}
${iter_macros}

String ipeekstringcur(IterFormatter *stream, u32 o);

${StackTest_h}

#endif // TEMPLATE_STACK_H

")

set(iter IterFormatter)
set(stack Formatter)
set(type Token)
set(stack_c "

${string_c}
${stack_token_c}
${iter_string_c}

${StackTest_c}

")

# message(STATUS "${stack_h}\n\n\n${stack_c}")
# message(FATAL_ERROR "DIE")

set(path_stack_h "${CMAKE_CURRENT_LIST_DIR}/stack.h")
set(path_stack_c "${CMAKE_CURRENT_LIST_DIR}/stack.c")
set(path_cmake   "${CMAKE_CURRENT_LIST_FILE}")

if(NOT EXISTS "${path_stack_h}" OR "${path_cmake}" IS_NEWER_THAN "${path_stack_h}")
  message(STATUS "\n> ${path_cmake}\n> ${path_stack_h}")
  file(WRITE "${path_stack_h}" "${stack_h}")
endif()

if(NOT EXISTS "${path_stack_c}" OR "${path_cmake}" IS_NEWER_THAN "${path_stack_c}")
  message(STATUS "\n> ${path_cmake}\n> ${path_stack_c}")
  file(WRITE "${path_stack_c}" "${stack_c}")
endif()

set(options "-include${path_stack_h}")
add_compile_options(${options})
add_link_options(${options})

add_library(stack SHARED "${path_stack_c}")
