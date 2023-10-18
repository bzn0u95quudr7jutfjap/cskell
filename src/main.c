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

void consume_while_white(FILE *stream) {
  char c;
  while (is_white(c = fgetc(stream))) {
  }
  fseek(stream, -1, SEEK_CUR);
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

void print_inside_double_quote(FILE * stream){
  fseek(stream, -1, SEEK_CUR);
  char c = fgetc(stream);
  if (c != '"') {
    fprintf(stderr, "IL CARATTERE DI INIZIO '%c' NON È '\"'\n", c);
    return;
  }

  printf("\"");
  while ((c = fgetc(stream)) != '"') {
    if(c == '\\'){
      printf("\\%c",fgetc(stream));
    }else{
      printf("%c",c);
    }
  }
  printf("\"");

}

void print_inside_single_quote(FILE * stream){
  fseek(stream, -1, SEEK_CUR);
  char c = fgetc(stream);
  if (c != '\'') {
    fprintf(stderr, "IL CARATTERE DI INIZIO '%c' NON È '''\n", c);
    return;
  }

  printf("'");
  while ((c = fgetc(stream)) != '\'') {
    if(c == '\\'){
      printf("\\%c",fgetc(stream));
    }else{
      printf("%c",c);
    }
  }
  printf("'");
}

void format_parenthesis(FILE *stream) {
  fseek(stream, -1, SEEK_CUR);
  char c = fgetc(stream);
  if (c != '(') {
    fprintf(stderr, "IL CARATTERE DI INIZIO '%c' NON È '('\n", c);
    return;
  }

  printf("(");
  while ((c = fgetc(stream)) != ')') {
    if (c == '(') {
      format_parenthesis(stream);
    } else if (c == '"') {
      print_inside_double_quote(stream);
    } else if (c == '\'') {
      print_inside_single_quote(stream);
    } else if (c == ',') {
      printf(", ");
    } else if (is_white(c)) {
      consume_while_white(stream);
      printf(" ");
    } else {
      printf("%c", c);
    }
  }
  printf(")");
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

  char c = 0;
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
      format_parenthesis(f);
    } else if (c == '}') {
      do {
        consume_while_white(f);
        printf("\n");
        print_indentation(indentation_level);
        printf("%c", c);
        indentation_level--;
      } while ((c = fgetc(f)) == '}');
      printf("\n");
      print_indentation(indentation_level);
      if (indentation_level == 0) {
        printf("\n");
      }
      fseek(f, -1, SEEK_CUR);
    } else if (c == ';') {
      consume_while_white(f);
      printf("\n");
      print_indentation(indentation_level);
      printf("%c ", c);
    } else if (c == '{') {
      indentation_level++;
      printf("\n");
      print_indentation(indentation_level);
      printf("%c ", c);
      consume_while_white(f);
    } else {
      printf("%c", c);
    }
  }

  fclose(f);

  return 0;
}
