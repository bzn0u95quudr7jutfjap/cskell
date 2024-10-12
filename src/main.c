#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <stack.h>
#include <string_class.h>

u0 print_formatted_code(FILE *out, Formatter *fmt);

String file_get_contents(char *file) {
  String tmp = new_String();
  char c = EOF;
  FILE *stream = fopen(file, "r");
  if (stream == NULL) {
    return tmp;
  }
  while ((c = fgetc(stream)) != EOF) {
    push(&tmp, c);
  }
  fclose(stream);
  return tmp;
}

String string_view(Formatter *fmt, u32 i) {
  Token *t = at(&fmt->tokens, i);
  return (String){.data = fmt->str.data + t->begin, .size = t->size};
}

int main(int argc, char *argv[]) {
  if (!(argc == 2 || argc == 3)) {
    printf("Uso invalido\n\n$ %s <INPUT_FILE> [OUTPUT_FILE]\n\n"
           "Se OUTPUT_FILE non è specificato viene usato stdout\n\n",
           argv[0]);
    return 1;
  }
  char *input = argc < 2 ? NULL : argv[1];
  if (input == NULL) {
    exit(EXIT_FAILURE);
  }
  Formatter fmt = {};
  fmt.str = file_get_contents(input);
  tokenizer(&fmt);
  formatter(&fmt);
  char *output = argc < 3 ? NULL : argv[2];
  if (output == NULL) {
    print_formatted_code(stdout, &fmt);
  } else {
    FILE *out = fopen(output, "w");
    if (out != NULL) {
      print_formatted_code(out, &fmt);
    }
    fclose(out);
  }
  free_Formatter(&fmt);
  return 0;
}
