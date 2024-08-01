#include "string_class.h"
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

define_template_stack_type(char, String);
define_template_stack_type(String, Stack_String);

void free_Stack_String(Stack_String *stack) {
  if (stack == NULL) {
    return;
  }
  for (size_t i = 0; i < stack->size; i++) {
    free_String(at_Stack_String(stack, i));
  }
  stack->capacity = 0;
  stack->size = 0;
  free(stack->data);
  stack->data = NULL;
}

char *c_str(String *str) {
  if (str == NULL) {
    return NULL;
  }
  push_String(str, '\0');
  pop_String(str);
  return str->data;
}

bool equals(String *a, String *b) {
  return a != NULL && b != NULL && ((a == b) || ((a->size == b->size) && strcmp(c_str(a), c_str(b)) == 0));
}

String from_cstr(char *str) {
  String tmp = new_String();
  for (size_t i = 0; str[i]; i++) {
    push_String(&tmp, str[i]);
  }
  return tmp;
}

void String_delete(String *str) {
  free(str->data);
  str->data = NULL;
  str->size = 0;
  str->capacity = 0;
}

bool is_empty(String *str) { return str->size == 0; }

void move_into(String *dst, String *src) {
  if (dst == src) {
    return;
  }
  for (size_t i = 0; i < src->size; i++) {
    push_String(dst, src->data[i]);
  }
  String_delete(src);
}

void free_String(String *stack) {
  if (stack == NULL) {
    return;
  }
  stack->capacity = 0;
  stack->size = 0;
  free(stack->data);
  stack->data = NULL;
}
