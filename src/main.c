#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "stack.h"
DECLARE_STACK(char, String);
DEFINE_STACK(char, String);

DECLARE_STACK(String, Stack_String);
DEFINE_STACK(String, Stack_String);

char *c_str(String *str) {
  push(str, '\0');
  pop(str);
  return str->data;
}

char fpeekbackc(FILE *stream) {
  fseek(stream, -1, SEEK_CUR);
  return fgetc(stream);
}

char fpeekc(FILE *stream) {
  char c = fgetc(stream);
  fseek(stream, -1, SEEK_CUR);
  return c;
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

bool is_any_of(char c, size_t size, const char cs[]) {
  for (size_t i = 0; i < size; i++) {
    if (c == cs[i]) {
      return true;
    }
  }
  return false;
}

bool is_white(char c) {
  static const char *const charset = "\n \t";
  return is_any_of(c, strlen(charset), charset);
}

bool is_speciale(char c) {
  static const char *const charset = "<>{}()[]#.;,+-*/";
  return is_any_of(c, strlen(charset), charset);
}

bool is_string_delimiter(char c) {
  static const char *const charset = "'\"";
  return is_any_of(c, strlen(charset), charset);
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

Stack_String parse_code_into_words(FILE *stream) {
  size_t pos = ftell(stream);
  fseek(stream, 0, SEEK_SET);

  Stack_String code = NewStack_String;
  push(&code, NewString);
  char c;
  while ((c = fgetc(stream)) != EOF) {
    // commenti
    if (c == '/' && fpeekc(stream) == '/') {
      push(&code, NewString);
      String *line = &(code.data[code.size - 1]);
      push(line, c);
      while ((c = fgetc(stream)) != EOF && c != '\n') {
        push(line, c);
      }
      push(&code, NewString);
      continue;
    }

    if (is_white(c)) {
      push(&code, NewString);
    } else if (is_speciale(c)) {
      push(&code, NewString);
      String *line = &(code.data[code.size - 1]);
      push(line, c);
      push(&code, NewString);
    } else if (true) {
      String *line = &(code.data[code.size - 1]);
      push(line, c);
    }
  }

  fseek(stream, pos, SEEK_SET);
  return code;
}

Stack_String remove_empty_strings(Stack_String stack) {
  Stack_String filtered = NewStack_String;
  for (size_t i = 0; i < stack.size; i++) {
    String line = stack.data[i];
    if (line.size > 0) {
      push(&filtered, line);
    }
  }
  return filtered;
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

  Stack_String codeblocks = remove_empty_strings(parse_code_into_words(f));
  for (size_t i = 0; i < codeblocks.size; i++) {
    String *line = &(codeblocks.data[i]);
    printf("%zu : %s\n", i, c_str(line));
  }

  fclose(f);

  return 0;
}
