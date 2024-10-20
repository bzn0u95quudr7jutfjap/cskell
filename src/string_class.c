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

u0 free_Formatter(Formatter *fmt) {
  free_String(&fmt->str);
  free_Stack_Token(&fmt->tokens);
}

char *c_str(String *str) {
  if (str == NULL) {
    return NULL;
  }
  push_String(str, '\0');
  pop_String(str);
  return str->data;
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

u8 s_is_end(Iter_String *stream) { return !(0 <= stream->idx && stream->idx < stream->str->size); }

char speekc(Iter_String *stream) { return s_is_end(stream) ? EOF : *at(stream->str, stream->idx); }

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
