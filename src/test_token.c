#ifndef TOKEN_UNIT_TESTING_C
#define TOKEN_UNIT_TESTING_C

#include "string_class.h"
#include "test.h"

#define make_test_token(a_name, a_i, a_o...)                                                                           \
  testdata a_name() {                                                                                                  \
    char *o[] = a_o;                                                                                                   \
    return (testdata){                                                                                                 \
        .name = #a_name,                                                                                               \
        .code = CODE_NOT_RUN,                                                                                          \
        .vtable = functions_test_tokenizer,                                                                            \
        .tokenizer =                                                                                                   \
            {                                                                                                          \
                .function = tokenizer,                                                                                 \
                .input = {.str = from_cstr(a_i)},                                                                      \
                .atteso = ss_ca(o, sizeof(o) / sizeof(*o)),                                                            \
            },                                                                                                         \
    };                                                                                                                 \
  }                                                                                                                    \
  int var_name_test_##testname##_force_semicolon_at_macro_end

make_test_token(token_prova, "", {});

make_test_token(token_variabile_strana, "int _a1_bc3;", {"int", "_a1_bc3", ";"});
make_test_token(token_tipo_strano, "_uint32_t a;", {"_uint32_t", "a", ";"});
make_test_token(token_variabile_strana_tipo_strano, "_uint32_t _a1_bc3;", {"_uint32_t", "_a1_bc3", ";"});
make_test_token(token_variabile, "int a = 12;", {"int", "a", "=", "12", ";"});
make_test_token(token_variabile_binary, "int a = 0b010;", {"int", "a", "=", "0b010", ";"});
make_test_token(token_variabile_octal, "int a = 010;", {"int", "a", "=", "010", ";"});
make_test_token(token_variabile_hex, "int a = 0xdeadBeef;", {"int", "a", "=", "0xdeadBeef", ";"});
make_test_token(token_variabile_negative, "int a = -12;", {"int", "a", "=", "-", "12", ";"});
make_test_token(token_variabile_long, "long a = 01L;", {"long", "a", "=", "01L", ";"});
make_test_token(token_variabile_double, "double a = 12.34;", {"double", "a", "=", "12", ".", "34", ";"});
make_test_token(token_variabile_float, "float a = 12.34f;", {"float", "a", "=", "12", ".", "34f", ";"});
make_test_token(token_variabile_float_dot, "float a = .34f;", {"float", "a", "=", ".", "34f", ";"});
make_test_token(token_variabile_float_dot_negative, "float a = -.34f;", {"float", "a", "=", "-", ".", "34f", ";"});
make_test_token(token_operatori_doppi, "int a = 0; a += 1; a = a >= 0;",
                {"int", "a", "=", "0", ";", "a", "+=", "1", ";", "a", "=", "a", ">=", "0", ";"});
make_test_token(token_stringhe, "char * a = \"int o a = 12 ;\";", {"char", "*", "a", "=", "\"int o a = 12 ;\"", ";"});
make_test_token(token_stringhe_incomplete, "char * a = \"int o a = 12 ;\nint b = 11;",
                {"char", "*", "a", "=", "\"int o a = 12 ;", "int", "b", "=", "11", ";"});
make_test_token(token_stringhe_escape, "char * a = \"char * a = \\\"12\\\" ;\";",
                {"char", "*", "a", "=", "\"char * a = \\\"12\\\" ;\"", ";"});
make_test_token(token_char, "char a = 'a';", {"char", "a", "=", "'a'", ";"});
make_test_token(token_char_escape, "char a = '\\'';", {"char", "a", "=", "'\\''", ";"});
make_test_token(token_commenti_monolinea, "int a; // int a = 12;\nint b;",
                {"int", "a", ";", "// int a = 12;", "int", "b", ";"});
make_test_token(token_commenti_monolinea_senza_newline, "int a; // int a = 12;", {"int", "a", ";", "// int a = 12;"});
make_test_token(token_commenti_multilinea, "int a; /* int a = 12;\nint b;*/ int c;",
                {"int", "a", ";", "/* int a = 12;\nint b;*/", "int", "c", ";"});
make_test_token(token_commenti_multilinea_senza_terminatore, "int a; /* int a = 12;\nint b;",
                {"int", "a", ";", "/* int a = 12;\nint b;"});

// test per le macro
make_test_token(token_macro_semplice, "#define a 0", {"#", "define", "a", "0", "\n"});
make_test_token(macro_semplice_con_concatenazione, "#define a a##a", {"#", "define", "a", "a", "##", "a", "\n"});
make_test_token(token_macro_semplice_con_concatenazione_con_nomi_strani, "#define a a_##a",
                {"#", "define", "a", "a_", "##", "a", "\n"});
make_test_token(token_macro_funzionale_semplice, "#define a() a", {"#", "define", "a", "(", ")", "a", "\n"});
make_test_token(token_macro_funzionale_piu_complessa, "#define count(a) (sizeof(a)/sizeof(*a))",
                {"#", "define", "count", "(", "a", ")", "(", "sizeof", "(", "a", ")", "/", "sizeof", "(", "*", "a", ")",
                 ")", "\n"});
make_test_token(token_macro_funzionale_stringify, "#define a(A) int A = #A;",
                {"#", "define", "a", "(", "A", ")", "int", "A", "=", "#", "A", ";", "\n"});

// test esempio di un codice
make_test_token(token_macro_test_codice_serio,
                "#include <fumo.h>\n"
                "\n"
                "u32 deltasigma(i32 a, i32 b){\n"
                "  i32 sum = a + b;\n"
                "\treturn 0 < sum ? sum : 0;\n"
                "}\n",
                {"#", "include", "<",   "fumo", ".",   "h", ">",   "\n",  "u32", "deltasigma", "(", "i32",
                 "a", ",",       "i32", "b",    ")",   "{", "i32", "sum", "=",   "a",          "+", "b",
                 ";", "return",  "0",   "<",    "sum", "?", "sum", ":",   "0",   ";",          "}"});

#endif // TOKEN_UNIT_TESTING_C
