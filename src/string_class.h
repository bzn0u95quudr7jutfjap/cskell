#ifndef STRING
#define STRING

#include <int.h>

#define count(a) (sizeof(a) / sizeof(*(a)))

typedef struct _IO_FILE FILE;

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
  u8     definitionspace;
  u8     globalspace;
  u8     macro   : 1;
  u8     include : 1;
  u32    indentation;
  Token *t;
} TokenEnv;

u0 tokenizer(CodeTokens *codetokens);
u0 formatter(CodeTokens *codetokens);

u0 filegetcontents(char *path, CodeTokens *codetokens);
u0 fileputcontents(char *path, CodeTokens *codetokens);

u0 filegetcontents_fd(FILE *stream, CodeTokens *codetokens);
u0 fileputcontents_fd(FILE *stream, CodeTokens *codetokens);

#endif
