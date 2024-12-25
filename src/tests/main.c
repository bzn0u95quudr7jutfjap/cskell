#include "testcases.c"
#include <int.h>
#include <stdio.h>
#include <string.h>

u8 success(TestCase *t) {
  return t->code == CODE_OK;
}
char *status(TestCase *t) {
  return success(t) ? "OK" : "ERR";
}

char *tokentypetostring(TokenType t) {
  // clang-format off
  switch (t) {
    case TOKEN_WORD               : return "TOKEN_WORD";
    case TOKEN_STRING             : return "TOKEN_STRING";
    case TOKEN_NUMBER             : return "TOKEN_NUMBER";
    case TOKEN_SPECIAL            : return "TOKEN_SPECIAL";
    case TOKEN_COMMENT_SL         : return "TOKEN_COMMENT_SL";
    case TOKEN_COMMENT_ML         : return "TOKEN_COMMENT_ML";
    case TOKEN_MACRO_BEGIN        : return "TOKEN_MACRO_BEGIN";
    case TOKEN_MACRO_END          : return "TOKEN_MACRO_END";
    case TOKEN_BLOCK_BEGIN        : return "TOKEN_BLOCK_BEGIN";
    case TOKEN_BLOCK_END          : return "TOKEN_BLOCK_END";
    case TOKEN_EXPR_BEGIN         : return "TOKEN_EXPR_BEGIN";
    case TOKEN_EXPR_END           : return "TOKEN_EXPR_END";
    case TOKEN_ARRAY_I_BEGIN      : return "TOKEN_ARRAY_I_BEGIN";
    case TOKEN_ARRAY_I_END        : return "TOKEN_ARRAY_I_END";
    case TOKEN_COMA               : return "TOKEN_COMA";
    case TOKEN_END_OF_LINE        : return "TOKEN_END_OF_LINE";
    case TOKEN_OPERATOR_UNARY     : return "TOKEN_OPERATOR_UNARY";
    case TOKEN_OPERATOR_BINARY    : return "TOKEN_OPERATOR_BINARY";
    case TOKEN_OPERATOR_ATTRIBUTE : return "TOKEN_OPERATOR_ATTRIBUTE";
    case TOKEN_OPERATOR_PREPOSTFIX: return "TOKEN_OPERATOR_PREPOSTFIX";
    case TOKEN_OPERATOR_AMBIGUOUS : return "TOKEN_OPERATOR_AMBIGUOUS";
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
  printf("{");
  for (u32 i = 0; i < len; i++) {
    print_char(s[i]);
  }
  printf("}");
}

u0 exec_TestTokenizer(TestCase *test) {
  printf("%-12s %6s :: %s\n", "ESECUZIONE", "INIZIO", test->nome);
  CodeTokens *puppet = &test->puppet;
  tokenizer(puppet);
  if (puppet->tokens.size != test->attesi_n) {
    test->code = CODE_TOKEN_LEN_DIFFER;
    return;
  }
  for (u32 i = 0; i < test->attesi_n; i++) {
    Token *t       = &puppet->tokens.data[i];
    char  *a       = &puppet->codice.data[t->begin];
    char  *b       = test->attesi[i].data;
    u32    cmpsize = test->attesi[i].size;
    if (t->size != cmpsize) {
      test->code = CODE_TOKEN_STR_LEN_DIFFER;
      return;
    }
    if (0 != strncmp(a, b, cmpsize)) {
      test->code = CODE_TOKEN_STR_DIFFER;
      return;
    }
  }
  test->code = CODE_OK;
  printf("%-12s %6s :: %s\n", "ESECUZIONE", "FINE", test->nome);
}

u0 print_TestTokenizer(TestCase *test) {
  printf("%-12s %6s :: %s\n", "RISULTATO", "INIZIO", test->nome);
  printf("%6s :: [%3s] :: %10s :: ", "STATUS", status(test), "Atteso");
  for (u32 i = 0; i < test->attesi_n; i++) {
    u32   len = test->attesi[i].size;
    char *s   = test->attesi[i].data;
    print_string(len, s);
  }
  printf("\n");
  printf("%6s :: [%3s] :: %10s :: ", "STATUS", status(test), "Output");
  for (u32 i = 0; i < test->puppet.tokens.size; i++) {
    u32   len = test->puppet.tokens.data[i].size;
    char *s   = test->puppet.codice.data + test->puppet.tokens.data[i].begin;
    print_string(len, s);
  }
  printf("\n");
  switch (test->code) {
  case CODE_TOKEN_STR_DIFFER:
    printf("%6s :: [%3s] :: %s\n", "STATUS", status(test), "CODE_TOKEN_STR_DIFFER");
    break;
  case CODE_TOKEN_LEN_DIFFER:
    printf("%6s :: [%3s] :: %s\n", "STATUS", status(test), "CODE_TOKEN_LEN_DIFFER");
    printf("%6s :: [%3s] :: %10s :: LEN : %6u\n", "STATUS", status(test), "Atteso", test->attesi_n);
    printf("%6s :: [%3s] :: %10s :: LEN : %6u\n", "STATUS", status(test), "Output", test->puppet.tokens.size);
    break;
  default:
    break;
  }
  if (test->code != CODE_OK) {
    for (u32 i = 0; i < test->puppet.tokens.size; i++) {
      Token *t = test->puppet.tokens.data + i;
      char  *a = test->puppet.codice.data + t->begin;
      printf("%6s :: [%3s] :: ", "STATUS", status(test));
      printf("%26s ", tokentypetostring(t->type));
      printf("%6d ", t->begin);
      print_string(t->size, a);
      printf("\n");
    }
  }
  printf("%-12s %6s :: %s\n", "RISULTATO", "FINE", test->nome);
}

int main(int argc, char *argv[]) {

  printf("\n");
  for (u32 i = 0; i < count(tokenizer_tests); i++) {
    TestCase *t = &tokenizer_tests[i];
    exec_TestTokenizer(t);
    print_TestTokenizer(t);
    printf("\n");
  }

  union {
    struct {
      u32 failures;
      u32 successes;
    };
    u32 vals[2];
  } report = {};

  for (u32 i = 0; i < count(tokenizer_tests); i++) {
    TestCase *t = &tokenizer_tests[i];
    report.vals[success(t)]++;
  }

  printf("%6lu : Totale di test svolti\n", count(tokenizer_tests));
  printf("%6d : Successi\n", report.successes);
  printf("%6d : Fallimenti\n", report.failures);

  printf("\n");
  printf("\n");

  return 0;
}
