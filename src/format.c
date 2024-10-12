#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <stack.h>
#include <string_class.h>

u0 set_newline(Token *t, tokenizer_env *env) {
  t->newline_before = env->prev->newline_after == 0;
  t->indentation = env->indentation;
}

u0 format_comment(Formatter *fmt, u32 i, tokenizer_env *env) {
  Token *t = at(&fmt->tokens, i);
  set_newline(t, env);
  t->newline_after = 1;
}

u0 format_special(Formatter *fmt, u32 i, tokenizer_env *env) {
  Token *t = at(&fmt->tokens, i);
  String tmp = {.data = fmt->str.data + t->begin, .size = t->size};
  char c = *at(&tmp, 0);
  switch (c) {
  case '{':
    env->indentation += 1;
    set_newline(t, env);
    t->space_after = 1;
    break;
  case '}':
    set_newline(t, env);
    env->indentation += -(env->indentation > 0);
    t->newline_after = env->indentation == 0 ? 2 : 1;
    break;
  case '.':
    env->prev->space_after = 0;
    t->space_before = 0;
    t->space_after = 0;
    break;
  case ';':
    set_newline(t, env);
    t->space_after = 1;
    break;
  case ',':
    env->prev->space_after = 0;
    t->space_after = 1;
    break;
  case '(':
    env->prev->space_before = 0;
    env->prev->space_after = 0;
    break;
  case ')':
    env->prev->space_before = 0;
    env->prev->space_after = 0;
    break;
  default:
    break;
  }
}

u0 formatter(Formatter *fmt) {
  tokenizer_env env_obj = {.prev = NULL};
  tokenizer_env *env = &env_obj;
  for (u32 i = 0; i < fmt->tokens.size; i++) {
    Token *t = at(&fmt->tokens, i);
    if (env->prev != NULL && env->prev->newline_after > 0) {
      t->indentation = env->indentation;
    }
    switch (t->type) {
    case TOKEN_IDENTIFIER:
      t->space_after = 1;
      break;
    case TOKEN_COMMENT_SL:
    case TOKEN_COMMENT_ML:
      format_comment(fmt, i, env);
      break;
    case TOKEN_SPECIAL:
      format_special(fmt, i, env);
      break;
    default:
      break;
    }
    env->prev = t;
  }
}

u0 ifp(u32 i, FILE *out, char *str) {
  while (i--) {
    fprintf(out, "%s", str);
  }
}

u0 print_formatted_code(FILE *out, Formatter *fmt) {
  for (u32 i = 0; i < fmt->tokens.size; i++) {
    Token *t = at(&fmt->tokens, i);
    ifp(t->newline_before, out, "\n");
    ifp(t->indentation, out, "  ");
    ifp(t->space_before, out, " ");
    String str = {.data = fmt->str.data + t->begin, .size = t->size};
    int len = str.size;
    if (len != str.size) {
      perror("len != size");
      exit(1);
    }
    fprintf(out, "%.*s", len, str.data);
    ifp(t->space_after, out, " ");
    ifp(t->newline_after, out, "\n");
  }
}
