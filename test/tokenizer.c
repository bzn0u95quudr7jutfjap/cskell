#include "string_class.h"
#include "test.h"
#include <int.h>
#include <stdbool.h>
#include <stdio.h>

extern Stack_Token g_tokens;

int main(int argc, char *argv[]) {
  if (argc < 2) {
    return 0;
  }
  String codice = file_get_content(argv[1]);
  Stack_String token = tokenizer(&codice);
  for (u32 i = 0; i < token.size; i++) {
    printf("%16s: ", type_string(at(&g_tokens, i)->type));
    print_string(at(&token, i));
    printf("\n");
  }
  free_String(&codice);
  free_Stack_String(&token);
  return 0;
}
