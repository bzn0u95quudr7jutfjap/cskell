#ifndef TOKEN
#define TOKEN


#include <stack.h>
#include <string_class.h>

u8 is_any_of(char c, size_t size, const char cs[]);
Stack_String tokenizer(String * stream);

#endif // TOKEN
