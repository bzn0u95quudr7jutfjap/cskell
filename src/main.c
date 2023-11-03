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
    String *line = stack->get(stack, i);
    if (line->size > 0) {
      push(&filtered, NewString);
      move_into(stack->get(&filtered, -1), line);
    }
  }
  free(stack->data);
  stack->data = filtered.data;
  stack->size = filtered.size;
  stack->capacity = filtered.capacity;
}

void merge_include_macros(Stack_String *stack) {
  String cancelletto = from_cstr("#");
  String include = from_cstr("include");
  String lt = from_cstr("<");
  String gt = from_cstr(">");
  char *null_str = "null";

  String *ca = NULL;
  String *in = NULL;
  String *le = NULL;
  for (size_t i = 0; i < stack->size; i++) {
    ca = stack->get(stack, i);
    in = stack->get(stack, i + 1);
    le = stack->get(stack, i + 2);
    if (ca == NULL || in == NULL || le == NULL) {
      break;
    }
    if (!(equals(ca, &cancelletto) && equals(in, &include))) {
      continue;
    }

    if (le->data[0] == '"') {
      move_into(ca, in);
      push(ca, ' ');
      move_into(ca, le);
      i += 3;
      continue;
    }

    if (!equals(le, &lt)) {
      continue;
    }

    move_into(ca, in);
    push(ca, ' ');
    move_into(ca, le);
    i += 3;
    for (String *str = stack->get(stack, i); str != NULL && !equals(str, &gt) && i < stack->size; i++) {
      move_into(ca, str);
      str = stack->get(stack, i + 1);
    }
    if (stack->get(stack, i) == NULL) {
      break;
    }
    move_into(ca, stack->get(stack, i));
  }
}

void merge_include_macros_rec(Stack_String *stack, size_t i) {
  static void (*this)(Stack_String *, size_t) = merge_include_macros_rec;
  if (i >= stack->size) {
    return;
  }

  String *cancelletto = stack->get(stack, i);
  String *includi = stack->get(stack, i + 1);
  String *resto = stack->get(stack, i + 2);

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
    resto = stack->get(stack, i + j);
    for (; resto != NULL && resto->size > 0 && resto->data[0] != '>'; j++, resto = stack->get(stack, i + j)) {
      move_into(cancelletto, resto);
    }
    if (resto != NULL && resto->size > 0 && resto->data[0] == '>') {
      move_into(cancelletto, resto);
    }
    return this(stack, i + j + 1);
  }

  return this(stack, i + 1);
}

void merge_parenthesis(Stack_String *stack) {
  String p_open = from_cstr("(");
  String p_closed = from_cstr(")");
  String coma = from_cstr(",");
  String *p = NULL;
  String *str = NULL;
  for (size_t i = 0; i < stack->size; i++) {
    p = stack->get(stack, i);
    if (p == NULL) {
      return;
    }
    if (p->size == 0 || !equals(p, &p_open)) {
      continue;
    }

    for (str = stack->get(stack, ++i); str != NULL && !equals(str, &p_closed) && i < stack->size; i++) {
      if (equals(str, &coma)) {
        pop(p);
      }
      move_into(p, str);
      push(p, ' ');
      str = stack->get(stack, i + 1);
    }
    if (stack->get(stack, i) == NULL) {
      break;
    }
    pop(p);
    move_into(p, stack->get(stack, i));
  }
}

void merge_parenthesis_rec(Stack_String *stack, String *str, size_t i, size_t level) {
  static void (*const this)(Stack_String *, String *, size_t, size_t) = merge_parenthesis_rec;
  if (i >= stack->size) {
    return;
  }

  String *line = stack->get(stack, i);
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
  // merge_include_macros(&codeblocks);
  merge_include_macros_rec(&codeblocks, 0);
  // merge_parenthesis(&codeblocks);
  merge_parenthesis_rec(&codeblocks, NULL, 0, 0);
  remove_empty_strings(&codeblocks);
  for (size_t i = 0; i < codeblocks.size; i++) {
    printf("%7zu : %s\n", i, c_str((&codeblocks)->get(&codeblocks, i)));
  }

  fclose(f);

  return 0;
}
