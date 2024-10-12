#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <stack.h>
#include <string_class.h>


u0 format_comment(Formatter *fmt, u32 i, tokenizer_env *env) {
  Token *t = at(&fmt->tokens, i);
  String tmp = {.data = fmt->str.data + t->begin, .size = t->size};
  if (env->prev->newline_after == 0) {
    t->newline_before = 1;
  }
  t->indentation = env->indentation;
  t->newline_after = 1;
}
u0 format_special(Formatter *fmt, u32 i, tokenizer_env *env) {
  Token *t = at(&fmt->tokens, i);
  String tmp = {.data = fmt->str.data + t->begin, .size = t->size};
  char c = *at(&tmp, 0);
  switch (c) {
  case '{':
    env->indentation += 1;
    t->indentation = env->indentation;
    t->newline_before = 1;
    break;
  case '}':
    t->indentation = env->indentation;
    env->indentation += -(env->indentation > 0);
    t->newline_before = 1;
    t->newline_after = t->indentation == 0 ? 2 : 1;
    break;
  case ';':
    t->indentation = env->indentation;
    t->newline_before = env->indentation > 0;
    break;
  case ',':
    t->nospace_before = 1;
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
      t->nospace_before = 1;
    }
    switch (t->type) {
    case TOKEN_COMMENT_SL:
    case TOKEN_COMMENT_ML:
      format_comment(fmt, i, env);
      break;
    case TOKEN_SPECIAL:
      format_special(fmt, i, env);
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
    ifp(t->indentation == 0 && !t->nospace_before, out, " ");
    String str = {.data = fmt->str.data + t->begin, .size = t->size};
    int len = str.size;
    if (len != str.size) {
      perror("len != size");
      exit(1);
    }
    fprintf(out, "%.*s", len, str.data);
    ifp(t->newline_after, out, "\n");
  }
}
