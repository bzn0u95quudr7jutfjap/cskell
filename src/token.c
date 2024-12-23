#include <stdlib.h>
#include <string.h>

#include "stack/stack.h"
#include "string_class.h"
#include <stack.h>

// ================================================================================
// TOKENIZER
// ================================================================================

typedef struct {
  TokenType type;
  u32       size;
  char     *data;
} TokenPattern;

#define pattern(t, str)                                                                                                \
  { .type = t, .size = sizeof(str) - 1, .data = str }

Token *gett(CodeTokens *code) {
  u32    size   = code->tokens.size;
  Token *tokens = code->tokens.data;
  return size < count(code->tokens.data) ? &tokens[size] : NULL;
}

u8 parse_word(CodeTokens *codetokens) {
  Token *t    = gett(codetokens);
  u32    i    = codetokens->iter;
  u32    size = codetokens->codice.size;
  char  *code = codetokens->codice.data;
  char   c    = code[i];
  u8     b;
  if (NULL == t) {
    return 0;
  }
  if (!(c == '_' || ('a' <= c && c <= 'z') || ('A' <= c && c <= 'Z'))) {
    return 0;
  }
  t->type  = TOKEN_WORD;
  t->begin = i;
  t->size  = 0;
  while (i < size) {
    c = code[i];
    b = c == '_' || ('a' <= c && c <= 'z') || ('A' <= c && c <= 'Z');
    b = b || (t->begin < i && '0' <= c && c <= '9');
    if (!b) {
      break;
    }
    t->size++;
    i++;
  }
  return 1;
}

u8 parse_numeric(CodeTokens *codetokens) {
  Token *t    = gett(codetokens);
  u32    i    = codetokens->iter;
  u32    size = codetokens->codice.size;
  char  *code = codetokens->codice.data;
  char   c    = code[i];
  u8     b;
  if (NULL == t) {
    return 0;
  }
  if (!('0' <= c && c <= '9')) {
    return 0;
  }
  t->type  = TOKEN_NUMBER;
  t->begin = i;
  t->size  = 0;
  while (i < size) {
    c = code[i];
    b = ('0' <= c && c <= '9');
    b = b || (t->begin < i && (('a' <= c && c <= 'f') || ('A' <= c && c <= 'F')));
    b = b || (t->begin < i && (c == 'x' || c == 'X'));
    b = b || (t->begin < i && (c == 'u' || c == 'U'));
    b = b || (t->begin < i && (c == 'l' || c == 'L'));
    if (!b) {
      break;
    }
    t->size++;
    i++;
  }
  return 1;
}

u8 parse_comment_sline(CodeTokens *codetokens) {
  Token *t    = gett(codetokens);
  u32    i    = codetokens->iter;
  u32    size = codetokens->codice.size;
  char  *code = codetokens->codice.data;
  if (NULL == t) {
    return 0;
  }
  if (!(i + 1 < size && '/' == code[i] && '/' == code[i + 1])) {
    return 0;
  }
  t->type  = TOKEN_COMMENT_SL;
  t->begin = i;
  t->size  = 0;
  t->size++;
  i++;
  while (i < size) {
    if ('\n' == code[i]) {
      break;
    }
    t->size++;
    i++;
  }
  return 1;
}

u8 parse_comment_mline(CodeTokens *codetokens) {
  Token *t    = gett(codetokens);
  u32    i    = codetokens->iter;
  u32    size = codetokens->codice.size;
  char  *code = codetokens->codice.data;
  if (NULL == t) {
    return 0;
  }
  if (!(i + 1 < size && '/' == code[i] && '*' == code[i + 1])) {
    return 0;
  }
  t->type  = TOKEN_COMMENT_ML;
  t->begin = i;
  t->size  = 0;
  while (i < size) {
    t->size++;
    i++;
    if ('*' == code[i - 1] && '/' == code[i]) {
      t->size++;
      i++;
      break;
    }
  }
  return 1;
}

u8 parse_include_string(CodeTokens *codetokens) {
  Token *t    = gett(codetokens);
  u32    i    = codetokens->iter;
  u32    size = codetokens->codice.size;
  char  *code = codetokens->codice.data;
  if (NULL == t) {
    return 0;
  }
  if ('<' != code[i]) {
    return 0;
  }
  t->type  = TOKEN_STRING;
  t->begin = i;
  t->size  = 0;
  while (i < size) {
    t->size++;
    i++;
    if ('>' == code[i]) {
      t->size++;
      i++;
      break;
    }
  }
  return 1;
}

