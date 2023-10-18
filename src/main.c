#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void print_indentation(int level) {
  for (int i = 0; i < level; i++) {
    printf(" ");
  }
}

bool is_white(char c) { return c == '\t' || c == ' ' || c == '\n'; }

char consume_while_white(FILE *stream) {
  char c;
  while (is_white(c = fgetc(stream))) {
  };
  return c;
}

void consume_until_char(FILE *stream, char end) {
  char c;
  while ((c = fgetc(stream)) == end) {
  };
}

int main(int argc, const char *argv[]) {
  if (argc == 1) {
    fprintf(stderr, "File da formattare non dato\n\nSINTASSI: %s <FILE>\n\n",
            argv[0]);
    return 1;
  }

  FILE *f = fopen(argv[1], "r");
  if (!f) {
    fprintf(stderr, "File %s invalido\n\n", argv[1]);
    return 2;
  }

  char c = 0;
  int skip_endline = 0;
  int indentation_level = 0;
  while ((c = fgetc(f)) != EOF) {
    if (c == '\'') {
      consume_until_char(f,'\'');
    } else if (c == '"') {
      consume_until_char(f,'"');
    } else if (c == '{') {
      indentation_level += 1;
      printf("\n");
      char c1 = consume_while_white(f);
      printf("%c", c);
      print_indentation(indentation_level);
      printf("%c", c1);
    } else if (c == '}') {
      indentation_level -= 1;
      printf("\n");
      char c1 = consume_while_white(f);
      printf("%c", c);
      print_indentation(indentation_level);
      printf("%c", c1);
    } else if (c == ';') {
      printf("\n");
      char c1 = consume_while_white(f);
      printf("%c", c);
      print_indentation(indentation_level);
      printf("%c", c1);
    } else {
      printf("%c", c);
    }
  }

  fclose(f);

  return 0;
}
