#ifndef STRING
#define STRING

#include "stack.h"
#include <int.h>
#include <stddef.h>

declare_template_stack_c(char, String);
declare_template_stack_c(String, Stack_String);

typedef enum {
  TOKEN_IDENTIFIER = 1,
  TOKEN_STRING,
  TOKEN_NUMBER,
  TOKEN_OPERATOR,
  TOKEN_SPECIAL,
  TOKEN_COMMENT_SL,
  TOKEN_COMMENT_ML,
  TOKEN_MACRO_BEGIN,
  TOKEN_MACRO_END,
} token_type;

typedef struct {
  token_type type;
  u32 begin;
  u32 size;
  u32 indentation;
  u8 newline_before : 1;
  u8 newline_after : 2;
  u8 space_before : 1;
  u8 space_after : 1;
} Token;

typedef struct {
  u8 macro : 1;
  u32 indentation;
  Token *prev;
} tokenizer_env;

declare_template_stack_c(Token, Stack_Token);

typedef struct {
  String str;
  Stack_Token tokens;
} Formatter;

u0 free_Formatter(Formatter *fmt);

u8 is_any_of(char c, size_t size, const char cs[]);
u0 tokenizer(Formatter *stream);
u0 formatter(Formatter *fmt);

char *c_str(String *str);
String from_cstr(char *str);
void move_into(String *dst, String *src);
void String_delete(String *str);

typedef struct {
  String *str;
  u32 idx;
  u32 is_end;
  char val;
} Iter_String;

Iter_String sseekres(String *stream);
char sgetc(Iter_String *stream);
char speekc(Iter_String *stream);
char speekoffset(Iter_String *stream, int o);
void sseekcur(Iter_String *stream, i32 o);

#endif
