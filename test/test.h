#ifndef TEST_H
#define TEST_H

#include <int.h>
#include <string_class.h>
#include <token.h>

u8 equals_stack_string(Stack_String *a, Stack_String *b);
u0 print_stack_string(Stack_String *output);

typedef struct {
  Stack_String (*function)(String *);
  String input;
  Stack_String output;
  Stack_String atteso;
} test_tokenizer;

typedef struct testdata testdata;

typedef struct {
  u0 (*exec)(testdata *);
  u0 (*print)(testdata *);
  u0 (*free)(testdata *);
} testdata_vtable;

struct testdata {
  char *name;
  u32 code;
  testdata_vtable vtable;
  union {
    test_tokenizer tokenizer;
  };
};

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
