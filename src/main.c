#include <ctype.h>
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

#define DEFINE_CSET(NAME, CSET)                                                                                        \
  bool NAME(char c) {                                                                                                  \
    static const char *const charset = CSET;                                                                           \
    return is_any_of(c, strlen(charset), charset);                                                                     \
  }

DEFINE_CSET(is_white, " \n\t")
DEFINE_CSET(is_special, "{}()[]#.;,")
DEFINE_CSET(is_string_delimiter, "'\"")
DEFINE_CSET(is_operator, "+-*/%!=&|^><")

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

    if (is_special(c)) {
      push(&code, NewString);
      String *line = &(code.data[code.size - 1]);
      push(line, c);
      push(&code, NewString);
      continue;
    }

    if (is_operator(c)) {
      push(&code, NewString);
      String *line = &(code.data[code.size - 1]);
      push(line, c);
      if (is_operator(fpeekc(stream))) {
        push(line, fgetc(stream));
      }
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

// . -> & *

bool is_possible_identifier(String *line) {
  if (line->size == 0) {
    return false;
  }
  char c = *at(line, 0);
  if (!(islower(c) || isupper(c) || c == '_')) {
    return false;
  }
  for (size_t i = 1; i < line->size; i++) {
    c = *at(line, i);
    if (!(islower(c) || isdigit(c) || isupper(c) || c == '_')) {
      return false;
    }
  }
  return true;
}

bool is_number(String *line) {
  if (line->size == 0) {
    return false;
  }
  if (line->size == 1) {
    return isdigit(*at(line, 0));
  }
  char c = *at(line, 0), d = *at(line, 1);
  if (c == '0' && (d == 'x' || d == 'X')) {
    for (size_t i = 2; i < line->size; i++) {
      if (!isxdigit(*at(line, i))) {
        return false;
      }
    }
    return true;
  }

  if (c == '0' && (d == 'b' || d == 'B')) {
    for (size_t i = 2; i < line->size; i++) {
      if (!isdigit(*at(line, i))) {
        return false;
      }
    }
    return true;
  }

  for (size_t i = 0; i < line->size; i++) {
    if (!isdigit(*at(line, i))) {
      return false;
    }
  }
  return true;
}

void merge_unary_operators(Stack_String *stack, size_t i) {
  String arrow = from_cstr("->");
  String dot = from_cstr(".");
  String not = from_cstr("!");
  String address = from_cstr("&");
  String deref = from_cstr("*");
  String increase = from_cstr("++");
  String decrease = from_cstr("--");

  String *left = at(stack, i);
  String *centre = at(stack, i + 1);
  String *right = at(stack, i + 2);
  if (left == NULL || centre == NULL || right == NULL) {
    return;
  }

  // if (equals(left, &not )) {
  //   move_into(left, centre);
  //   return merge_unary_operators(stack, i + 2);
  // }

  if (equals(centre, &increase) || equals(centre, &decrease)) {
    if (is_possible_identifier(left) && !is_possible_identifier(right)) {
      move_into(left, centre);
      return merge_unary_operators(stack, i + 2);
    }
    if (!is_possible_identifier(left) && is_possible_identifier(right)) {
      move_into(centre, right);
      return merge_unary_operators(stack, i + 2);
    }
    return merge_unary_operators(stack, i + 1);
  }

  if ((equals(centre, &deref) || equals(centre, &address)) && !is_possible_identifier(left) &&
      is_possible_identifier(right)) {
    move_into(centre, right);
    return merge_unary_operators(stack, i + 2);
  }

  if (equals(centre, &arrow)) {
    move_into(left, centre);
    move_into(left, right);
    return merge_unary_operators(stack, i + 3);
  }

  if (equals(centre, &dot) &&
      ((is_number(left) && is_number(right)) || (is_possible_identifier(left) && is_possible_identifier(right)))) {
    move_into(left, centre);
    move_into(left, right);
    return merge_unary_operators(stack, i + 3);
  }

  return merge_unary_operators(stack, i + 1);
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

  if (*at(resto, 0) == '"') {
    move_into(cancelletto, includi);
    push(cancelletto, ' ');
    move_into(cancelletto, resto);
    return this(stack, i + 3);
  }

  if (*at(resto, 0) == '<') {
    move_into(cancelletto, includi);
    push(cancelletto, ' ');
    move_into(cancelletto, resto);
    size_t j = 3;
    resto = at(stack, i + j);
    for (; resto != NULL && resto->size > 0 && *at(resto, 0) != '>'; j++, resto = at(stack, i + j)) {
      move_into(cancelletto, resto);
    }
    if (resto != NULL && resto->size > 0 && *at(resto, 0) == '>') {
      move_into(cancelletto, resto);
    }
    return this(stack, i + j + 1);
  }

  return this(stack, i + 1);
}

void merge_parenthesis_adjacent_rec(Stack_String *stack, String *line, size_t i, size_t j) {
  static void (*const this)(Stack_String *stack, String *line, size_t i, size_t j) = merge_parenthesis_adjacent_rec;
  if (i >= j) {
    return;
  }

  String *next = at(stack, i + 1);

  if (line == NULL || next == NULL) {
    return;
  }

  move_into(line, next);
  return this(stack, line, i + 1, j);
}

size_t find_parentesi_chiusa(Stack_String *stack, size_t i, size_t j) {
  if (j >= stack->size) {
    return j;
  }
  String *line = at(stack, j);
  if (line == NULL) {
    return j;
  }

  if (line->size > 0 && *at(line, 0) == ')') {
    merge_parenthesis_adjacent_rec(stack, at(stack, i), i, j);
  }

  return find_parentesi_chiusa(stack, i, j + 1);
}

void find_parentesi_aperta(Stack_String *stack, size_t i) {
  if (i >= stack->size) {
    return;
  }
  String *line = at(stack, i);
  if (line == NULL) {
    return;
  }

  if (line->size > 0 && *at(line, 0) == '(') {
    return find_parentesi_aperta(stack, find_parentesi_chiusa(stack, i, i + 1));
  }

  return find_parentesi_aperta(stack, i + 1);
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
  merge_unary_operators(&codeblocks, 0);
  find_parentesi_aperta(&codeblocks, 0);
  remove_empty_strings(&codeblocks);
  for (size_t i = 0; i < codeblocks.size; i++) {
    printf("%7zu : %s\n", i, c_str(at(&codeblocks, i)));
  }

  fclose(f);

  return 0;
}
