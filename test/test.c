#include "test.h"
#include "string_class.h"
#include <stdlib.h>

u0 exec_test(testdata *t) { t->vtable.exec(t); }
u0 print_test(testdata *t) { t->vtable.print(t); }
u0 free_test(testdata *t) { t->vtable.free(t); }

u8 success(testdata *t) { return t->code == 0; }
char *status(testdata *t) { return success(t) ? "OK" : "ERR"; }

define_test(
    test_tokenizer,
    {
      t->tokenizer.output = t->tokenizer.function(&t->tokenizer.input);
      t->code = equals_stack_string(&t->tokenizer.output, &t->tokenizer.atteso) ? 0 : 1;
    },
    {
      printf("%6s :: %s\n", "INIZIO", t->name);
      printf("%6s :: [%3s] :: %10s :: ", "STATUS", status(t), "Atteso");
      print_stack_string(&t->tokenizer.atteso);
      printf("\n");
      printf("%6s :: [%3s] :: %10s :: ", "STATUS", status(t), "Output");
      print_stack_string(&t->tokenizer.output);
      printf("\n");
      printf("%6s :: %s\n", "FINE", t->name);
    },
    {
      free_String(&t->tokenizer.input);
      free_Stack_String(&t->tokenizer.output);
      free_Stack_String(&t->tokenizer.atteso);
    });

u0 resize_string(String *s, u32 cap) { s->data = realloc(s->data, s->capacity = cap); }

u0 print_string(String *s) {
  String buffer = new_String();
  resize_string(&buffer, s->size * 2);
  for (u32 i = 0; i < s->size; i++) {
    char c = *at_String(s, i);
    if (32 < c && c < 127) {
      push_String(&buffer, c);
    } else {
      u8 g = c;
      switch (g) {
      case ' ':
        push_String(&buffer, g);
        break;
      case '\t':
        push_String(&buffer, '\\');
        push_String(&buffer, 't');
        break;
      case '\n':
        push_String(&buffer, '\\');
        push_String(&buffer, 'n');
        break;
      case '\r':
        push_String(&buffer, '\\');
        push_String(&buffer, 'r');
        break;
      default:
        push_String(&buffer, '\\');
        push_String(&buffer, 'x');
        push_String(&buffer, '0' + (g / 16));
        push_String(&buffer, '0' + (g % 16));
        break;
      };
    }
  }
  printf("{%s}", c_str(&buffer));
  free_String(&buffer);
}

u0 print_stack_string(Stack_String *output) {
  for (u32 i = 0; i < output->size; i++) {
    print_string(at(output, i));
  }
}

u8 equals_stack_string(Stack_String *a, Stack_String *b) {
  if (a == b) {
    return 1;
  }
  if (a == NULL || b == NULL) {
    return 0;
  }
  if (a->size != b->size) {
    return 0;
  }
  for (u32 i = 0; i < a->size; i++) {
    if (!equals_string(at(a, i), at(b, i))) {
      return 0;
    }
  }
  return 1;
}

String file_get_content(char *fn) {
  String tmp = new_String();
  char c = EOF;
  FILE *f = fopen(fn, "r");
  if (f == NULL) {
    return tmp;
  }
  while ((c = fgetc(f)) != EOF) {
    push_String(&tmp, c);
  }
  fclose(f);
  return tmp;
}

char *type_string(token_type t) {
  switch (t) {
  case TOKEN_UNIDENTIFIED:
    return "NONE";
    break;
  case TOKEN_IDENTIFIER:
    return "ID";
    break;
  case TOKEN_NUMBER:
    return "NUM";
    break;
  case TOKEN_STRING:
    return "STR";
    break;
  case TOKEN_COMMENT_SL:
    return "CMSL";
    break;
  case TOKEN_COMMENT_ML:
    return "CMML";
    break;
  case TOKEN_OPERATOR:
    return "OP";
    break;
  case TOKEN_SPECIAL:
    return "SPE";
    break;
  case TOKEN_MACRO_BEGIN:
    return "MACRO B";
    break;
  case TOKEN_MACRO_END:
    return "MACRO E";
    break;
  }
}
