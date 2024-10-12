#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <stack.h>
#include <string_class.h>

#define from_cstr(CSTR)                                                                                                \
  (String) { .data = CSTR, .size = sizeof(CSTR) / sizeof(CSTR[0]) - 1 }

u0 set_newline(Token *t, tokenizer_env *env) {
  t->newline_before = env->prev->newline_after == 0;
  t->indentation = env->indentation;
}

u8 es(String *a, String *b) { return a->size == b->size && strncmp(a->data, b->data, a->size) == 0; }

u8 in(String a[], u32 len, String *b) {
  for (u32 i = 0; i < len; i++) {
    if (es(&a[i], b)) {
      return 1;
    }
  }
  return 0;
}

u8 is_operatore_binario(String *b) {
  static String a[] = {from_cstr("=="), from_cstr("!="), from_cstr(">="), from_cstr("<="), from_cstr("<<"),
                       from_cstr(">>"), from_cstr("||"), from_cstr("&&"), from_cstr("|"),  from_cstr("<"),
                       from_cstr(">"),  from_cstr("+"),  from_cstr("-"),  from_cstr("/"),  from_cstr("=")};
  static u32 len = sizeof(a) / sizeof(a[0]);
  return in(a, len, b);
}

u8 is_operatore_unario(String *b) {
  static String a[] = {
      from_cstr("!"),
      from_cstr("++"),
      from_cstr("--"),
  };
  static u32 len = sizeof(a) / sizeof(a[0]);
  return in(a, len, b);
}

u8 is_operatore_ambiguo(String *b) {
  static String a[] = {
      from_cstr("*"),
      from_cstr("&"),
  };
  static u32 len = sizeof(a) / sizeof(a[0]);
  return in(a, len, b);
}

u0 format_operator(Formatter *fmt, u32 i, tokenizer_env *env) {
  static String arrow = from_cstr("->");
  static String not = from_cstr("!");
  static String inc = from_cstr("++");
  static String dec = from_cstr("--");
  static String clp = from_cstr(")");
  Token *t = at(&fmt->tokens, i);
  String tmp = {.data = fmt->str.data + t->begin, .size = t->size};
  String tmp2 = {.data = fmt->str.data + env->prev->begin, .size = env->prev->size};
  u8 tonda_chiusa = env->prev->type == TOKEN_SPECIAL && es(&tmp2, &clp);
  if (es(&tmp, &arrow)) {
    env->prev->space_after = 0;
    t->space_after = 0;
  } else if (es(&tmp, &not )) {
    t->space_after = 0;
  } else if (es(&tmp, &inc) || es(&tmp, &dec)) {
    if (tonda_chiusa || env->prev->type == TOKEN_IDENTIFIER) {
      // postfix
      env->prev->space_after = 0;
      t->space_after = 1;
    } else {
      // prefix
      env->prev->space_after = 1;
      t->space_after = 0;
    }
  } else if (is_operatore_binario(&tmp)) {
    env->prev->space_after = 1;
    t->space_after = 1;
  } else if (is_operatore_ambiguo(&tmp)) {
    if (tonda_chiusa || env->prev->type == TOKEN_IDENTIFIER) {
      // binario
      env->prev->space_after = 1;
      t->space_after = 1;
    } else {
      // prefix
      t->space_after = 0;
    }
  } else {
    printf("OPERATORE SCONOSCIUTO :: %.*s\n", (int)tmp.size, tmp.data);
    // exit(1);
  }
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
    t->space_after = 0;
    break;
  case ';':
    set_newline(t, env);
    env->prev->space_after = 0;
    t->space_after = 1;
    break;
  case ',':
    env->prev->space_after = 0;
    t->space_after = 1;
    break;
  case '(':
    if (env->prev->type != TOKEN_OPERATOR && !is_operatore_binario(&tmp)) {
      env->prev->space_after = 0;
    }
    t->space_after = 0;
    break;
  case ')':
    env->prev->space_after = 0;
    t->space_after = 0;
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
    case TOKEN_OPERATOR:
      format_operator(fmt, i, env);
      break;
    case TOKEN_COMMENT_SL:
    case TOKEN_COMMENT_ML:
      format_comment(fmt, i, env);
      break;
    case TOKEN_MACRO_END:
      t->newline_after = 2;
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
