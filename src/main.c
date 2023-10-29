#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "stack.h"
DECLARE_STACK(char, String);
DEFINE_STACK(char, String);

char *c_str(String *str) {
  if (str == NULL) {
    return NULL;
  }
  push(str, '\0');
  pop(str);
  return str->data;
}

bool equals(String *a, String *b) { return a != NULL && b != NULL && ((a == b) || ((a->size == b->size) && strcmp(c_str(a), c_str(b)) == 0)); }

#define from_cstr(STR)                                                                                                                                         \
  { .data = STR "\0\0", .size = strlen(STR "\0") + 1, .capacity = strlen(STR "\0") + 2 }

DECLARE_STACK(String, Stack_String);
DEFINE_STACK(String, Stack_String);

String *get(Stack_String *stack, size_t i) {
  if (i < stack->size) {
    return &(stack->data[i]);
  }
  if (i >= -(stack->size)) {
    return &(stack->data[i + stack->size]);
  }
  return NULL;
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

void append(String *a, String *b) {
  for (size_t i = 0; i < b->size; i++) {
    push(a, b->data[i]);
  }
}
void merge_include_macros(Stack_String *stack) {
  fprintf(stderr, "INIZIO MACRO FOLDING\n");
  String cancelletto = from_cstr("#");
  String include = from_cstr("include");
  String gt = from_cstr(">");
  char *null_str = "null";

  String *line = NULL;
  String *next = NULL;
  for (size_t i = 0; i < stack->size; i++) {
    if ((line = get(stack, i)) == NULL) {
      return;
    }
    fprintf(stderr, "LINE: %p -> %s\n", line, line ? c_str(line) : null_str);
    if (!equals(line, &cancelletto)) {
      continue;
    }

    if ((next = get(stack, ++i)) == NULL) {
      return;
    }
    fprintf(stderr, "NEXT: %p -> %s\n", next, next ? c_str(next) : null_str);
    if (!equals(next, &include)) {
      continue;
    }

    for (size_t j = i; j < stack->size; j++) {
      fprintf(stderr, "FOUND #include INIZIO INTERNAL LOOP\n");
      if ((next = get(stack, j)) == NULL) {
        return;
      }
      append(line, next);
      next->size = 0;
      if (equals(next, &gt)) {
        push(line, '\n');
        fprintf(stderr, "FOUND > EXITING INTERNAL LOOP\n");
        break;
      }
    }
  }
  fprintf(stderr, "EXITING EXTERNAL LOOP\n");
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
  merge_include_macros(&codeblocks);
  // {
  //   size_t i;
  //   i = 1;
  //   append(get(&codeblocks, 0), get(&codeblocks, i));
  //   get(&codeblocks, i)->size = 0;
  //   i = 2;
  //   append(get(&codeblocks, 0), get(&codeblocks, i));
  //   get(&codeblocks, i)->size = 0;
  //   i = 3;
  //   append(get(&codeblocks, 0), get(&codeblocks, i));
  //   get(&codeblocks, i)->size = 0;
  //   i = 4;
  //   append(get(&codeblocks, 0), get(&codeblocks, i));
  //   get(&codeblocks, i)->size = 0;
  //   i = 5;
  //   append(get(&codeblocks, 0), get(&codeblocks, i));
  //   get(&codeblocks, i)->size = 0;
  //   i = 6;
  //   append(get(&codeblocks, 0), get(&codeblocks, i));
  //   get(&codeblocks, i)->size = 0;
  // }
  for (size_t i = 0; i < codeblocks.size; i++) {
    String *line = &(codeblocks.data[i]);
    printf("%7zu : %s\n", i, c_str(line));
  }

  fclose(f);

  return 0;
}
