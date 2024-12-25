#include <stdio.h>

// ================================================================================
// IO
// ================================================================================

u0 filegetcontents_fd(FILE *stream, CodeTokens *codetokens) {
  codetokens->codice.size = 0;
  codetokens->tokens.size = 0;
  CharCollection *code    = &codetokens->codice;
  do {
    code->data[code->size++] = fgetc(stream);
  } while (code->size < count(code->data) && !feof(stream));
  code->size += -(code->size > 0);
}

u0 filegetcontents(char *path, CodeTokens *codetokens) {
  FILE *stream = fopen(path, "r");
  if (stream == NULL) {
    return;
  }
  filegetcontents_fd(stream, codetokens);
  fclose(stream);
}

static u0 ifp(u32 i, FILE *out, char *str) {
  while (i--) {
    fprintf(out, "%s", str);
  }
}

u0 fileputcontents_fd(FILE *stream, CodeTokens *codetokens) {
  char    *indent    = "  ";
  char    *space     = " ";
  char    *newline   = "\n";
  char    *backslash = "\\";
  u32      size      = codetokens->tokens.size;
  TokenEnv env       = {};
  for (u32 i = 0; i < size; i++) {
    Token *t   = &codetokens->tokens.data[i];
    char  *str = &codetokens->codice.data[t->begin];
    i32    len = t->size;
    env.macro  = TOKEN_MACRO_END == t->type ? 0 : (TOKEN_MACRO_BEGIN == t->type ? 1 : env.macro);
    ifp(t->indentation, stream, indent);
    fprintf(stream, "%.*s", len, str);
    if (t->newline_after == 0) {
      ifp(t->space_after, stream, space);
    } else if (!env.macro) {
      ifp(t->newline_after, stream, newline);
    } else {
      ifp(1, stream, space);
      ifp(1, stream, backslash);
      ifp(1, stream, newline);
    }
  }
}

u0 fileputcontents(char *path, CodeTokens *codetokens) {
  FILE *stream = fopen(path, "r");
  if (stream == NULL) {
    return;
  }
  fileputcontents_fd(stream, codetokens);
  fclose(stream);
}
