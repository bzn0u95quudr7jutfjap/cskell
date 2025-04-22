#include <stdio.h>
#include <string.h>

char *tokentypetostring(TokenType t) {
  // clang-format off
  switch (t) {
    case TOKEN_WORD               : return "word";
    case TOKEN_STRING             : return "string";
    case TOKEN_NUMBER             : return "number";
    case TOKEN_SPECIAL            : return "special";
    case TOKEN_COMMENT_SL         : return "comment_sl";
    case TOKEN_COMMENT_ML         : return "comment_ml";
    case TOKEN_MACRO_BEGIN        : return "macro_begin";
    case TOKEN_MACRO_END          : return "macro_end";
    case TOKEN_BLOCK_BEGIN        : return "block_begin";
    case TOKEN_BLOCK_END          : return "block_end";
    case TOKEN_EXPR_BEGIN         : return "expr_begin";
    case TOKEN_EXPR_END           : return "expr_end";
    case TOKEN_ARRAY_I_BEGIN      : return "array_i_begin";
    case TOKEN_ARRAY_I_END        : return "array_i_end";
    case TOKEN_COMA               : return "coma";
    case TOKEN_END_OF_LINE        : return "end_of_line";
    case TOKEN_OPERATOR_UNARY     : return "operator_unary";
    case TOKEN_OPERATOR_BINARY    : return "operator_binary";
    case TOKEN_OPERATOR_ATTRIBUTE : return "operator_attribute";
    case TOKEN_OPERATOR_PREPOSTFIX: return "operator_prepostfix";
    case TOKEN_OPERATOR_AMBIGUOUS : return "operator_ambiguous";
  }
  // clang-format on
  return NULL;
}

u0 print_char(char c) {
  if (32 < c && c < 127) {
    printf("%c", c);
  } else if (c == '\n') {
    printf("\\n");
  } else if (c == '\r') {
    printf("\\r");
  } else if (c == '\t') {
    printf("\\t");
  } else if (c == ' ') {
    printf(" ");
  } else {
    printf("\\x%d%d", '0' + (c / 16), '0' + (c % 16));
  }
}

u0 print_string(u32 len, char *s) {
  for (u32 i = 0; i < len; i++) {
    print_char(s[i]);
  }
}

void print_tokentype(TokenType t) {
  printf("%s", tokentypetostring(t));
}

void print_codetokens(CodeTokens *codetokens) {
  if(0 == codetokens->tokens.size){
    printf("\n");
  }
  for (u32 i = 0; i < codetokens->tokens.size; i++) {
    Token *t = &codetokens->tokens.data[i];
    print_tokentype(t->type);
    printf(",");
    print_string(t->size, &codetokens->codice.data[t->begin]);
    printf("\n");
  }
}

int main(void) {
  CodeTokens codetokens = {};
  u32        maxsize    = sizeof(codetokens.codice.data);
  char      *str        = codetokens.codice.data;
  fgets(str, maxsize, stdin);
  u32 size               = strlen(str);
  codetokens.codice.size = (size > maxsize) ? maxsize : size;
  tokenizer(&codetokens);
  print_codetokens(&codetokens);
}
