#include <bits/pthreadtypes.h>
#include <bits/types/sigevent_t.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

bool is_any_of(char c, size_t size, char cs[]) {
  for (size_t i = 0; i < size; i++) {
    if (c == cs[i]) {
      return true;
    }
  }
  return false;
}

bool is_white(char c) { return is_any_of(c, 3, "\n \t"); }

bool is_operatore(char c) { return is_any_of(c, 10, "+-*/<>&|=!"); }

void consume_while_white(FILE *stream) {
  char c;
  size_t file_len = fsize(stream);
  for (size_t i = ftell(stream); is_white(c = fgetc(stream)) && i < file_len; i++) {
  }
  fseek(stream, -1, SEEK_CUR);
}

void print_operatore(char c, FILE *stream, bool comma) {
  char next = fpeekc(stream);
  if (is_operatore(next)) {
    if (c == next) {
      if (c == '+' || c == '-') {
        if (!comma && !is_white(fpeekbackc(stream))) {
          fprintf(stdout, "%c%c ", c, fgetc(stream));
        } else {
          fprintf(stdout, " %c%c", c, fgetc(stream));
        }
      } else {
        fprintf(stdout, " %c%c ", c, fgetc(stream));
      }
    } else {
      fprintf(stdout, " %c%c ", c, fgetc(stream));
    }
  } else {
    if (comma) {
      fprintf(stdout, "%c ", c);
    } else {
      fprintf(stdout, " %c ", c);
    }
  }
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
  bool comma = false;
  for (size_t i = ftell(stream); (c = fgetc(stream)) != ')' && i < file_len; i++) {
    if (c == '(') {
      format_parenthesis(stream);
    } else if (c == '"' || c == '\'') {
      print_inside_quote(stream, c);
    } else if (is_operatore(c)) {
      print_operatore(c, stream, comma);
    } else if (is_white(c)) {
      consume_while_white(stream);
      continue; // skip comma = false;
    } else if (c == ',') {
      fprintf(stdout, ", ");
      comma = true;
      continue; // skip comma = false;
    } else if (is_white(c)) {
      consume_while_white(stream);
      fprintf(stdout, " ");
    } else {
      fprintf(stdout, "%c", c);
    }
    comma = false;
  }
  fprintf(stdout, ")");
}

// RECURSION HELL

bool is_macro(char, FILE*);
bool is_comment_line(char, FILE*);
bool is_comment_multiline(char, FILE*);
void parse_comment_multiline(char, FILE*, size_t);
void parse_comment_line(char, FILE*, size_t);
void parse_macro(char, FILE*, size_t);

typedef struct pattern_match pattern_match;
struct pattern_match {
  bool (*const condition)(char, FILE *);
  void (*const consequence)(char, FILE *,size_t);
};

pattern_match next(char c, FILE * stream){
  size_t len = 3;
  pattern_match cases[] ={
    {.condition = is_macro, .consequence = parse_macro},
    {.condition = is_comment_line, .consequence = parse_comment_line},
    {.condition = is_comment_multiline, .consequence = parse_comment_multiline},
  };

  size_t i = 0;
  for(i = 0; i < len; i++){
    if(cases[i].condition(c,stream)){
      break;
    }
  }

  fprintf(stderr, "NEXT: indice selezionato %zu su %zu\n", i, len);
  
  return cases[i];
}

bool is_comment_line(char c, FILE * stream){
  return c == '/' && fpeekc(stream) == '/';
}
void parse_comment_line(char c, FILE *f, size_t indentation_level) {
  fprintf(stdout, "%c", c);
  while ((c = fgetc(f)) != '\n') {
    fprintf(stdout, "%c", c);
  }
  consume_while_white(f);
  fprintf(stdout, "\n");
  print_indentation(indentation_level);
}

bool is_comment_multiline(char c, FILE * stream){
  return c == '/' && fpeekc(stream) == '*';
}
void parse_comment_multiline(char c, FILE *f, size_t indentation_level) {
  fprintf(stdout, "\n");
  print_indentation(indentation_level);
  fprintf(stdout, "%c%c ", c, fgetc(f));
  consume_while_white(f);
  while (!((c = fgetc(f)) == '*' && fpeekc(f) == '/')) {
    fprintf(stdout, "%c", c);
  }
  fgetc(f);
  consume_while_white(f);
  fprintf(stdout, "\n");
  print_indentation(indentation_level);
  fprintf(stdout, "*/\n");
  print_indentation(indentation_level);

  next(c,f).consequence(c,f,indentation_level);
}

bool is_macro(char c, FILE * stream){
  return c == '#';
}
void parse_macro(char c, FILE *f, size_t indentation_level) {
  fprintf(stdout, "#");
  while (!((c = fgetc(f)) == '\\' && fpeekc(f) == '\n') && c != '\n') {
    fprintf(stdout, "%c", c);
  }
  consume_while_white(f);
  fprintf(stdout, "\n");
  print_indentation(indentation_level);

  next(c,f).consequence(c,f,indentation_level);
}

void parse_brace_closed() {}

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
  bool macro = false, commento_monolinea = false, commento_multilinea = false;
  for (int i = 0; (c = fgetc(f)) != EOF && i < file_len + 12; i++) {

    if (c == '/' && fpeekc(f) == '/') {
      parse_comment_line(c, f, indentation_level);
    } else if (c == '/' && fpeekc(f) == '*') {
      parse_comment_multiline(c, f, indentation_level);
    } else if (c == '#') {
      parse_macro(c, f, indentation_level);
    } else if (c == '"' || c == '\'') {
      print_inside_quote(f, c);
    } else if (is_operatore(c)) {
      print_operatore(c, f, false);
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
      consume_while_white(f);
      fprintf(stdout, "\n");
      print_indentation(indentation_level);
      fprintf(stdout, "%c ", c);
    } else if (c == '\n') {
      if (macro) {
        if (fpeekbackc(f) != '\\') {
          macro = false;
        }
        fprintf(stdout, "%c", c);
      }
    } else {
      fprintf(stdout, "%c", c);
    }
  }

  fclose(f);

  return 0;
}
