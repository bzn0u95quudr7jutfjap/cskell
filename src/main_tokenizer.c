#include "string_class.h"
#include "test.h"
#include <int.h>
#include <stdbool.h>
#include <stdio.h>

int main(int argc, char *argv[]) {
  if (argc < 2) {
    return 0;
  }
  String view = {};
  Formatter tokens = {};
  tokens.str = file_get_content(argv[1]);
  tokenizer(&tokens);
  for (u32 i = 0; i < tokens.tokens.size; i++) {
    Token *t = at(&tokens.tokens, i);
    printf("%16s: ", type_string(t->type));
    view.data = tokens.str.data + t->begin;
    view.size = t->size;
    print_string(&view);
    printf("\n");
  }
  free_Formatter(&tokens);
  return 0;
}
