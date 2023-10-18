#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAKE_STACK(NAME, T, push, pop, peek)                                   \
  typedef struct NAME NAME;                                                    \
  struct NAME {                                                                \
    T *data;                                                                   \
    size_t capacity;                                                           \
    size_t size;                                                               \
  };                                                                           \
  void push(NAME *s, T o) {                                                    \
    if (s->size == s->capacity) {                                              \
      T *newdata = realloc(s->data, sizeof(T) * (s->capacity * 1.5 + 1));      \
      if (newdata) {                                                           \
        s->data = newdata;                                                     \
        s->capacity = s->capacity * 1.5 + 1;                                   \
      }                                                                        \
    }                                                                          \
    s->data[s->size++] = o;                                                    \
  }                                                                            \
  void pop(NAME *s) {                                                          \
    if (s->size-- <= (s->capacity / 3)) {                                      \
      T *newdata = realloc(s->data, sizeof(T) * (s->capacity / 2));            \
      if (newdata) {                                                           \
        s->data = newdata;                                                     \
        s->capacity = s->capacity / 2;                                         \
      }                                                                        \
    }                                                                          \
  }                                                                            \
  T *peek(NAME *s) { return &(s->data[s->size - 1]); }                         \
                                                                               \
  const NAME New##NAME = {.data = NULL, .capacity = 0, .size = 0}

int main(int argc, const char *argv[]) {
  if (argc == 1) {
    fprintf(stderr, "File da formattare non dato\n\nSINTASSI: %s <FILE>\n\n",
            argv[0]);
    return 1;
  }

  FILE *f = fopen(argv[1], "r");
  if (!f) {
    fprintf(stderr, "File %s invalido\n\n", argv[1]);
    return 2;
  }

  Stack words = NewStack;

  char c = 0;
  int skip_endline = 0;
  int indentation_level = 0;
  while ((c = fgetc(f)) != EOF) {
    if (c == '{') {
      printf("\n");
      char c1 = fgetc(f);
      if (c1 == '\n') {
        for (int i = 0; i < indentation_level; i++) {
          printf(" ");
        }
      }
      printf("%c  ", c);
      indentation_level += 1;
    } else if (c == '}') {
      printf("\n");
      char c1 = fgetc(f);
      if (c1 == '\n') {
        for (int i = 0; i < indentation_level; i++) {
          printf(" ");
        }
      }
      printf("%c  ", c);
      indentation_level -= 1;
    } else if (c == ';') {
      printf("\n");
      char c1 = fgetc(f);
      if (c1 == '\n') {
        for (int i = 0; i < indentation_level; i++) {
          printf(" ");
        }
      }
      printf("%c  ", c);
    } else {
      printf("%c", c);
    }
  }

  fclose(f);

  return 0;
}