u8 parse_string(CodeTokens *codetokens) {
  Token *t    = gett(codetokens);
  u32    i    = codetokens->iter;
  u32    size = codetokens->codice.size;
  char  *code = codetokens->codice.data;
  char   c;
  char   d;
  if (NULL == t) {
    return 0;
  }
  d = code[i];
  if (!('"' == d || '\'' == d)) {
    return 0;
  }
  t->type  = TOKEN_STRING;
  t->begin = i;
  t->size  = 0;
  t->size++;
  i++;
  while (i < size) {
    c = code[i];
    if (c == d) {
      t->size++;
      break;
    } else if ('\n' == c) {
      break;
    } else if ('\\' == c) {
      t->size++;
      i++;
    }
    t->size++;
    i++;
  }
  return 1;
}

u8 parse_macro_begin(CodeTokens *codetokens) {
  Token *t    = gett(codetokens);
  u32    i    = codetokens->iter;
  u32    size = codetokens->codice.size;
  char  *code = codetokens->codice.data;
  if (NULL == t) {
    return 0;
  }
  static TokenPattern ops[] = {
      pattern(TOKEN_MACRO_BEGIN, "#include"), pattern(TOKEN_MACRO_BEGIN, "#define"),
      pattern(TOKEN_MACRO_BEGIN, "#if"),      pattern(TOKEN_MACRO_BEGIN, "#ifdef"),
      pattern(TOKEN_MACRO_BEGIN, "#ifndef"),  pattern(TOKEN_MACRO_BEGIN, "#else"),
      pattern(TOKEN_MACRO_BEGIN, "#elif"),    pattern(TOKEN_MACRO_BEGIN, "#endif"),
  };
  static u8     ops_n = count(ops);
  TokenPattern *ptn   = NULL;
  u32           len;
  for (u8 j = 0; j < ops_n; j++) {
    ptn = &ops[j];
    len = size - i;
    len = len < ptn->size ? len : ptn->size;
    if (0 == strncmp(code + i, ptn->data, len)) {
      t->type  = ptn->type;
      t->begin = i;
      t->size  = ptn->size;
      return 1;
    }
  }
  return 0;
}

u8 parse_macro_enabled(CodeTokens *codetokens) {
  Token *t    = gett(codetokens);
  u32    i    = codetokens->iter;
  u32    size = codetokens->codice.size;
  char  *code = codetokens->codice.data;
  if (NULL == t) {
    return 0;
  }
  if (0 < i && '\\' != code[i - 1] && '\n' == code[i]) {
    t->type  = TOKEN_MACRO_END;
    t->begin = i;
    t->size  = 1;
    return 1;
  }
  static TokenPattern ops[] = {
      pattern(TOKEN_OPERATOR_BINARY, "##"),
      pattern(TOKEN_OPERATOR_UNARY, "#"),
  };
  static u32    ops_n = count(ops);
  TokenPattern *ptn   = NULL;
  u32           len;
  for (u32 j = 0; j < ops_n; j++) {
    ptn = &ops[j];
    len = size - i;
    len = len < ptn->size ? len : ptn->size;
    if (0 == strncmp(code + i, ptn->data, len)) {
      t->type  = ptn->type;
      t->begin = i;
      t->size  = ptn->size;
      return 1;
    }
  }
  return 0;
}

