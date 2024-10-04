#include "string_class.h"
#include "test.h"
#include <int.h>
#include <stdbool.h>
#include <stdio.h>

u0 *realloc(u0 *, u64);

Stack_String ss_ca(int argc, char **argv) {
  Stack_String ss1 = new_Stack_String();
  for (int i = 0; i < argc; i++) {
    push_Stack_String(&ss1, from_cstr(argv[i]));
  }
  return ss1;
}

#define push_test_tokenizer(a_name, a_i, a_o...)                                                                       \
  do {                                                                                                                 \
    char *o[] = a_o;                                                                                                   \
    push_stack_test(&g_tests, (testdata){                                                                              \
                                  .name = a_name,                                                                      \
                                  .code = 1,                                                                           \
                                  .vtable = functions_test_tokenizer,                                                  \
                                  .tokenizer =                                                                         \
                                      {                                                                                \
                                          .function = tokenizer,                                                       \
                                          .input = from_cstr(a_i),                                                     \
                                          .output = new_Stack_String(),                                                \
                                          .atteso = ss_ca(sizeof(o) / sizeof(*o), o),                                  \
                                      },                                                                               \
                              });                                                                                      \
  } while (0)

#define push_test_formatter(a_name, a_i, a_o, a_args...)

declare_template_stack_c(testdata, stack_test);
define_template_stack_c(testdata, stack_test);

stack_test g_tests = {};

