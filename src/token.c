#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include <stack.h>
#include <string_class.h>

u8 is_any_of(char c, size_t size, const char cs[]) {
  for (size_t i = 0; i < size; i++) {
    if (c == cs[i]) {
      return true;
    }
  }
  return false;
}

#define DEFINE_CSET(NAME, CSET)                                                                                        \
  bool NAME(char c) {                                                                                                  \
    static const char *const charset = CSET;                                                                           \
    return is_any_of(c, strlen(charset), charset);                                                                     \
  }

DEFINE_CSET(is_white, " \n\t")
DEFINE_CSET(is_special, "{}()[]#.;,")
DEFINE_CSET(is_string_delimiter, "'\"")
DEFINE_CSET(is_operator, "+-*/%!=&|^><?:")

bool is_name_first(char c) { return (c == '_') || ('a' <= c && c <= 'z') || ('A' <= c && c <= 'Z'); }
bool is_name(char c) { return is_name_first(c) || ('0' <= c && c <= '9'); }
bool is_number1_first(char c) { return ('0' <= c && c <= '9'); }
bool is_number1(char c) {
  return is_number1_first(c) || ('a' <= c && c <= 'f') || ('A' <= c && c <= 'F') || c == 'L' || c == 'x' || c == 'X';
}

String *pushNewString(Stack_String *tokens) {
  push(tokens, new_String());
  return at(tokens, -1);
}

Stack_Token g_tokens = {};

Stack_String tokenizer(String *stream_string) {
  Iter_String stream_string_obj = sseekres(stream_string);
  Iter_String *stream = &stream_string_obj;
  u8 macro = 0;
  char c = EOF;
  Stack_String tokens_obj = new_Stack_String();
  Stack_String *tokens = &tokens_obj;

  free_Stack_Token(&g_tokens);

  while ((c = speekc(stream)) != EOF) {
    if (macro && c == '\n' && speekoffset(stream, -1) != '\\') {
      macro = 0;
      sgetc(stream);
      push(tokens, from_cstr("\n"));
      push(&g_tokens, (Token){.type = TOKEN_MACRO_END});
    } else if (macro && c == '#') {
      push(pushNewString(tokens), sgetc(stream));
      push(&g_tokens, (Token){.type = TOKEN_MACRO_BEGIN});
    } else if (c == '#') {
      macro = 1;
    } else if (is_name_first(c)) {
      String *token = pushNewString(tokens);
      while (is_name(c = sgetc(stream))) {
        push(token, c);
      }
      sseekcur(stream, -1);
      push(&g_tokens, (Token){.type = TOKEN_IDENTIFIER});
    } else if (is_special(c)) {
      String *token = pushNewString(tokens);
      push(token, sgetc(stream));
      push(&g_tokens, (Token){.type = TOKEN_SPECIAL});
    } else if (c == '/' && speekoffset(stream, 1) == '/') {
      String *token = pushNewString(tokens);
      push(token, sgetc(stream));
      while ((c = sgetc(stream)) != '\n' && c != EOF) {
        push(token, c);
      }
      push(&g_tokens, (Token){.type = TOKEN_COMMENT_SL});
    } else if (c == '/' && speekoffset(stream, 1) == '*') {
      String *token = pushNewString(tokens);
      push(token, sgetc(stream));
      while ((c = sgetc(stream)) != EOF && !(c == '*' && speekc(stream) == '/')) {
        push(token, c);
      }
      if (c != EOF) {
        push(token, c);
        push(token, sgetc(stream));
      }
      push(&g_tokens, (Token){.type = TOKEN_COMMENT_ML});
    } else if (is_operator(c)) {
      String *token = pushNewString(tokens);
      push(token, sgetc(stream));
      if (is_operator(c = sgetc(stream))) {
        push(token, c);
      } else {
        sseekcur(stream, -1);
      }
      push(&g_tokens, (Token){.type = TOKEN_OPERATOR});
    } else if (is_number1(c)) {
      String *token = pushNewString(tokens);
      push(token, sgetc(stream));
      while (is_number1(c = sgetc(stream))) {
        push(token, c);
      }
      sseekcur(stream, -1);
      push(&g_tokens, (Token){.type = TOKEN_NUMBER});
    } else if (is_string_delimiter(c)) {
      char delimiter = c;
      String *token = pushNewString(tokens);
      push(token, sgetc(stream));
      while ((c = sgetc(stream)) != EOF && c != delimiter && c != '\n') {
        push(token, c);
        if (c == '\\') {
          push(token, sgetc(stream));
        }
      }
      if (c == delimiter) {
        push(token, c);
      } else {
        sseekcur(stream, -1);
      }
      push(&g_tokens, (Token){.type = TOKEN_STRING});
    } else {
      sgetc(stream);
    }
  }
  if (macro) {
    push(&tokens_obj, from_cstr("\n"));
    push(&g_tokens, (Token){.type = TOKEN_MACRO_END});
  }
  return tokens_obj;
}

Stack_String parse_code_into_words(FILE *stream) {
  String s = new_String();
  fseek(stream, 0, SEEK_SET);
  char c;
  while ((c = fgetc(stream)) != EOF) {
    push(&s, c);
  }
  return tokenizer(&s);
}
