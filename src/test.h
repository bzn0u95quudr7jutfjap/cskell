#ifndef TEST_H
#define TEST_H

#include <int.h>
#include <string_class.h>

typedef struct {
  u0 (*function)(Formatter *);
  Formatter input;
  Stack_String atteso;
} test_tokenizer;

typedef struct testdata testdata;

typedef struct {
  u0 (*exec)(testdata *);
  u0 (*print)(testdata *);
  u0 (*free)(testdata *);
} testdata_vtable;

typedef enum {
  CODE_OK,
  CODE_NULL_RESULT,
  CODE_STR_NULL_RESULT,
  CODE_TOKEN_STR_LEN_DIFFER,
  CODE_TOKEN_LEN_DIFFER,
  CODE_TOKEN_STR_DIFFER,
} testcode;

struct testdata {
  char *name;
  testcode code;
  testdata_vtable vtable;
  union {
    test_tokenizer tokenizer;
  };
};

u0 print_stack_string(Stack_String *output);
u0 print_formatter(Formatter *output);
testcode equals_formatter_strings(Formatter *a, Stack_String *b);

char *status(testdata *t);
u8 success(testdata *t);

u0 print_string(String *s);

#define declare_test_function(type) extern testdata_vtable functions_##type

#define define_test_function_internal(type, execname, execbody, printname, printbody, freename, freebody)              \
  u0 execname(testdata *t);                                                                                            \
  u0 printname(testdata *t);                                                                                           \
  u0 freename(testdata *t);                                                                                            \
  u0 execname(testdata *t) { execbody };                                                                               \
  u0 printname(testdata *t) { printbody };                                                                             \
  u0 freename(testdata *t) { freebody };                                                                               \
  testdata_vtable functions_##type = {.exec = execname, .print = printname, .free = freename}

#define define_test(type, exec, print, free)                                                                           \
  define_test_function_internal(type, exec_##type, exec, print_##type, print, free_##type, free)

u0 exec_test(testdata *t);
u0 print_test(testdata *t);
u0 free_test(testdata *t);

declare_test_function(test_tokenizer);

String file_get_content(char *fn);
char *type_string(token_type t);

#endif
