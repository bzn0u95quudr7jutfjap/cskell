#ifndef TOKEN
#define TOKEN

#include <stdbool.h>
#include <stdio.h>

#include <stack.h>
#include <string_class.h>

char fpeekc(FILE *stream);
bool is_any_of(char c, size_t size, const char cs[]);
Stack_String parse_code_into_words(FILE *stream);
Stack_String tokenizer(String *);

#endif // TOKEN
