#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char fpeekbackc(FILE *stream) {
  fseek(stream, -1, SEEK_CUR);
  return fgetc(stream);
}

size_t fsize(FILE *stream) {
  size_t pos = ftell(stream);
  fseek(stream, 0, SEEK_END);
  size_t size = ftell(stream);
  fseek(stream, pos, SEEK_SET);
  return size;
}

void print_indentation(int level) {
  for (int i = 0; i < level; i++) {
    fprintf(stdout, "  ");
  }
}

bool is_white(char c) { return c == '\t' || c == ' ' || c == '\n'; }

void consume_while_white(FILE *stream) {
  char c;
  size_t file_len = fsize(stream);
  for (size_t i = ftell(stream); is_white(c = fgetc(stream)) && i < file_len; i++) {
  }
  fseek(stream, -1, SEEK_CUR);
}

void print_inside_quote(FILE *stream, char delimiter) {
  char c = fpeekbackc(stream);
  if (c != delimiter) {
    fprintf(stderr, "IL CARATTERE DI INIZIO '%c' NON È '%c'\n", c, delimiter);
    return;
  }

  fprintf(stdout, "%c", delimiter);
  size_t file_len = fsize(stream);
  for (size_t i = ftell(stream); (c = fgetc(stream)) != delimiter && i < file_len; i++) {
    if (c == '\\') {
      fprintf(stdout, "\\%c", fgetc(stream));
    } else {
      fprintf(stdout, "%c", c);
    }
  }
  fprintf(stdout, "%c", delimiter);
}

void format_parenthesis(FILE *stream) {
  char c = fpeekbackc(stream);
  if (c != '(') {
    fprintf(stderr, "IL CARATTERE DI INIZIO '%c' NON È '('\n", c);
    return;
  }

  fprintf(stdout, "(");
  size_t file_len = fsize(stream);
  for (size_t i = ftell(stream); (c = fgetc(stream)) != ')' && i < file_len; i++) {
    if (c == '(') {
      format_parenthesis(stream);
    } else if (c == '"' || c == '\'') {
      print_inside_quote(stream, c);
    } else if (is_white(c)) {
      consume_while_white(stream);
    } else if (c == ',') {
      fprintf(stdout, ", ");
    } else if (is_white(c)) {
      consume_while_white(stream);
      fprintf(stdout, " ");
    } else {
      fprintf(stdout, "%c", c);
    }
  }
  fprintf(stdout, ")");
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
  size_t file_len = fsize(f);
  for (int i = 0; (c = fgetc(f)) != EOF && i < file_len + 12; i++) {
    if (c == '"' || c == '\'') {
      print_inside_quote(f, c);
    } else if (c == '(') {
      format_parenthesis(f);
    } else if (c == '}') {
      do {
        consume_while_white(f);
        fprintf(stdout, "\n");
        print_indentation(indentation_level);
        fprintf(stdout, "%c", c);
        indentation_level--;
      } while ((c = fgetc(f)) == '}');
      fprintf(stdout, "\n");
      print_indentation(indentation_level);
      if (indentation_level == 0) {
        fprintf(stdout, "\n");
      }
      fseek(f, -1, SEEK_CUR);
    } else if (c == ';') {
      consume_while_white(f);
      fprintf(stdout, "\n");
      print_indentation(indentation_level);
      fprintf(stdout, "%c ", c);
    } else if (c == '{') {
      indentation_level++;
      fprintf(stdout, "\n");
      print_indentation(indentation_level);
      fprintf(stdout, "%c ", c);
      consume_while_white(f);
    } else {
      fprintf(stdout, "%c", c);
    }
  }

  fclose(f);

  return 0;
}
