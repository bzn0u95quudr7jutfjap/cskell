#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void print_indentation(int level) {
  for (int i = 0; i < level; i++) {
    printf("  ");
  }
}

bool is_white(char c) { return c == '\t' || c == ' ' || c == '\n'; }

char consume_while_white(FILE *stream) {
  char c;
  while (is_white(c = fgetc(stream))) {
  };
  return c;
}

void print_inside_delimiters(FILE *stream, char start, char end) {
  int count = 1;
  fseek(stream, -1, SEEK_CUR);
  char c = fgetc(stream);

  if (c != start) {
    fprintf(stderr, "CARATTERE DI INIZIO NON È IL DELIMITATORE ('%c' != '%c')\n", c, start);
    return;
  }

  for (; (c != end && count != 0) && c != EOF; c = fgetc(stream)) {
    printf("%c", c);
    if (c == start) {
      count += 1;
    }
    if (count == end) {
      count -= 1;
    }
  }
  printf("%c", c);
}

bool is_char_any_of(char c, size_t len, char possibles[]) {
  for (int i = 0; i < len; i++) {
    if (c == possibles[i]) {
      return true;
    }
  }
  return false;
}

int main(int argc, const char *argv[]) {
  if (argc == 1) {
    fprintf(stderr, "File da formattare non dato\n\nSINTASSI: %s <FILE>\n\n", argv[0]);
    return 1;
  }

  FILE *f = fopen(argv[1], "r");
  if (!f) {
    fprintf(stderr, "File %s invalido\n\n", argv[1]);
    return 2;
  }

  int c = 0;
  int indentation_level = 0;
  fseek(f, 0, SEEK_END);
  size_t file_len = ftell(f);
  fseek(f, 0, SEEK_SET);
  for (int i = 0; (c = fgetc(f)) != EOF && i < file_len + 12; i++) {
    if (c == '\'') {
      print_inside_delimiters(f, '\'', '\'');
    } else if (c == '"') {
      print_inside_delimiters(f, '"', '"');
    } else if (c == '(') {
      print_inside_delimiters(f, '(', ')');
    } else if (is_char_any_of(c, 3, "{};")) {
      printf("\n");
      if (c == '{') {
        indentation_level += 1;
      }
      if (c == '}') {
        indentation_level -= 1;
        printf("\n");
      }
      char c1 = consume_while_white(f);
      print_indentation(indentation_level);
      printf("%c  ", c);
      fseek(f, -1, SEEK_CUR);
    } else {
      printf("%c", c);
    }
  }

  fclose(f);

  return 0;
}
