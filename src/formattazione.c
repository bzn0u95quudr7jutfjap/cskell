#include <stack.h>
#include <string_class.h>
#include <token.h>

String formattazione(Stack_String *tokens) {
  for (u32 i = 0; i < tokens->size; i++) {
    Token *t = at(&g_tokens, i);
    switch (t->type) {
    TOKEN_IDENTIFIER:
      break;
    TOKEN_STRING:
      break;
    TOKEN_NUMBER:
      break;
    TOKEN_COMMENT_SL:
      break;
    TOKEN_COMMENT_ML:
      break;
    TOKEN_OPERATOR:
      break;
    TOKEN_SPECIAL:
      break;
    TOKEN_MACRO_BEGIN:
      break;
    TOKEN_MACRO_END:
      break;
    TOKEN_TONDA_BEGIN:
      break;
    TOKEN_TONDA_END:
      break;
    TOKEN_QUADRA_BEGIN:
      break;
    TOKEN_QUADRA_END:
      break;
    TOKEN_GRAFFA_BEGIN:
      break;
    TOKEN_GRAFFA_END:
      break;
    }
  }
  return new_String();
}
