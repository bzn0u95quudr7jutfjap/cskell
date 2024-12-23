#include "string_class.h"
#include <int.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void free_String(String *stack) {
  if (stack == NULL) {
    return;
  }
  free(stack->data);
  *stack = new_String();
}

void free_StackToken(StackToken *stack) {
  if (stack == NULL) {
    return;
  }
  free(stack->data);
  *stack = new_StackToken();
}

u0 free_Formatter(Formatter *fmt) {
  free_String(&fmt->codice);
  free_StackToken(&fmt->tokens);
}

char *c_str(String *str) {
  if (str == NULL) {
    return NULL;
  }
  push_String(str, '\0');
  pop_String(str);
  return str->data;
}
