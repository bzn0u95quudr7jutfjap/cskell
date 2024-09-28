#include "string_class.h"
#include <int.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

define_template_stack_c(char, String);
define_template_stack_c(String, Stack_String);
define_template_stack_c(Token, Stack_Token);

void free_Stack_String(Stack_String *stack) {
  if (stack == NULL) {
    return;
  }
  for (size_t i = 0; i < stack->size; i++) {
    free_String(at(stack, i));
  }
  free(stack->data);
  *stack = new_Stack_String();
}

void free_String(String *stack) {
  if (stack == NULL) {
    return;
  }
  free(stack->data);
  *stack = new_String();
}

void free_Stack_Token(Stack_Token *stack) {
  if (stack == NULL) {
    return;
  }
  free(stack->data);
  *stack = new_Stack_Token();
}

char *c_str(String *str) {
  if (str == NULL) {
    return NULL;
  }
  push_String(str, '\0');
  pop_String(str);
  return str->data;
}

u8 equals_string(String *a, String *b) {
  return a != NULL && b != NULL && ((a == b) || ((a->size == b->size) && strcmp(c_str(a), c_str(b)) == 0));
}

String from_cstr(char *str) {
  String tmp = new_String();
  for (size_t i = 0; str[i]; i++) {
    push_String(&tmp, str[i]);
  }
  return tmp;
}

bool is_empty(String *str) { return str->size == 0; }

void move_into(String *dst, String *src) {
  if (dst == src) {
    return;
  }
  for (size_t i = 0; i < src->size; i++) {
    push_String(dst, src->data[i]);
  }
  free_String(src);
}

Iter_String sseekres(String *str) {
  Iter_String i = {.str = str, .idx = 0};
  speekc(&i);
  return i;
}

char speekc(Iter_String *stream) {
  char *c = at(stream->str, stream->idx);
  stream->is_end = c == NULL;
  return c == NULL ? EOF : *c;
}

char sgetc(Iter_String *stream) {
  char c = speekc(stream);
  stream->idx++;
  return c;
}

char speekoffset(Iter_String *stream, int o) {
  char *c = at(stream->str, stream->idx + o);
  return c == NULL ? EOF : *c;
}

void sseekcur(Iter_String *stream, i32 o) { stream->idx += o; }