int main(int argc, char *argv[]) {
  push_test_tokenizer("token_prova", "", {});
  push_test_tokenizer("token_variabile_strana", "int _a1_bc3;", {"int", "_a1_bc3", ";"});
  push_test_tokenizer("token_tipo_strano", "_uint32_t a;", {"_uint32_t", "a", ";"});
  push_test_tokenizer("token_variabile_strana_tipo_strano", "_uint32_t _a1_bc3;", {"_uint32_t", "_a1_bc3", ";"});
  push_test_tokenizer("token_variabile", "int a = 12;", {"int", "a", "=", "12", ";"});
  push_test_tokenizer("token_variabile_binary", "int a = 0b010;", {"int", "a", "=", "0b010", ";"});
  push_test_tokenizer("token_variabile_octal", "int a = 010;", {"int", "a", "=", "010", ";"});
  push_test_tokenizer("token_variabile_hex", "int a = 0xdeadBeef;", {"int", "a", "=", "0xdeadBeef", ";"});
  push_test_tokenizer("token_variabile_negative", "int a = -12;", {"int", "a", "=", "-", "12", ";"});
  push_test_tokenizer("token_variabile_long", "long a = 01L;", {"long", "a", "=", "01L", ";"});
  push_test_tokenizer("token_variabile_double", "double a = 12.34;", {"double", "a", "=", "12", ".", "34", ";"});
  push_test_tokenizer("token_variabile_float", "float a = 12.34f;", {"float", "a", "=", "12", ".", "34f", ";"});
  push_test_tokenizer("token_variabile_float_dot", "float a = .34f;", {"float", "a", "=", ".", "34f", ";"});
  push_test_tokenizer("token_variabile_float_dot_negative", "float a = -.34f;",
                      {"float", "a", "=", "-", ".", "34f", ";"});
  push_test_tokenizer("token_operatori_doppi", "int a = 0; a += 1; a = a >= 0;",
                      {"int", "a", "=", "0", ";", "a", "+=", "1", ";", "a", "=", "a", ">=", "0", ";"});
  push_test_tokenizer("token_stringhe", "char * a = \"int o a = 12 ;\";",
                      {"char", "*", "a", "=", "\"int o a = 12 ;\"", ";"});
  push_test_tokenizer("token_stringhe_incomplete", "char * a = \"int o a = 12 ;\nint b = 11;",
                      {"char", "*", "a", "=", "\"int o a = 12 ;", "int", "b", "=", "11", ";"});
  push_test_tokenizer("token_stringhe_escape", "char * a = \"char * a = \\\"12\\\" ;\";",
                      {"char", "*", "a", "=", "\"char * a = \\\"12\\\" ;\"", ";"});
  push_test_tokenizer("token_char", "char a = 'a';", {"char", "a", "=", "'a'", ";"});
  push_test_tokenizer("token_char_escape", "char a = '\\'';", {"char", "a", "=", "'\\''", ";"});
  push_test_tokenizer("token_commenti_monolinea", "int a; // int a = 12;\nint b;",
                      {"int", "a", ";", "// int a = 12;", "int", "b", ";"});
  push_test_tokenizer("token_commenti_monolinea_senza_newline", "int a; // int a = 12;",
                      {"int", "a", ";", "// int a = 12;"});
  push_test_tokenizer("token_commenti_multilinea", "int a; /* int a = 12;\nint b;*/ int c;",
                      {"int", "a", ";", "/* int a = 12;\nint b;*/", "int", "c", ";"});
  push_test_tokenizer("token_commenti_multilinea_senza_terminatore", "int a; /* int a = 12;\nint b;",
                      {"int", "a", ";", "/* int a = 12;\nint b;"});
  // test per le macro
  push_test_tokenizer("token_macro_semplice", "#define a 0", {"#", "define", "a", "0", "\n"});
  push_test_tokenizer("macro_semplice_con_concatenazione", "#define a a##a",
                      {"#", "define", "a", "a", "#", "#", "a", "\n"});
  push_test_tokenizer("token_macro_semplice_con_concatenazione_con_nomi_strani", "#define a a_##a",
                      {"#", "define", "a", "a_", "#", "#", "a", "\n"});
  push_test_tokenizer("token_macro_funzionale_semplice", "#define a() a", {"#", "define", "a", "(", ")", "a", "\n"});
  push_test_tokenizer("token_macro_funzionale_semplice", "#define count(a) (sizeof(a)/sizeof(*a))",
                      {"#", "define", "count", "(", "a", ")", "(", "sizeof", "(", "a", ")", "/", "sizeof", "(", "*",
                       "a", ")", ")", "\n"});
  push_test_tokenizer("token_macro_funzionale_stringify", "#define a(A) int A = #A;",
                      {"#", "define", "a", "(", "A", ")", "int", "A", "=", "#", "A", ";", "\n"});
  // test esempio di un codice
  push_test_tokenizer("token_macro_test_codice_serio",
                      "#include <fumo.h>\n"
                      "\n"
                      "u32 deltasigma(i32 a, i32 b){\n"
                      "  i32 sum = a + b;\n"
                      "\treturn 0 < sum ? sum : 0;\n"
                      "}\n",
                      {"#", "include", "<",   "fumo", ".",   "h", ">",   "\n",  "u32", "deltasigma", "(", "i32",
                       "a", ",",       "i32", "b",    ")",   "{", "i32", "sum", "=",   "a",          "+", "b",
                       ";", "return",  "0",   "<",    "sum", "?", "sum", ":",   "0",   ";",          "}"});

  push_test_formatter("token_prova", "", "");
  push_test_formatter("token_variabile_strana", "int _a1_bc3;", "int _a1_bc3\n;");
  push_test_formatter("token_tipo_strano", "_uint32_t a;", {"_uint32_t", "a", ";"});
  push_test_formatter("token_variabile_strana_tipo_strano", "_uint32_t _a1_bc3;", {"_uint32_t", "_a1_bc3", ";"});
  push_test_formatter("token_variabile", "int a = 12;", {"int", "a", "=", "12", ";"});
  push_test_formatter("token_variabile_binary", "int a = 0b010;", {"int", "a", "=", "0b010", ";"});
  push_test_formatter("token_variabile_octal", "int a = 010;", {"int", "a", "=", "010", ";"});
  push_test_formatter("token_variabile_hex", "int a = 0xdeadBeef;", {"int", "a", "=", "0xdeadBeef", ";"});
  push_test_formatter("token_variabile_negative", "int a = -12;", {"int", "a", "=", "-", "12", ";"});
  push_test_formatter("token_variabile_long", "long a = 01L;", {"long", "a", "=", "01L", ";"});
  push_test_formatter("token_variabile_double", "double a = 12.34;", {"double", "a", "=", "12", ".", "34", ";"});
  push_test_formatter("token_variabile_float", "float a = 12.34f;", {"float", "a", "=", "12", ".", "34f", ";"});
  push_test_formatter("token_variabile_float_dot", "float a = .34f;", {"float", "a", "=", ".", "34f", ";"});
  push_test_formatter("token_variabile_float_dot_negative", "float a = -.34f;",
                      {"float", "a", "=", "-", ".", "34f", ";"});
  push_test_formatter("token_operatori_doppi", "int a = 0; a += 1; a = a >= 0;",
                      {"int", "a", "=", "0", ";", "a", "+=", "1", ";", "a", "=", "a", ">=", "0", ";"});
  push_test_formatter("token_stringhe", "char * a = \"int o a = 12 ;\";",
                      {"char", "*", "a", "=", "\"int o a = 12 ;\"", ";"});
  push_test_formatter("token_stringhe_incomplete", "char * a = \"int o a = 12 ;\nint b = 11;",
                      {"char", "*", "a", "=", "\"int o a = 12 ;", "int", "b", "=", "11", ";"});
  push_test_formatter("token_stringhe_escape", "char * a = \"char * a = \\\"12\\\" ;\";",
                      {"char", "*", "a", "=", "\"char * a = \\\"12\\\" ;\"", ";"});
  push_test_formatter("token_char", "char a = 'a';", {"char", "a", "=", "'a'", ";"});
  push_test_formatter("token_char_escape", "char a = '\\'';", {"char", "a", "=", "'\\''", ";"});
  push_test_formatter("token_commenti_monolinea", "int a; // int a = 12;\nint b;",
                      {"int", "a", ";", "// int a = 12;", "int", "b", ";"});
  push_test_formatter("token_commenti_monolinea_senza_newline", "int a; // int a = 12;",
                      {"int", "a", ";", "// int a = 12;"});
  push_test_formatter("token_commenti_multilinea", "int a; /* int a = 12;\nint b;*/ int c;",
                      {"int", "a", ";", "/* int a = 12;\nint b;*/", "int", "c", ";"});
  push_test_formatter("token_commenti_multilinea_senza_terminatore", "int a; /* int a = 12;\nint b;",
                      {"int", "a", ";", "/* int a = 12;\nint b;"});
  push_test_formatter("token_macro_semplice", "#define a 0", {"#", "define", "a", "0", "\n"});
  push_test_formatter("macro_semplice_con_concatenazione", "#define a a##a",
                      {"#", "define", "a", "a", "#", "#", "a", "\n"});
  push_test_formatter("token_macro_semplice_con_concatenazione_con_nomi_strani", "#define a a_##a",
                      {"#", "define", "a", "a_", "#", "#", "a", "\n"});
  push_test_formatter("token_macro_funzionale_semplice", "#define a() a", {"#", "define", "a", "(", ")", "a", "\n"});
  push_test_formatter("token_macro_funzionale_semplice", "#define count(a) (sizeof(a)/sizeof(*a))",
                      {"#", "define", "count", "(", "a", ")", "(", "sizeof", "(", "a", ")", "/", "sizeof", "(", "*",
                       "a", ")", ")", "\n"});
  push_test_formatter("token_macro_funzionale_stringify", "#define a(A) int A = #A;",
                      {"#", "define", "a", "(", "A", ")", "int", "A", "=", "#", "A", ";", "\n"});

  printf("\n");
  for (u32 i = 0; i < g_tests.size; i++) {
    testdata *t = at_stack_test(&g_tests, i);
    exec_test(t);
    print_test(t);
    printf("\n");
  }

  uint32_t successes = 0;
  uint32_t failures = 0;
  uint32_t *t_ints[2] = {&failures, &successes};
  for (uint32_t i = 0; i < g_tests.size; i++) {
    (*(t_ints[success(at_stack_test(&g_tests, i))]))++;
  }

  printf("%6ld : Totale di test svolti\n", g_tests.size);
  printf("%6d : Successi\n", successes);
  printf("%6d : Fallimenti\n", failures);

  printf("\n");
  printf("\n");

  return 0;
}
