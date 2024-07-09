#include "../src/stack_string.h"
#include "../src/string_class.h"
#include <stdbool.h>
#include <stdio.h>

void assertTrue(char *name, bool b) { printf("Testing :: [%3s] : %s\n", b ? "OK" : "ERR", name); }

int test_tokenization(char *name, Stack_String (*function)(String *), String *input, Stack_String *output) {
  Stack_String a = function(input);
  bool errcode = 0;
  int min = 0;
  errcode += (a.size == output->size) != true;
  for (int i = 0; i < a.size && i < output->size; i++) {
    errcode += !equals(&(a.data[i]), &(output->data[i]));
  }
  assertTrue(name, errcode == 0);
  return errcode;
}

Stack_String id(String *a) { return NewStack_String; };

int main(int argc, char *argv[]) {
  int errcode = 0;
  String s = NewString;
  Stack_String ss = NewStack_String;
  errcode += test_tokenization("prova del test", id, &s, &ss) == 0;
  push(&ss,NewString);
  errcode += test_tokenization("prova del test", id, &s, &ss) == 0;
  errcode += test_tokenization("prova del test", id, &s, &ss) == 0;
  errcode += test_tokenization("prova del test", id, &s, &ss) == 0;
  return errcode;
}
