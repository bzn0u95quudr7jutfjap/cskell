#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <stack.h>
#include <string_class.h>

#define from_cstr(CSTR)                                                                                                \
  (String) { .data = CSTR, .size = sizeof(CSTR) / sizeof(CSTR[0]) - 1 }

u8 t_is_end(Iter_Formatter *stream) { return !(0 <= stream->idx && stream->idx < stream->fmt->tokens.size); }

Token *tpeekoffset(Iter_Formatter *stream, i32 o) { return at(&stream->fmt->tokens, stream->idx + o); }

Token *tpeekt(Iter_Formatter *stream) { return tpeekoffset(stream, 0); }

Token *tseekcur(Iter_Formatter *stream, i32 o) {
  stream->idx += o;
  return tpeekt(stream);
}

Token *tgett(Iter_Formatter *stream) {
  Token *t = tpeekt(stream);
  stream->idx++;
  return t;
}

String tgets_offset(Iter_Formatter *stream, i32 o) {
  Token *t = tpeekoffset(stream, o);
  return (String){.data = stream->fmt->str.data + t->begin, .size = t->size};
}

Iter_Formatter tseekres(Formatter *stream) { return (Iter_Formatter){.fmt = stream, .idx = 0}; }

u0 set_newline(Token *t, tokenizer_env *env) {
  env->prev->newline_after = env->prev->newline_after > 0 ? env->prev->newline_after : 1;
  env->prev->space_after = 0;
  t->indentation = env->indentation;
}

u8 in_definitionspace(tokenizer_env *env) { return env->globalspace == 0 && 0 < env->definitionspace; }

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
                       from_cstr(">"),  from_cstr("+"),  from_cstr("-"),  from_cstr("/"),  from_cstr("="),
                       from_cstr("##"), from_cstr("?"),  from_cstr(":"),  from_cstr("%")};
  static u32 len = sizeof(a) / sizeof(a[0]);
  return in(a, len, b);
}

