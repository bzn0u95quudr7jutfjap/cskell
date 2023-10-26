#include <bits/pthreadtypes.h>
#include <bits/types/sigevent_t.h>
#include <bits/types/stack_t.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "stack.h"
DECLARE_STACK(char, String);
DEFINE_STACK(char, String);

DECLARE_STACK(String, Stack_String);
DEFINE_STACK(String, Stack_String);

char *c_str(String *str) {
  str->push(str, '\0');
  str->pop(str);
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
  static const char * const bianchi = "\n \t";
  static const size_t num = strlen(bianchi);
  return is_any_of(c, num, bianchi);
}

bool is_speciale(char c){
  static const char * const speciali = "<>{}()[]#.;,+-*/";
  static const size_t num_speciali = strlen(speciali);
  return is_any_of(c, num_speciali, speciali);
}

bool is_string_delimiter(char c){
  static const char * const speciali = "'\"";
  static const size_t num_speciali = strlen(speciali);
  return is_any_of(c, num_speciali, speciali);
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

String from_c_str(char *c_str) {
  String str = NewString;
  for (size_t i = 0; c_str[i]; i++) {
    str.push(&str, c_str[i]);
  }
  return str;
}

Stack_String parse_code_into_words(FILE *stream) {
  size_t pos = ftell(stream);
  fseek(stream, 0, SEEK_SET);


  Stack_String code = NewStack_String;
  code.push(&code, NewString);
  char c;
  while ((c = fgetc(stream)) != EOF) {
    // commenti
    if (c == '/' && fpeekc(stream) == '/') {
      code.push(&code, NewString);
      String *line = &(code.data[code.size - 1]);
      line->push(line, c);
      while ((c = fgetc(stream)) != EOF && c != '\n') {
        line->push(line, c);
      }
      code.push(&code, NewString);
      continue;
    }

    if (is_white(c)) {
      code.push(&code, NewString);
    } else if (is_speciale(c)) {
      code.push(&code, NewString);
      String *line = &(code.data[code.size - 1]);
      line->push(line, c);
      code.push(&code, NewString);
    } else if (true) {
      String *line = &(code.data[code.size - 1]);
      line->push(line, c);
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
      filtered.push(&filtered, line);
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
