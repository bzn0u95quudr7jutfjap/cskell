#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "stack.h"
#include "stack_string.h"
#include "string_class.h"

char fpeekc(FILE *stream) {
  char c = fgetc(stream);
  fseek(stream, -1, SEEK_CUR);
  return c;
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

void remove_empty_strings(Stack_String *stack) {
  Stack_String filtered = NewStack_String;
  for (size_t i = 0; i < stack->size; i++) {
    String *line = at(stack, i);
    // String *line = ({
    //   typeof(stack) istack = stack;
    //   istack->get(istack, i);
    // });
    if (line->size > 0) {
      push(&filtered, NewString);
      move_into(at(&filtered, -1), line);
    }
  }
  free(stack->data);
  stack->data = filtered.data;
  stack->size = filtered.size;
  stack->capacity = filtered.capacity;
}

void merge_include_macros_rec(Stack_String *stack, size_t i) {
  static void (*this)(Stack_String *, size_t) = merge_include_macros_rec;
  if (i >= stack->size) {
    return;
  }

  String *cancelletto = at(stack, i);
  String *includi = at(stack, i + 1);
  String *resto = at(stack, i + 2);

  if (cancelletto == NULL || includi == NULL || resto == NULL) {
    return this(stack, i + 1);
  }

  bool is_inclusione = strcmp(c_str(cancelletto), "#") == 0 && strcmp(c_str(includi), "include") == 0;
  if (!is_inclusione) {
    return this(stack, i + 1);
  }

  if (resto->data[0] == '"') {
    move_into(cancelletto, includi);
    push(cancelletto, ' ');
    move_into(cancelletto, resto);
    return this(stack, i + 3);
  }

  if (resto->data[0] == '<') {
    move_into(cancelletto, includi);
    push(cancelletto, ' ');
    move_into(cancelletto, resto);
    size_t j = 3;
    resto = at(stack, i + j);
    for (; resto != NULL && resto->size > 0 && resto->data[0] != '>'; j++, resto = at(stack, i + j)) {
      move_into(cancelletto, resto);
    }
    if (resto != NULL && resto->size > 0 && resto->data[0] == '>') {
      move_into(cancelletto, resto);
    }
    return this(stack, i + j + 1);
  }

  return this(stack, i + 1);
}

void merge_parenthesis_rec(Stack_String *stack, String *str, size_t i, size_t level) {
  static void (*const this)(Stack_String *, String *, size_t, size_t) = merge_parenthesis_rec;
  if (i >= stack->size) {
    return;
  }

  String *line = at(stack, i);
  if (line == NULL) {
    return;
  }

  if (strcmp(c_str(line), "(") == 0) {
    if (str == NULL) {
      return this(stack, line, i + 1, level + 1);
    }
    pop(str);
    move_into(str, line);
    return this(stack, str, i + 1, level + 1);
  }

  if (strcmp(c_str(line), ")") == 0) {
    pop(str);
    move_into(str, line);
    if (level - 1 == 0) {
      return this(stack, NULL, i + 1, 0);
    }
    return this(stack, str, i + 1, level - 1);
  }

  if (strcmp(c_str(line), ",") == 0) {
    pop(str);
    move_into(str, line);
    push(str, ' ');
    return this(stack, str, i + 1, level);
  }

  if (level == 0) {
    return this(stack, str, i + 1, level);
  }

  move_into(str, line);
  push(str, ' ');
  return this(stack, str, i + 1, level);
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

  Stack_String codeblocks = parse_code_into_words(f);
  remove_empty_strings(&codeblocks);
  merge_include_macros_rec(&codeblocks, 0);
  merge_parenthesis_rec(&codeblocks, NULL, 0, 0);
  remove_empty_strings(&codeblocks);
  for (size_t i = 0; i < codeblocks.size; i++) {
    printf("%7zu : %s\n", i, c_str(at(&codeblocks, i)));
  }

  fclose(f);

  return 0;
}