u8 is_operatore_unario(String *b) {
  static String a[] = {
      from_cstr("!"),
      from_cstr("#"),
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

u0 format_operator(Iter_Formatter *fmt, tokenizer_env *env) {
  static String arrow = from_cstr("->");
  static String not = from_cstr("!");
  static String inc = from_cstr("++");
  static String dec = from_cstr("--");
  static String clp = from_cstr(")");
  Token *t = tpeekt(fmt);
  String tmp = tgets_offset(fmt, 0);
  String tmp2 = tgets_offset(fmt, -1);
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

u0 format_comment(Iter_Formatter *fmt, tokenizer_env *env) {
  Token *t = tpeekt(fmt);
  set_newline(t, env);
  t->newline_after = 1;
}

u0 print_debug(Iter_Formatter *fmt, i32 o) {
  Token *tt = tpeekoffset(fmt, o);
  String s = tt == NULL ? from_cstr("(NULL)") : tgets_offset(fmt, o);
  fprintf(stderr, "A[%4d][def: %3d]: %.*s\n", fmt->idx + o, tt == NULL ? -1 : tt->definition, (i32)s.size, s.data);
}

u0 print_str_debug(char *a) { fprintf(stderr, "%s\n", a); }

u0 format_special(Iter_Formatter *fmt, tokenizer_env *env) {
  Token *t = tpeekt(fmt);
  Token *t1, *t2, *t3;
  String tmp = tgets_offset(fmt, 0);
  String tmp1, tmp2, tmp3, tmp4;
  String equalop = from_cstr("=");
  String evbegin = from_cstr("(");
  String evend = from_cstr(")");
  char c = *at(&tmp, 0);
  switch (c) {
  case '{':
    t3 = tpeekoffset(fmt, -3);
    tmp3 = t3 == NULL ? from_cstr("") : tgets_offset(fmt, -3);
    t2 = tpeekoffset(fmt, -2);
    tmp2 = t2 == NULL ? from_cstr("") : tgets_offset(fmt, -2);
    t1 = tpeekoffset(fmt, -1);
    tmp1 = t1 == NULL ? from_cstr("") : tgets_offset(fmt, -1);
    u8 casting = (es(&tmp3, &evbegin) && t2 != NULL && t2->type == TOKEN_IDENTIFIER && es(&tmp1, &evend));
    u8 in_definition = 0;
    in_definition = in_definition || in_definitionspace(env);
    in_definition = in_definition || (t1 != NULL && t1->type == TOKEN_OPERATOR && es(&tmp1, &equalop));
    in_definition = in_definition || casting;
    if (in_definition) {
      env->definitionspace += 1;
      env->globalspace = 0;
    }
    if (casting) {
      Token *t4 = tpeekoffset(fmt, -4);
      if (t4 != NULL) {
        t4->newline_after = 1;
        t3->indentation = env->indentation + 1;
      }
    }
    if (in_definition) {
      t->definition = 1;
    }
    env->indentation += 1;
    set_newline(t, env);
    t->space_after = 1;
    break;
  case '}':
    set_newline(t, env);
    env->indentation += -(env->indentation > 0);
    t->newline_after = env->indentation == 0 ? 2 : 1;
    if (in_definitionspace(env)) {
      env->definitionspace += -(env->definitionspace > 0);
      t->definition = 1;
    }
    break;
  case '.':
    if (in_definitionspace(env)) {
      env->prev->space_after = 1;
      t->space_after = 0;
      break;
    }
    env->prev->space_after = 0;
    t->space_after = 0;
    break;
  case ';':
    if (env->indentation == 0) {
      if (env->prev != NULL) {
        env->prev->space_after = 0;
        env->prev->newline_after = 0;
        set_newline(t, env);
        t->indentation = 1;
      }
      t->newline_after = 2;
    } else {
      set_newline(t, env);
      env->prev->space_after = 0;
      t->space_after = 1;
    }
    break;
  case ',':
    if (in_definitionspace(env)) {
      set_newline(t, env);
      t->space_after = 1;
      break;
    }
    env->prev->space_after = 0;
    t->space_after = 1;
    break;
  case '[':
  case '(':
    if (in_definitionspace(env)) {
      env->globalspace = 1;
    }
    if (env->prev != NULL && env->prev->type != TOKEN_OPERATOR && !is_operatore_binario(&tmp)) {
      env->prev->space_after = 0;
    }
    String coma = from_cstr(",");
    String tmp = tgets_offset(fmt, -1);
    if (es(&tmp, &coma)) {
      env->prev->space_after = 1;
    }
    t->space_after = 0;
    break;
  case ']':
  case ')':
    if (env->definitionspace > 0) {
      env->globalspace = 0;
    }
    if (env->prev != NULL) {
      if (env->prev->definition == 1) {
        env->prev->newline_after = 1;
        t->indentation = env->prev->indentation;
        t->newline_after = 0;
      } else {
        env->prev->space_after = 0;
        t->space_after = 0;
      }
    }
    break;
  default:
    break;
  }
  if (in_definitionspace(env)) {
    t->definition = 1;
  }
}

u0 format_macro_begin(Iter_Formatter *fmt, tokenizer_env *env) {
  static String inc = from_cstr("include");
  if (env->prev != NULL) {
    env->prev->newline_after = env->prev->type == TOKEN_MACRO_END ? 0 : 2;
  }
  env->indentation += 1;
  Token *t = tseekcur(fmt, 1);
  t->space_after = 1;
  String macrocmd = tgets_offset(fmt, 0);
  if (es(&macrocmd, &inc)) {
    t = tseekcur(fmt, 1);
    while (t->type != TOKEN_MACRO_END) {
      t->space_after = 0;
      t->newline_after = 0;
      t = tseekcur(fmt, 1);
    }
    tseekcur(fmt, -1);
  }
}

u0 format_identifier(Iter_Formatter *fmt, tokenizer_env *env) {
  Token *t = tpeekt(fmt);
  Token *t1 = tpeekoffset(fmt, -1);
  String blockend = from_cstr("}");
  String tmp1 = t1 == NULL ? from_cstr("") : tgets_offset(fmt, -1);
  if (es(&tmp1, &blockend)) {
    env->prev->newline_after = 0;
    env->prev->space_after = 1;
  }
  t->space_after = 1;
}

u0 formatter(Formatter *fmttr) {
  tokenizer_env env_obj = {.prev = NULL};
  tokenizer_env *env = &env_obj;
  Iter_Formatter iterfmt = tseekres(fmttr);
  Iter_Formatter *fmt = &iterfmt;
  while (!t_is_end(&iterfmt)) {
    Token *t = tpeekt(fmt);
    if (env->prev != NULL && env->prev->newline_after > 0) {
      t->indentation = env->indentation;
    }
    switch (t->type) {
    case TOKEN_IDENTIFIER:
      format_identifier(fmt, env);
      break;
    case TOKEN_OPERATOR:
      format_operator(fmt, env);
      break;
    case TOKEN_COMMENT_SL:
    case TOKEN_COMMENT_ML:
      format_comment(fmt, env);
      break;
    case TOKEN_MACRO_END:
      env->indentation += -(env->indentation > 0);
      t->newline_after = 1;
      break;
    case TOKEN_MACRO_BEGIN:
      format_macro_begin(fmt, env);
      break;
    case TOKEN_SPECIAL:
      format_special(fmt, env);
      break;
    default:
      break;
    }
    env->prev = tgett(fmt);
  }
}

u0 ifp(u32 i, FILE *out, char *str) {
  while (i--) {
    fprintf(out, "%s", str);
  }
}

u0 print_formatted_code(FILE *out, Formatter *fmttr) {
  tokenizer_env env;
  Iter_Formatter fmt_obj = tseekres(fmttr);
  Iter_Formatter *fmt = &fmt_obj;
  while (!t_is_end(fmt)) {
    Token *t = tpeekt(fmt);
    String str = tgets_offset(fmt, 0);
    int len = str.size;
    if (len != str.size) {
      perror("len != size");
      exit(1);
    }
    env.macro = t->type == TOKEN_MACRO_BEGIN ? 1 : env.macro;
    env.macro = t->type == TOKEN_MACRO_END ? 0 : env.macro;
    ifp(t->indentation, out, "  ");
    fprintf(out, "%.*s", len, str.data);
    if (t->newline_after == 0) {
      ifp(t->space_after, out, " ");
    } else if (!env.macro) {
      ifp(t->newline_after, out, "\n");
    } else {
      ifp(1, out, " ");
      ifp(1, out, "\\");
      ifp(1, out, "\n");
    }
    (u0) tgett(fmt);
  }
}
