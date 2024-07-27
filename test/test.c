#include <stack_string.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <string_class.h>
#include <token.h>

char *status(int e) { return e ? "OK" : "ERR"; }
#define foreach(array, iter, body)                                                                                     \
  for (size_t _i = 0; _i < array->size; _i++) {                                                                        \
    typeof(at(array, _i)) iter = at(array, _i);                                                                        \
    body                                                                                                               \
  }

String *s_c(char *str) {
  static String s;
  String_free(&s);
  String s1 = from_cstr(str);
  memcpy(&s, &s1, sizeof(s1));
  return &s;
}

Stack_String *ss_ca(int argc, char *argv[]) {
  static Stack_String ss;
  Stack_String_free(&ss);
  Stack_String ss1 = NewStack_String;
  for (int i = 0; i < argc; i++) {
    push(&ss1, from_cstr(argv[i]));
  }
  memcpy(&ss, &ss1, sizeof(ss1));
  return &ss;
}

int test_tokenization(char *name, Stack_String (*function)(String *), String *input, Stack_String *output) {
  printf("%6s :: %s\n", "INIZIO", name);
  Stack_String a = function(input);
  bool errcode = 0;
  int min = 0;
  errcode += (a.size == output->size) != true;
  for (int i = 0; i < a.size && i < output->size; i++) {
    errcode += !equals(&(a.data[i]), &(output->data[i]));
  }
  printf("%6s :: [%3s] :: %10s :: ", "STATUS", status(errcode == 0), "Atteso");
  foreach (output, i, { printf("{%s}", c_str(i)); })
    printf("\n");
  printf("%6s :: [%3s] :: %10s :: ", "STATUS", status(errcode == 0), "Ottenuto");
  foreach ((&a), i, { printf("{%s}", c_str(i)); })
    printf("\n");
  printf("%6s :: %s\n", "FINE", name);
  String_free(input);
  Stack_String_free(output);
  Stack_String_free(&a);
  return errcode;
}

#define test_tokenization(name, function, string, argv)                                                                \
  test_tokenization(#name, function, s_c(string), ss_ca(sizeof(argv) / sizeof(argv[0]), argv))

Stack_String id(String *a) { return NewStack_String; };

int main(int argc, char *argv[]) {
  printf("\n");
  {
    char *a = "";
    char *o[] = {};
    test_tokenization(token_prova, tokenizer, a, o);
  }
  printf("\n");
  {
    char *a = "int _a1_bc3;";
    char *o[] = {"int", "_a1_bc3", ";"};
    test_tokenization(token_variabile_strana, tokenizer, a, o);
  }
  printf("\n");
  {
    char *a = "_uint32_t a;";
    char *o[] = {"_uint32_t", "a", ";"};
    test_tokenization(token_tipo_strano, tokenizer, a, o);
  }
  printf("\n");
  {
    char *a = "_uint32_t _a1_bc3;";
    char *o[] = {"_uint32_t", "_a1_bc3", ";"};
    test_tokenization(token_variabile_strana_tipo_strano, tokenizer, a, o);
  }
  printf("\n");
  {
    char *a = "int a = 12;";
    char *o[] = {"int", "a", "=", "12", ";"};
    test_tokenization(token_variabile, tokenizer, a, o);
  }
  printf("\n");
  {
    char *a = "int a = 0b010;";
    char *o[] = {"int", "a", "=", "0b010", ";"};
    test_tokenization(token_variabile_binary, tokenizer, a, o);
  }
  printf("\n");
  {
    char *a = "int a = 010;";
    char *o[] = {"int", "a", "=", "010", ";"};
    test_tokenization(token_variabile_octal, tokenizer, a, o);
  }
  printf("\n");
  {
    char *a = "int a = 0xdeadBeef;";
    char *o[] = {"int", "a", "=", "0xdeadBeef", ";"};
    test_tokenization(token_variabile_hex, tokenizer, a, o);
  }
  printf("\n");
  {
    char *a = "int a = -12;";
    char *o[] = {"int", "a", "=", "-", "12", ";"};
    test_tokenization(token_variabile_negative, tokenizer, a, o);
  }
  printf("\n");
  {
    char *a = "long a = 01L;";
    char *o[] = {"long", "a", "=", "01L", ";"};
    test_tokenization(token_variabile_long, tokenizer, a, o);
  }
  printf("\n");
  {
    char *a = "double a = 12.34;";
    char *o[] = {"double", "a", "=", "12", ".", "34", ";"};
    test_tokenization(token_variabile_double, tokenizer, a, o);
  }
  printf("\n");
  {
    char *a = "float a = 12.34f;";
    char *o[] = {"float", "a", "=", "12", ".", "34f", ";"};
    test_tokenization(token_variabile_float, tokenizer, a, o);
  }
  printf("\n");
  {
    char *a = "float a = .34f;";
    char *o[] = {"float", "a", "=", ".", "34f", ";"};
    test_tokenization(token_variabile_float_dot, tokenizer, a, o);
  }
  printf("\n");
  {
    char *a = "float a = -.34f;";
    char *o[] = {"float", "a", "=", "-", ".", "34f", ";"};
    test_tokenization(token_variabile_float_dot_negative, tokenizer, a, o);
  }
  printf("\n");
  {
    char *a = "int a = 0; a += 1; a = a >= 0;";
    char *o[] = {"int", "a", "=", "0", ";", "a", "+=", "1", ";", "a", "=", "a", ">=", "0", ";"};
    test_tokenization(token_operatori_doppi, tokenizer, a, o);
  }
  printf("\n");
  {
    char *a = "int a; // int a = 12;\nint b;";
    char *o[] = {"int", "a", ";", "// int a = 12;", "int", "b", ";"};
    test_tokenization(token_commenti, tokenizer, a, o);
  }
  printf("\n");
  {
    char *a = "char * a = \"int o a = 12 ;\";";
    char *o[] = {"char", "*", "a", "=", "\"int o a = 12 ;\"", ";"};
    test_tokenization(token_stringhe, tokenizer, a, o);
  }
  printf("\n");
  {
    char *a = "char * a = \"int o a = 12 ;\nint b = 11;";
    char *o[] = {"char", "*", "a", "=", "\"int o a = 12 ;", "int", "b", "=", "11", ";"};
    test_tokenization(token_stringhe_incomplete, tokenizer, a, o);
  }
  printf("\n");
  {
    char *a = "char * a = \"char * a =\\\"12\\\" ;\";";
    char *o[] = {"char", "*", "a", "=", "\"char * a = \\\"12\\\" ;\"", ";"};
    test_tokenization(token_stringhe_escape, tokenizer, a, o);
  }
  printf("\n");
  {
    char *a = "char a = 'a';";
    char *o[] = {"char", "a", "=", "'a'", ";"};
    test_tokenization(token_char, tokenizer, a, o);
  }
  printf("\n");
  {
    char *a = "char a = '\\'';";
    char *o[] = {"char", "a", "=", "'\\''", ";"};
    test_tokenization(token_char_escape, tokenizer, a, o);
  }
  printf("\n");
  return 0;
}
