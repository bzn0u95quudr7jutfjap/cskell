#include "string_class.h"
#include "test.h"
#include "test_token.c"
#include <int.h>
#include <stdbool.h>
#include <stdio.h>

u0 *realloc(u0 *, u64);

declare_template_stack_c(testdata, stack_test);
define_template_stack_c(testdata, stack_test);

stack_test g_tests = {};

typedef testdata (*return_test_func)();
u0 push_test(return_test_func f) { push_stack_test(&g_tests, f()); }

int main(int argc, char *argv[]) {
  push_test(token_prova);
  push_test(token_variabile_strana);
  push_test(token_tipo_strano);
  push_test(token_variabile_strana_tipo_strano);
  push_test(token_variabile);
  push_test(token_variabile_binary);
  push_test(token_variabile_octal);
  push_test(token_variabile_hex);
  push_test(token_variabile_negative);
  push_test(token_variabile_long);
  push_test(token_variabile_double);
  push_test(token_variabile_float);
  push_test(token_variabile_float_dot);
  push_test(token_variabile_float_dot_negative);
  push_test(token_operatori_doppi);
  push_test(token_stringhe);
  push_test(token_stringhe_incomplete);
  push_test(token_stringhe_escape);
  push_test(token_char);
  push_test(token_char_escape);
  push_test(token_commenti_monolinea);
  push_test(token_commenti_monolinea_senza_newline);
  push_test(token_commenti_multilinea);
  push_test(token_commenti_multilinea_senza_terminatore);
  push_test(token_macro_semplice);
  push_test(macro_semplice_con_concatenazione);
  push_test(token_macro_semplice_con_concatenazione_con_nomi_strani);
  push_test(token_macro_funzionale_semplice);
  push_test(token_macro_funzionale_semplice);
  push_test(token_macro_funzionale_stringify);
  push_test(token_macro_test_codice_serio);

  printf("\n");
  for (u32 i = 0; i < g_tests.size; i++) {
    testdata *t = at_stack_test(&g_tests, i);
    exec_test(t);
    print_test(t);
    printf("\n");
  }

  union {
    struct {
      u32 failures;
      u32 successes;
    };
    u32 vals[2];
  } report = {};

  for (uint32_t i = 0; i < g_tests.size; i++) {
    report.vals[success(at_stack_test(&g_tests, i))]++;
  }

  printf("%6ld : Totale di test svolti\n", g_tests.size);
  printf("%6d : Successi\n", report.successes);
  printf("%6d : Fallimenti\n", report.failures);

  printf("\n");
  printf("\n");

  return 0;
}
