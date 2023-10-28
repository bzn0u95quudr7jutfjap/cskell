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

bool equals(String *a, String *b) { return strcmp(c_str(a), c_str(b)) == 0; }

#define from_cstr(STR)                                                                                                                                         \
  { .data = STR "\0\0", .size = strlen(STR "\0") + 1, .capacity = strlen(STR "\0") + 2 }

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

    /* TODO TESTING
     * del COMMENTO multilinea
     */
    // commenti multilinea
    if (c == '/' && fpeekc(stream) == '*') {
      push(&code, NewString);
      String *line = &(code.data[code.size - 1]);
      push(line, c);
      while ((c = fgetc(stream)) != EOF && !(c == '*' && fpeekc(stream) == '/')) {
        push(line, c);
      }
      push(line, c);
      push(line, fgetc(stream));
      push(&code, NewString);
      continue;
    }

    if (is_string_delimiter(c)) {
      char delimiter = c;
      String str = NewString;
      push(&str, delimiter);
      while ((c = fgetc(stream)) != EOF && c != delimiter) {
        if (c == '\\') {
          push(&str, c);
          push(&str, fgetc(stream));
        } else {
          push(&str, c);
        }
      }
      push(&str, delimiter);
      push(&code, str);
      push(&code, NewString);
      continue;
    }

    if (is_white(c)) {
      push(&code, NewString);
      continue;
    }

    if (is_speciale(c)) {
      push(&code, NewString);
      String *line = &(code.data[code.size - 1]);
      push(line, c);
      push(&code, NewString);
      continue;
    }

    if (true) {
      String *line = &(code.data[code.size - 1]);
      push(line, c);
      continue;
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
    printf("%7zu : %s\n", i, c_str(line));
  }

  fclose(f);

  return 0;
}
