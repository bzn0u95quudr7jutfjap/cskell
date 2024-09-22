#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <stack.h>
#include <string_class.h>
#include <token.h>

size_t string_index;

void sseekres() { string_index = 0; }

char sgetc(String *stream) {
  char *c = at(stream, string_index++);
  return c == NULL ? EOF : *c;
}

char speekc(String *stream) {
  char *c = at(stream, string_index);
  return c == NULL ? EOF : *c;
}

char speekoffset(String *stream, int o) {
  char *c = at(stream, string_index + o);
  return c == NULL ? EOF : *c;
}

void sseekcur(int o) { string_index += o; }

bool is_any_of(char c, size_t size, const char cs[]) {
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
DEFINE_CSET(is_operator, "+-*/%!=&|^><")

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

u8 is_macro(String *stream) { return 1; }

String extract_macro(String *stream) {
  String macro = new_String();
  char c;
  while ((c = sgetc(stream)) != EOF && !(c != '\\' && speekc(stream) == '\n')) {
    push(&macro, c);
  }
  return macro;
}

Stack_String tokenizer(String *stream) {
  char c = EOF;
  Stack_String tokens_obj = new_Stack_String();
  Stack_String *tokens = &tokens_obj;

  for (sseekres(); (c = speekc(stream)) != EOF;) {
    if (c == '#' && is_macro(stream)) {
      push(pushNewString(tokens), sgetc(stream));
      if (speekc(stream) == '#') {
        push(pushNewString(tokens), sgetc(stream));
      } else {
        String macro = extract_macro(stream);
        u32 old = string_index;
        Stack_String macro_tokens = tokenizer(&macro);
        for (u32 i = 0; i < macro_tokens.size; i++) {
          push(tokens, *at(&macro_tokens, i));
        }
        free_String(&macro);
        free(macro_tokens.data);
        push(tokens, from_cstr("\n"));
        string_index = old;
      }
    } else if (is_name_first(c)) {
      String *token = pushNewString(tokens);
      while (is_name(c = sgetc(stream))) {
        push(token, c);
      }
      sseekcur(-1);
    } else if (is_special(c)) {
      String *token = pushNewString(tokens);
      push(token, sgetc(stream));
    } else if (c == '/' && speekoffset(stream, 1) == '/') {
      String *token = pushNewString(tokens);
      push(token, sgetc(stream));
      while ((c = sgetc(stream)) != '\n' && c != EOF) {
        push(token, c);
      }
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
    } else if (is_operator(c)) {
      String *token = pushNewString(tokens);
      push(token, sgetc(stream));
      if (is_operator(c = sgetc(stream))) {
        push(token, c);
      } else {
        sseekcur(-1);
      }
    } else if (is_number1(c)) {
      String *token = pushNewString(tokens);
      push(token, sgetc(stream));
      while (is_number1(c = sgetc(stream))) {
        push(token, c);
      }
      sseekcur(-1);
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
        sseekcur(-1);
      }
    } else {
      sgetc(stream);
    }
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
