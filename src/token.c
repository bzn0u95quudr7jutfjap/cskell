#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include <stack.h>
#include <string_class.h>

typedef struct {
  u8 macro;
  u32 indentation;
} tokenizer_env;

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
DEFINE_CSET(is_special, "{}()[].;,")
DEFINE_CSET(is_string_delimiter, "'\"")
DEFINE_CSET(is_operator, "+-*/%!=&|^><?:#")

bool is_name_first(char c) { return (c == '_') || ('a' <= c && c <= 'z') || ('A' <= c && c <= 'Z'); }
bool is_name(char c) { return is_name_first(c) || ('0' <= c && c <= '9'); }
bool is_number1_first(char c) { return ('0' <= c && c <= '9'); }
bool is_number1(char c) {
  return is_number1_first(c) || ('a' <= c && c <= 'f') || ('A' <= c && c <= 'F') || c == 'L' || c == 'x' || c == 'X';
}

u0 push_macro_begin(Stack_Token *tokens, Iter_String *stream, tokenizer_env *env) {
  push(tokens, ((Token){.begin = stream->idx, .size = 1, .type = TOKEN_MACRO_BEGIN}));
  sgetc(stream);
}

u0 push_macro_end(Stack_Token *tokens, Iter_String *stream, tokenizer_env *env) {
  push(tokens, ((Token){.begin = stream->idx, .size = 1, .type = TOKEN_MACRO_END}));
  sgetc(stream);
}

u0 push_identifier(Stack_Token *tokens, Iter_String *stream, tokenizer_env *env) {
  push(tokens, ((Token){.begin = stream->idx, .size = 0, .type = TOKEN_IDENTIFIER}));
  Token *t = at(tokens, -1);
  while (is_name(sgetc(stream))) {
    t->size++;
  }
  sseekcur(stream, -1);
}

u0 push_special(Stack_Token *tokens, Iter_String *stream, tokenizer_env *env) {
  push(tokens, ((Token){.begin = stream->idx, .size = 1, .type = TOKEN_SPECIAL}));
  Token *t = at(tokens, -1);
  switch (sgetc(stream)) {
  case '{':
    env->indentation += 1;
    t->indentation = env->indentation;
    t->newline_before = 1;
    break;
  case '}':
    t->indentation = env->indentation;
    env->indentation += -(env->indentation > 0);
    t->newline_before = 1;
    t->newline_after = 1;
    break;
  case ';':
    t->indentation = env->indentation;
    t->newline_before = env->indentation > 0;
    break;
  default:
    break;
  }
}

u0 push_comment_sline(Stack_Token *tokens, Iter_String *stream, tokenizer_env *env) {
  push(tokens, ((Token){.begin = stream->idx, .size = 0, .type = TOKEN_COMMENT_SL}));
  Token *t = at(tokens, -1);
  char c;
  while ((c = sgetc(stream)) != '\n' && c != EOF) {
    t->size++;
  }
}

u0 push_comment_mline(Stack_Token *tokens, Iter_String *stream, tokenizer_env *env) {
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

u0 push_operator(Stack_Token *tokens, Iter_String *stream, tokenizer_env *env) {
  push(tokens, ((Token){.begin = stream->idx, .size = 1, .type = TOKEN_OPERATOR}));
  sgetc(stream);
  if (is_operator(speekc(stream))) {
    at(tokens, -1)->size++;
    sgetc(stream);
  }
}

u0 push_number(Stack_Token *tokens, Iter_String *stream, tokenizer_env *env) {
  push(tokens, ((Token){.begin = stream->idx, .size = 0, .type = TOKEN_NUMBER}));
  Token *t = at(tokens, -1);
  while (is_number1(sgetc(stream))) {
    t->size++;
  }
  sseekcur(stream, -1);
}

u0 push_string(Stack_Token *tokens, Iter_String *stream, tokenizer_env *env) {
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

u0 push_nothing(Stack_Token *tokens, Iter_String *stream, tokenizer_env *env) { sgetc(stream); }

u8 is_macro_end(Iter_String *stream, tokenizer_env *env) {
  u8 ret = env->macro && speekc(stream) == '\n' && speekoffset(stream, -1) != '\\';
  env->macro = ret ? 0 : env->macro;
  return ret;
}

u8 is_macro_begin(Iter_String *stream, tokenizer_env *env) {
  u8 ret = !env->macro && speekc(stream) == '#';
  env->macro = ret ? 1 : env->macro;
  return ret;
}

u8 is_name_begin(Iter_String *stream, tokenizer_env *env) { return is_name_first(speekc(stream)); }

u8 is_special_begin(Iter_String *stream, tokenizer_env *env) { return is_special(speekc(stream)); }

u8 is_operator_begin(Iter_String *stream, tokenizer_env *env) { return is_operator(speekc(stream)); }

u8 is_number_begin(Iter_String *stream, tokenizer_env *env) { return is_number1_first(speekc(stream)); }

u8 is_string_begin(Iter_String *stream, tokenizer_env *env) { return is_string_delimiter(speekc(stream)); }

u8 is_comment_sline_begin(Iter_String *stream, tokenizer_env *env) {
  return speekc(stream) == '/' && speekoffset(stream, 1) == '/';
}
u8 is_comment_mline_begin(Iter_String *stream, tokenizer_env *env) {
  return speekc(stream) == '/' && speekoffset(stream, 1) == '*';
}

u8 is_otherwise(Iter_String *stream, tokenizer_env *env) { return 1; }

typedef u8 (*func_is_type)(Iter_String *, tokenizer_env *);
typedef u0 (*func_push_token)(Stack_Token *, Iter_String *, tokenizer_env *);
typedef struct {
  func_is_type is_type;
  func_push_token push_token;
} is_push_struct;

u0 tokenizer(Formatter *stream_string) {
  Iter_String stream_string_obj = sseekres(&stream_string->str);
  Stack_Token *tokens = &stream_string->tokens;
  Iter_String *stream = &stream_string_obj;
  tokenizer_env env = {};
  static is_push_struct is_push_array[] = {
      {.is_type = is_macro_end, .push_token = push_macro_end},
      {.is_type = is_macro_begin, .push_token = push_macro_begin},
      {.is_type = is_name_begin, .push_token = push_identifier},
      {.is_type = is_special_begin, .push_token = push_special},
      {.is_type = is_comment_sline_begin, .push_token = push_comment_sline},
      {.is_type = is_comment_mline_begin, .push_token = push_comment_mline},
      {.is_type = is_operator_begin, .push_token = push_operator},
      {.is_type = is_number_begin, .push_token = push_number},
      {.is_type = is_string_begin, .push_token = push_string},
      {.is_type = is_otherwise, .push_token = push_nothing},
  };
  static u32 is_push_array_len = sizeof(is_push_array) / sizeof(*is_push_array);

  char c = EOF;
  while ((c = speekc(stream)) != EOF) {
    for (u32 i = 0; i < is_push_array_len; i++) {
      if (is_push_array[i].is_type(stream, &env)) {
        is_push_array[i].push_token(tokens, stream, &env);
        break;
      }
    }
  }
  if (env.macro) {
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
