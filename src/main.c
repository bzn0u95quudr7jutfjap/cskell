#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <stack.h>
#include <string_class.h>

u0 print_formatted_code(FILE * out, Formatter * fmt);

String file_get_contents(char *file) {
  String tmp = new_String();
  char c = EOF;
  FILE *stream = fopen(file, "r");
  if (stream == NULL) {
    return tmp;
  }
  while ((c = fgetc(stream)) != EOF) {
    push(&tmp, c);
  }
  fclose(stream);
  return tmp;
}

String string_view(Formatter *fmt, u32 i) {
  Token *t = at(&fmt->tokens, i);
  return (String){.data = fmt->str.data + t->begin, .size = t->size};
}

#if 1

int main(int argc, char *argv[]) {
  if (argc != 3) {
    printf("Uso invalido\n\n$ %s <INPUT FILE> <OUTPUT FILE>\n", argv[0]);
    return 1;
  }
  char *input = argv[1];
  char *output = argv[2];
  Formatter fmt = {};
  fmt.str = file_get_contents(input);
  tokenizer(&fmt);
  print_formatted_code(stdout, &fmt);
  free_Formatter(&fmt);
  return 0;
}

#else

void print_indentation(int level) {
  for (int i = 0; i < level; i++) {
    fprintf(stdout, "  ");
  }
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
  if (!(islower(c) || isupper(c) || c == '_' || c == '*' || c == '&')) {
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

void merge_level1_parentesi(Stack_String *stack, size_t i, size_t j, bool b) {
  if (i >= stack->size) {
    return;
  }

  String *line = at(stack, i);
  char *c = at(line, 0);

  if (line->size == 1 && *c == '(' || *c == '[') {
    return merge_level1_parentesi(stack, i + 1, i, true);
  }

  if (b && (*c == ')' || *c == ']')) {
    String *line = at(stack, j);
    move_into(line, at(stack, j + 1));
    for (size_t k = j + 2; k < i; k++) {
      String *next = at(stack, k);
      if (*at(next, 0) != ';' && *at(line, -1) != '!') {
        push(line, ' ');
      }
      move_into(line, next);
    }
    move_into(line, at(stack, i));
    return merge_level1_parentesi(stack, i + 1, i + 1, false);
  }

  return merge_level1_parentesi(stack, i + 1, j, b);
}

void merge_parentesi_identificatori(Stack_String *stack, size_t i) {
  if (i >= stack->size) {
    return;
  }

  String *line = at(stack, i);
  String *next = at(stack, i + 1);
  if (line == NULL || next == NULL) {
    return;
  }

  char *c = at(next, 0);
  if (c != NULL && *c == '[') {
    move_into(line, next);
    return merge_parentesi_identificatori(stack, i + 2);
  }

  if (is_possible_identifier(line) && next->size > 1 && c != NULL && *c == '(') {
    move_into(line, next);
    return merge_parentesi_identificatori(stack, i + 2);
  }

  return merge_parentesi_identificatori(stack, i + 1);
}

void merge_linee(Stack_String *stack, size_t i, size_t j) {
  if (i >= stack->size) {
    return;
  }

  String *line = at(stack, i);
  char c = *at(line, 0);

  if (c == '#' || c == '{' || c == '}' || c == '\\' || c == '/') {
    return merge_linee(stack, i + 1, i + 1);
  }

  if (c == ';') {
    line = at(stack, j);
    for (size_t k = j + 1; k < i; k++) {
      String *next = at(stack, k);
      push(line, ' ');
      move_into(line, next);
    }
    return merge_linee(stack, i + 1, i + 1);
  }

  return merge_linee(stack, i + 1, j);
}

void merge_inizioriga_istruzione(Stack_String *stack, size_t i) {
  if (i >= stack->size) {
    return;
  }

  String *sx = at(stack, i + 0);
  String *dx = at(stack, i + 1);

  if (sx == NULL || dx == NULL) {
    return;
  }

  char *c = at(sx, 0);
  char *d = at(dx, 0);
  if (((*c == '{' || *c == ';') && *d != '}') && (!is_any_of(*c, 2, "#/") && !is_any_of(*d, 2, "#/"))) {
    push(sx, ' ');
    move_into(sx, dx);
    return merge_inizioriga_istruzione(stack, i + 2);
  }

  return merge_inizioriga_istruzione(stack, i + 1);
}

void pad_braces(Stack_String *stack, size_t i, size_t j, bool closing, bool peso_a_destra) {
  if (i >= stack->size) {
    return;
  }

  String *line = at(stack, i);
  char *c = at(line, 0);
  if (c != NULL && *c == '{') {
    return pad_braces(stack, i + 1, i, true, peso_a_destra);
  }

  if (closing && c != NULL && *c == '}') {
    if (peso_a_destra) {
      line = at(stack, j - 1);
      push(line, ' ');
      size_t padding = line->size;
      move_into(line, at(stack, j));
      for (size_t k = j + 1; k <= i; k++) {
        String pad = NewString;
        for (size_t a = 0; a < padding; a++) {
          push(&pad, ' ');
        }
        line = at(stack, k);
        char c = *at(line, 0);
        // if (c != ';' && c != '}') {
        //   push(&pad, ';');
        //   if (!is_white(c)) {
        //     push(&pad, ' ');
        //   }
        // }
        move_into(&pad, line);
        free(line->data);
        line->data = pad.data;
        line->size = pad.size;
        line->capacity = pad.capacity;
      }
    } else {
      for (size_t k = j; k <= i; k++) {
        line = at(stack, k);
        String pad = from_cstr("  ");
        move_into(&pad, line);
        free(line->data);
        line->data = pad.data;
        line->size = pad.size;
        line->capacity = pad.capacity;
      }
    }
    return pad_braces(stack, i + 1, i + 1, false, peso_a_destra);
  }

  return pad_braces(stack, i + 1, j, closing, peso_a_destra);
}

int main(int argc, const char *argv[]) {
  if (argc == 1) {
    fprintf(stderr, "File da formattare non dato\n\nSINTASSI: %s <FILE> [--pesato-a-destra]\n\n", argv[0]);
    return 1;
  }

  FILE *f = fopen(argv[1], "r");
  if (!f) {
    fprintf(stderr, "File %s invalido\n\n", argv[1]);
    return 2;
  }

  bool pesato_a_destra = (argc == 3 && strcmp(argv[2], "--pesato-a-destra"));

  Stack_String codeblocks = parse_code_into_words(f);
  remove_empty_strings(&codeblocks);
  merge_include_macros_rec(&codeblocks, 0);
  remove_empty_strings(&codeblocks);
  merge_unary_operators(&codeblocks, 0);
  // ==============================================================
  remove_empty_strings(&codeblocks);
  for (size_t i = 0; i + 1 < codeblocks.size; i++) {
    String *line = at(&codeblocks, i + 1);
    if (*at(line, 0) == ',') {
      move_into(at(&codeblocks, i), line);
    }
  }
  remove_empty_strings(&codeblocks);
  // ==============================================================

  size_t len;
  do {
    len = codeblocks.size;
    merge_level1_parentesi(&codeblocks, 0, 0, false);
    merge_parentesi_identificatori(&codeblocks, 0);
    remove_empty_strings(&codeblocks);
  } while (len > codeblocks.size);

  remove_empty_strings(&codeblocks);
  merge_linee(&codeblocks, 0, 0);
  remove_empty_strings(&codeblocks);
  for (size_t i = 0; i + 1 < codeblocks.size; i++) {
    String *line = at(&codeblocks, i);
    String *next = at(&codeblocks, i + 1);
    if (is_possible_identifier(line) && *at(next, -1) == ')') {
      push(line, ' ');
      move_into(line, next);
      i++;
    }
  }
  remove_empty_strings(&codeblocks);
  merge_inizioriga_istruzione(&codeblocks, 0);

  remove_empty_strings(&codeblocks);
  size_t num_open_braces;
  do {
    pad_braces(&codeblocks, 0, 0, false, pesato_a_destra);
    remove_empty_strings(&codeblocks);
    num_open_braces = 0;
    for (size_t i = 0; i < codeblocks.size; i++) {
      String *line = at(&codeblocks, i);
      if (*at(line, 0) == '{') {
        num_open_braces++;
      }
    }
  } while (num_open_braces > 0);

  remove_empty_strings(&codeblocks);

  for (size_t i = 0; i < codeblocks.size; i++) {
    // printf("%7zu : %s\n", i, c_str(at(&codeblocks, i)));
    printf("%s\n", c_str(at(&codeblocks, i)));
  }

  fclose(f);

  return 0;
}

#endif
