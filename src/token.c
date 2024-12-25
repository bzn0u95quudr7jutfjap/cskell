#include <stdlib.h>
#include <string.h>

// ================================================================================
// TOKENIZER
// ================================================================================

typedef struct {
  u8    isjust;
  Token data;
} MaybeToken;

#define Nothing                                                                                                        \
  { .isjust = 0 }
#define Just(obj)                                                                                                      \
  { .isjust = 1, .data = obj }

typedef struct {
  TokenType type;
  u32       size;
  char     *data;
} TokenPattern;

#define pattern(t, str)                                                                                                \
  { .type = t, .size = sizeof(str) - 1, .data = str }

static MaybeToken parse_word(u32 i, u32 size, char *code) {
  Token t = {};
  char  c = code[i];
  u8    b;
  if (!(c == '_' || ('a' <= c && c <= 'z') || ('A' <= c && c <= 'Z'))) {
    return (MaybeToken)Nothing;
  }
  t.type  = TOKEN_WORD;
  t.begin = i;
  t.size  = 0;
  while (i < size) {
    c = code[i];
    b = c == '_' || ('a' <= c && c <= 'z') || ('A' <= c && c <= 'Z');
    b = b || (t.begin < i && '0' <= c && c <= '9');
    if (!b) {
      break;
    }
    t.size++;
    i++;
  }
  return (MaybeToken)Just(t);
}

static MaybeToken parse_numeric(u32 i, u32 size, char *code) {
  Token t = {};
  char  c = code[i];
  u8    b;
  if (!('0' <= c && c <= '9')) {
    return (MaybeToken)Nothing;
  }
  t.type  = TOKEN_NUMBER;
  t.begin = i;
  t.size  = 0;
  while (i < size) {
    c = code[i];
    b = ('0' <= c && c <= '9');
    b = b || (t.begin < i && (('a' <= c && c <= 'f') || ('A' <= c && c <= 'F')));
    b = b || (t.begin < i && (c == 'x' || c == 'X'));
    b = b || (t.begin < i && (c == 'u' || c == 'U'));
    b = b || (t.begin < i && (c == 'l' || c == 'L'));
    if (!b) {
      break;
    }
    t.size++;
    i++;
  }
  return (MaybeToken)Just(t);
}

static MaybeToken parse_comment_sline(u32 i, u32 size, char *code) {
  Token t = {};
  if (!(i + 1 < size && '/' == code[i] && '/' == code[i + 1])) {
    return (MaybeToken)Nothing;
  }
  t.type  = TOKEN_COMMENT_SL;
  t.begin = i;
  t.size  = 0;
  t.size++;
  i++;
  while (i < size) {
    if ('\n' == code[i]) {
      break;
    }
    t.size++;
    i++;
  }
  return (MaybeToken)Just(t);
}

static MaybeToken parse_comment_mline(u32 i, u32 size, char *code) {
  Token t = {};
  if (!(i + 1 < size && '/' == code[i] && '*' == code[i + 1])) {
    return (MaybeToken)Nothing;
  }
  t.type  = TOKEN_COMMENT_ML;
  t.begin = i;
  t.size  = 0;
  while (i < size) {
    t.size++;
    i++;
    if ('*' == code[i - 1] && '/' == code[i]) {
      t.size++;
      i++;
      break;
    }
  }
  return (MaybeToken)Just(t);
}

static MaybeToken parse_include_string(u32 i, u32 size, char *code) {
  Token t = {};
  if ('<' != code[i]) {
    return (MaybeToken)Nothing;
  }
  t.type  = TOKEN_STRING;
  t.begin = i;
  t.size  = 0;
  while (i < size) {
    t.size++;
    i++;
    if ('>' == code[i]) {
      t.size++;
      i++;
      break;
    }
  }
  return (MaybeToken)Just(t);
}

static MaybeToken parse_string(u32 i, u32 size, char *code) {
  Token t = {};
  char  c;
  char  d = code[i];
  if (!('"' == d || '\'' == d)) {
    return (MaybeToken)Nothing;
  }
  t.type  = TOKEN_STRING;
  t.begin = i;
  t.size  = 0;
  t.size++;
  i++;
  while (i < size) {
    c = code[i];
    if (c == d) {
      t.size++;
      break;
    } else if ('\n' == c) {
      break;
    } else if ('\\' == c) {
      t.size++;
      i++;
    }
    t.size++;
    i++;
  }
  return (MaybeToken)Just(t);
}

static MaybeToken parse_macro_begin(u32 i, u32 size, char *code) {
  Token               t     = {};
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
      t.type  = ptn->type;
      t.begin = i;
      t.size  = ptn->size;
      return (MaybeToken)Just(t);
    }
  }
  return (MaybeToken)Nothing;
}

static MaybeToken parse_macro_enabled(u32 i, u32 size, char *code) {
  Token t = {};
  if (0 < i && '\\' != code[i - 1] && '\n' == code[i]) {
    t.type  = TOKEN_MACRO_END;
    t.begin = i;
    t.size  = 1;
    return (MaybeToken)Just(t);
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
      t.type  = ptn->type;
      t.begin = i;
      t.size  = ptn->size;
      return (MaybeToken)Just(t);
    }
  }
  return (MaybeToken)Nothing;
}

static MaybeToken parse_match(u32 i, u32 size, char *code) {
  Token               t         = {};
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
      t.type  = ptn->type;
      t.begin = i;
      t.size  = ptn->size;
      return (MaybeToken)Just(t);
    }
  }
  return (MaybeToken)Nothing;
}

u8 parse_token(CodeTokens *codetokens, TokenEnv *env) {
  static TokenPattern includeptn = pattern(TOKEN_MACRO_BEGIN, "#include");
  u8                  isinclude  = 1;
  MaybeToken          t          = {};
  u32                 i          = codetokens->iter;
  u32                 size       = codetokens->codice.size;
  char               *code       = codetokens->codice.data;
  u8                  b          = 0;
  // clang-format off
  b = b || (1 == env->macro && 1 == env->include && (t = parse_include_string (i, size, code)).isjust);
  b = b || (1 == env->macro &&                      (t = parse_macro_enabled  (i, size, code)).isjust);
  b = b || (0 == env->macro &&                      (t = parse_macro_begin    (i, size, code)).isjust);
  b = b ||                                          (t = parse_word           (i, size, code)).isjust;
  b = b ||                                          (t = parse_numeric        (i, size, code)).isjust;
  b = b ||                                          (t = parse_string         (i, size, code)).isjust;
  b = b ||                                          (t = parse_comment_sline  (i, size, code)).isjust;
  b = b ||                                          (t = parse_comment_mline  (i, size, code)).isjust;
  b = b ||                                          (t = parse_match          (i, size, code)).isjust;
  // clang-format on
  if (b) {
    isinclude = isinclude && t.data.type == includeptn.type;
    isinclude = isinclude && t.data.size == includeptn.size;
    isinclude = isinclude && 0 == strncmp(includeptn.data, code + t.data.begin, includeptn.size);
    if (isinclude) {
      env->macro   = 1;
      env->include = 1;
    } else if (TOKEN_MACRO_BEGIN == t.data.type) {
      env->macro = 1;
    } else if (TOKEN_MACRO_END == t.data.type) {
      env->macro   = 0;
      env->include = 0;
      t.data.size  = 0;
    }
    codetokens->tokens.data[codetokens->tokens.size++] = t.data;
  }
  codetokens->iter += (0 == t.data.size) + t.data.size;
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
