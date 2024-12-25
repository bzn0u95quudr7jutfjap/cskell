#ifndef STRING
#define STRING

#include <int.h>

#define count(a) (sizeof(a) / sizeof(*(a)))

typedef struct _IO_FILE FILE;

typedef enum {
  TOKEN_WORD = 1,
  TOKEN_STRING,
  TOKEN_NUMBER,
  TOKEN_SPECIAL,
  TOKEN_COMMENT_SL,
  TOKEN_COMMENT_ML,
  TOKEN_MACRO_BEGIN,
  TOKEN_MACRO_END,
  TOKEN_BLOCK_BEGIN,
  TOKEN_BLOCK_END,
  TOKEN_EXPR_BEGIN,
  TOKEN_EXPR_END,
  TOKEN_ARRAY_I_BEGIN,
  TOKEN_ARRAY_I_END,
  TOKEN_COMA,
  TOKEN_END_OF_LINE,
  TOKEN_OPERATOR_UNARY,
  TOKEN_OPERATOR_BINARY,
  TOKEN_OPERATOR_ATTRIBUTE,
  TOKEN_OPERATOR_PREPOSTFIX,
  TOKEN_OPERATOR_AMBIGUOUS,
} TokenType;

typedef struct {
  TokenType type;
  u32       begin;
  u32       size;
  u32       indentation;
  u8        newline_after : 2;
  u8        space_after   : 1;
  u8        definition    : 1;
} Token;

typedef struct {
  u32  size : 16;
  char data[1u << 16u];
} CharCollection;

typedef struct {
  u32   size : 14;
  Token data[1u << 14u];
} TokenCollection;

typedef struct {
  u32             iter;
  CharCollection  codice;
  TokenCollection tokens;
} CodeTokens;

typedef struct {
  u8  definitionspace;
  u8  globalspace;
  u8  macro   : 1;
  u8  include : 1;
  u32 indentation;
} TokenEnv;

u0 tokenizer(CodeTokens *codetokens);
u0 formatter(CodeTokens *codetokens);

u0 filegetcontents(char *path, CodeTokens *codetokens);
u0 fileputcontents(char *path, CodeTokens *codetokens);

u0 filegetcontents_fd(FILE *stream, CodeTokens *codetokens);
u0 fileputcontents_fd(FILE *stream, CodeTokens *codetokens);

#endif
