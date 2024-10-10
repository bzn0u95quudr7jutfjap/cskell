#include "test.h"
#include "string_class.h"
#include <stdio.h>

define_test(
    test_tokenizer,
    {
      t->tokenizer.function(&t->tokenizer.input);
      t->code = equals_formatter_strings(&t->tokenizer.input, &t->tokenizer.atteso);
    },
    {
      printf("%6s :: %s\n", "INIZIO", t->name);
      printf("%6s :: [%3s] :: %10s :: ", "STATUS", status(t), "Atteso");
      print_stack_string(&t->tokenizer.atteso);
      printf("\n");
      printf("%6s :: [%3s] :: %10s :: ", "STATUS", status(t), "Output");
      print_formatter(&t->tokenizer.input);
      printf("\n");
      switch (t->code) {
      case CODE_NULL_RESULT:
        printf("%6s :: [%3s] :: %s\n", "STATUS", status(t), "CODE_NULL_RESULT");
        printf("%6s :: [%3s] :: %10s :: %p\n", "STATUS", status(t), "Atteso", &t->tokenizer.atteso);
        printf("%6s :: [%3s] :: %10s :: %p\n", "STATUS", status(t), "Output", &t->tokenizer.input);
        break;
      case CODE_TOKEN_STR_DIFFER:
        printf("%6s :: [%3s] :: %s\n", "STATUS", status(t), "CODE_TOKEN_STR_DIFFER");
        break;
      case CODE_TOKEN_LEN_DIFFER:
        printf("%6s :: [%3s] :: %s\n", "STATUS", status(t), "CODE_TOKEN_LEN_DIFFER");
        printf("%6s :: [%3s] :: %10s :: LEN : %6lu\n", "STATUS", status(t), "Atteso", t->tokenizer.atteso.size);
        printf("%6s :: [%3s] :: %10s :: LEN : %6lu\n", "STATUS", status(t), "Output", t->tokenizer.input.tokens.size);
        break;
      default:
        break;
      }
      printf("%6s :: %s\n", "FINE", t->name);
    },
    {
      free_Formatter(&t->tokenizer.input);
      free_Stack_String(&t->tokenizer.atteso);
    });
