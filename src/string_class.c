#include "string_class.h"
#include <stdbool.h>
#include <string.h>

DEFINE_STACK(char, String);

char *c_str(String *str) {
  if (str == NULL) {
    return NULL;
  }
  push(str, '\0');
  pop(str);
  return str->data;
}

bool equals(String *a, String *b) {
  return a != NULL && b != NULL && ((a == b) || ((a->size == b->size) && strcmp(c_str(a), c_str(b)) == 0));
}

String from_cstr(char *str) {
  String tmp = NewString;
  for (size_t i = 0; str[i]; i++) {
    push(&tmp, str[i]);
  }
  return tmp;
}

void move_into(String *dst, String *src) {
  for (size_t i = 0; i < src->size; i++) {
    push(dst, src->data[i]);
  }
  free(src->data);
  src->data = NULL;
  src->size = 0;
  src->capacity = 0;
}