u8 parse_match(CodeTokens *codetokens) {
  Token *t    = gett(codetokens);
  u32    i    = codetokens->iter;
  u32    size = codetokens->codice.size;
  char  *code = codetokens->codice.data;
  if (NULL == t) {
    return 0;
  }
  static TokenPattern matches[] = {
      pattern(TOKEN_BLOCK_BEGIN, "{"),
      pattern(TOKEN_BLOCK_END, "}"),
      pattern(TOKEN_EXPR_BEGIN, "("),
      pattern(TOKEN_EXPR_END, ")"),
      pattern(TOKEN_ARRAY_I_BEGIN, "["),
      pattern(TOKEN_ARRAY_I_END, "]"),
      pattern(TOKEN_COMA, ","),
      pattern(TOKEN_END_OF_LINE, ";"),
      pattern(TOKEN_OPERATOR_PREPOSTFIX, "++"),
      pattern(TOKEN_OPERATOR_PREPOSTFIX, "--"),
      pattern(TOKEN_OPERATOR_ATTRIBUTE, "."),
      pattern(TOKEN_OPERATOR_ATTRIBUTE, "->"),
      pattern(TOKEN_OPERATOR_BINARY, "&&"),
      pattern(TOKEN_OPERATOR_BINARY, "||"),
      pattern(TOKEN_OPERATOR_BINARY, ">>"),
      pattern(TOKEN_OPERATOR_BINARY, "<<"),
      pattern(TOKEN_OPERATOR_BINARY, "=="),
      pattern(TOKEN_OPERATOR_BINARY, "%="),
      pattern(TOKEN_OPERATOR_BINARY, "+="),
      pattern(TOKEN_OPERATOR_BINARY, "-="),
      pattern(TOKEN_OPERATOR_BINARY, "/="),
      pattern(TOKEN_OPERATOR_BINARY, "|="),
      pattern(TOKEN_OPERATOR_BINARY, ">="),
      pattern(TOKEN_OPERATOR_BINARY, "<="),
      pattern(TOKEN_OPERATOR_BINARY, "^="),
      pattern(TOKEN_OPERATOR_BINARY, "!="),
      pattern(TOKEN_OPERATOR_BINARY, "*="),
      pattern(TOKEN_OPERATOR_BINARY, "&="),
      pattern(TOKEN_OPERATOR_BINARY, "?"),
      pattern(TOKEN_OPERATOR_BINARY, ":"),
      pattern(TOKEN_OPERATOR_BINARY, "="),
      pattern(TOKEN_OPERATOR_BINARY, "%"),
      pattern(TOKEN_OPERATOR_BINARY, "+"),
      pattern(TOKEN_OPERATOR_BINARY, "-"),
      pattern(TOKEN_OPERATOR_BINARY, "/"),
      pattern(TOKEN_OPERATOR_BINARY, "|"),
      pattern(TOKEN_OPERATOR_BINARY, ">"),
      pattern(TOKEN_OPERATOR_BINARY, "<"),
      pattern(TOKEN_OPERATOR_BINARY, "^"),
      pattern(TOKEN_OPERATOR_UNARY, "!"),
      pattern(TOKEN_OPERATOR_AMBIGUOUS, "*"),
      pattern(TOKEN_OPERATOR_AMBIGUOUS, "&"),
  };
  static u32    matches_n = count(matches);
  TokenPattern *ptn;
  u32           len;
  for (u32 j = 0; j < matches_n; j++) {
    ptn = &matches[j];
    len = size - i;
    len = len < ptn->size ? len : ptn->size;
    if (0 == strncmp(code + i, ptn->data, len)) {
      t->type  = ptn->type;
      t->begin = i;
      t->size  = ptn->size;
      return 1;
    }
  }
  return 0;
}

u8 parse_token(CodeTokens *codetokens, TokenEnv *env) {
  u8 b = 0;
  b    = b || (1 == env->macro && 1 == env->include && parse_include_string(codetokens));
  b    = b || (1 == env->macro && parse_macro_enabled(codetokens));
  b    = b || (0 == env->macro && parse_macro_begin(codetokens));
  b    = b || (parse_word(codetokens));
  b    = b || (parse_numeric(codetokens));
  b    = b || (parse_string(codetokens));
  b    = b || (parse_comment_sline(codetokens));
  b    = b || (parse_comment_mline(codetokens));
  b    = b || (parse_match(codetokens));
  if (b) {
    static TokenPattern p = pattern(TOKEN_MACRO_BEGIN, "#include");
    Token              *t = gett(codetokens);
    if (t->type == p.type && t->size == p.size && 0 == strncmp(p.data, codetokens->codice.data + t->begin, p.size)) {
      env->macro   = 1;
      env->include = 1;
    } else if (TOKEN_MACRO_BEGIN == t->type) {
      env->macro = 1;
    } else if (TOKEN_MACRO_END == t->type) {
      env->macro   = 0;
      env->include = 0;
      t->size      = 0;
    }
    codetokens->tokens.size++;
    codetokens->iter += 0 < t->size ? t->size : 1;
  } else {
    codetokens->iter++;
  }
  return b;
}

u0 tokenizer(CodeTokens *codetokens) {
  TokenEnv env;
  env.macro        = 0;
  env.include      = 0;
  codetokens->iter = 0;
  while (codetokens->iter < codetokens->codice.size) {
    parse_token(codetokens, &env);
  }
}
