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

u0 push_macro_begin(Stack_Token *tokens, Iter_String *stream) {
  push(tokens, ((Token){.begin = stream->idx, .size = 1, .type = TOKEN_MACRO_BEGIN}));
  sgetc(stream);
}

u0 push_macro_end(Stack_Token *tokens, Iter_String *stream) {
  push(tokens, ((Token){.begin = stream->idx, .size = 1, .type = TOKEN_MACRO_END}));
  sgetc(stream);
}

u0 push_identifier(Stack_Token *tokens, Iter_String *stream) {
  push(tokens, ((Token){.begin = stream->idx, .size = 0, .type = TOKEN_IDENTIFIER}));
  Token *t = at(tokens, -1);
  while (is_name(sgetc(stream))) {
    t->size++;
  }
  sseekcur(stream, -1);
}

u0 push_special(Stack_Token *tokens, Iter_String *stream) {
  push(tokens, ((Token){.begin = stream->idx, .size = 1, .type = TOKEN_SPECIAL}));
  sgetc(stream);
}

u0 push_comment_sline(Stack_Token *tokens, Iter_String *stream) {
  push(tokens, ((Token){.begin = stream->idx, .size = 0, .type = TOKEN_COMMENT_SL}));
  Token *t = at(tokens, -1);
  char c;
  while ((c = sgetc(stream)) != '\n' && c != EOF) {
    t->size++;
  }
}

u0 push_comment_mline(Stack_Token *tokens, Iter_String *stream) {
  push(tokens, ((Token){.begin = stream->idx, .size = 0, .type = TOKEN_COMMENT_ML}));
  Token *t = at(tokens, -1);
  char c;
  while ((c = sgetc(stream)) != EOF && !(c == '*' && speekc(stream) == '/')) {
    t->size++;
  }
  if (c != EOF) {
    t->size += 2;
    sgetc(stream);
  }
}

u0 push_operator(Stack_Token *tokens, Iter_String *stream) {
  push(tokens, ((Token){.begin = stream->idx, .size = 1, .type = TOKEN_OPERATOR}));
  sgetc(stream);
  if (is_operator(speekc(stream))) {
    at(tokens, -1)->size++;
    sgetc(stream);
  }
}

u0 push_number(Stack_Token *tokens, Iter_String *stream) {
  push(tokens, ((Token){.begin = stream->idx, .size = 0, .type = TOKEN_NUMBER}));
  Token *t = at(tokens, -1);
  while (is_number1(sgetc(stream))) {
    t->size++;
  }
  sseekcur(stream, -1);
}

u0 push_string(Stack_Token *tokens, Iter_String *stream) {
  push(tokens, ((Token){.begin = stream->idx, .size = 1, .type = TOKEN_STRING}));
  Token *t = at(tokens, -1);
  char delimiter = sgetc(stream);
  char c;
  while ((c = sgetc(stream)) != EOF && c != delimiter && c != '\n') {
    t->size++;
    if (c == '\\') {
      t->size++;
      sgetc(stream);
    }
  }
  if (c == delimiter) {
    t->size++;
  } else {
    sseekcur(stream, -1);
  }
}

u0 tokenizer(Formatter *stream_string) {
  Iter_String stream_string_obj = sseekres(&stream_string->str);
  Stack_Token *tokens = &stream_string->tokens;
  Iter_String *stream = &stream_string_obj;
  u8 macro = 0;
  char c = EOF;

  while ((c = speekc(stream)) != EOF) {
    if (macro && c == '\n' && speekoffset(stream, -1) != '\\') {
      push_macro_end(tokens, stream);
      macro = 0;
    } else if (macro && c == '#') {
      push_macro_begin(tokens, stream);
    } else if (c == '#') {
      macro = 1;
    } else if (is_name_first(c)) {
      push_identifier(tokens, stream);
    } else if (is_special(c)) {
      push_special(tokens, stream);
    } else if (c == '/' && speekoffset(stream, 1) == '/') {
      push_comment_sline(tokens, stream);
    } else if (c == '/' && speekoffset(stream, 1) == '*') {
      push_comment_mline(tokens, stream);
    } else if (is_operator(c)) {
      push_operator(tokens, stream);
    } else if (is_number1(c)) {
      push_number(tokens, stream);
    } else if (is_string_delimiter(c)) {
      push_string(tokens, stream);
    } else {
      sgetc(stream);
    }
  }
  if (macro) {
    push(&stream_string->str, '\n');
    push(tokens, ((Token){.begin = stream_string->str.size - 1, .size = 1, .type = TOKEN_MACRO_END}));
  }
  // return tokens_obj;
}

Formatter parse_code_into_words(FILE *stream) {
  String s = new_String();
  fseek(stream, 0, SEEK_SET);
  char c;
  while ((c = fgetc(stream)) != EOF) {
    push(&s, c);
  }
  // return tokenizer(&s);
  return (Formatter){};
}
