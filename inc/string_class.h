#ifndef STRING
#define STRING

#include "stack.h"
#include <int.h>
#include <stddef.h>

declare_template_stack_c(char, String);
declare_template_stack_c(String, Stack_String);

#define resize(stack, cap) _Generic(*stack, String: resize_String, Stack_String: resize_Stack_String)(stack, cap)
#define at(stack, i) _Generic(*stack, String: at_String, Stack_String: at_Stack_String)(stack, i)
#define push(stack, data) _Generic(*stack, String: push_String, Stack_String: push_Stack_String)(stack, data)
#define pop(stack) _Generic(*stack, String: pop_String, Stack_String: pop_Stack_String)(stack)

char *c_str(String *str);
String from_cstr(char *str);
void move_into(String *dst, String *src);
void String_delete(String *str);

u8 equals_string(String *a, String *b);

#endif
