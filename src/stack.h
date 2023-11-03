#include <stdio.h>
#include <stdlib.h>

#ifndef STACK_TEMPLATE
#define STACK_TEMPLATE

#define DECLARE_STACK(T, STACK)                                                                                        \
  typedef struct STACK STACK;                                                                                          \
  struct STACK {                                                                                                       \
    T *data;                                                                                                           \
    size_t capacity;                                                                                                   \
    size_t size;                                                                                                       \
    void (*const push)(STACK *, T);                                                                                    \
    void (*const pop)(STACK *);                                                                                        \
    T *(*const get)(STACK *, size_t);                                                                                  \
  };                                                                                                                   \
  void STACK##_push(STACK *, T);                                                                                       \
  void STACK##_pop(STACK *);                                                                                           \
  T *STACK##_get(STACK *, size_t);                                                                                     \
  extern const STACK New##STACK;

#define DEFINE_STACK(T, STACK)                                                                                         \
  const STACK New##STACK = {                                                                                           \
      .data = NULL, .capacity = 0, .size = 0, .push = STACK##_push, .pop = STACK##_pop, .get = STACK##_get};           \
                                                                                                                       \
  void STACK##_push(STACK *stack, T data) {                                                                            \
    if (stack->size >= stack->capacity) {                                                                              \
      stack->capacity = stack->size * 1.5 + 1;                                                                         \
      T *data = realloc(stack->data, stack->capacity * sizeof(T));                                                     \
      if (!data) {                                                                                                     \
        fprintf(stderr, "%s : %s\n", #STACK "_push", "realloc ha fallito");                                            \
        return;                                                                                                        \
      }                                                                                                                \
      stack->data = data;                                                                                              \
    }                                                                                                                  \
    memcpy(stack->data + stack->size++, &data, sizeof(T));                                                             \
  }                                                                                                                    \
                                                                                                                       \
  void STACK##_pop(STACK *stack) {                                                                                     \
    if (stack->size <= stack->capacity / 3) {                                                                          \
      stack->capacity /= 2;                                                                                            \
      T *data = realloc(stack->data, stack->capacity * sizeof(T));                                                     \
      if (!data) {                                                                                                     \
        fprintf(stderr, "%s : %s\n", #STACK "_pop", "realloc ha fallito");                                             \
        return;                                                                                                        \
      }                                                                                                                \
      stack->data = data;                                                                                              \
    }                                                                                                                  \
    stack->size--;                                                                                                     \
  }                                                                                                                    \
  T *STACK##_get(STACK *stack, size_t i) {                                                                             \
    if (i < stack->size) {                                                                                             \
      return &(stack->data[i]);                                                                                        \
    }                                                                                                                  \
    if (i >= -(stack->size)) {                                                                                         \
      return &(stack->data[i + stack->size]);                                                                          \
    }                                                                                                                  \
    return NULL;                                                                                                       \
  }

#define push(STACK_P, DATA)                                                                                            \
  ({                                                                                                                   \
    typeof(STACK_P) stack = STACK_P;                                                                                   \
    stack->push(stack, DATA);                                                                                          \
  })

#define pop(STACK_P)                                                                                                   \
  ({                                                                                                                   \
    typeof(STACK_P) stack = STACK_P;                                                                                   \
    stack->pop(stack);                                                                                                 \
  })

#define get(STACK_P, INDEX)                                                                                            \
  ({                                                                                                                   \
    typeof(STACK_P) stack = STACK_P;                                                                                   \
    stack->get(stack, INDEX);                                                                                          \
  })

#endif
