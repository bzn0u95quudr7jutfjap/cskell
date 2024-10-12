#ifndef STACK_TEMPLATE
#define STACK_TEMPLATE

#define declare_template_stack_internal(type, stack_type, new, free, resize, at, push, pop)                            \
  typedef struct stack_type stack_type;                                                                                \
  struct stack_type {                                                                                                  \
    size_t size;                                                                                                       \
    size_t capacity;                                                                                                   \
    type *data;                                                                                                        \
  };                                                                                                                   \
  stack_type new ();                                                                                                   \
  void free(stack_type *stack);                                                                                        \
  void resize(stack_type *stack, size_t capacity);                                                                     \
  type *at(stack_type *, size_t);                                                                                      \
  void push(stack_type *, type);                                                                                       \
  void pop(stack_type *);

#define define_template_stack_internal(type, stack_type, new, free, resize, at, push, pop)                             \
  stack_type new () { return (stack_type){.size = 0, .capacity = 0, .data = NULL}; }                                   \
                                                                                                                       \
  void resize(stack_type *stack, size_t capacity) {                                                                    \
    stack->data = (typeof(stack->data))realloc(stack->data, (stack->capacity = capacity) * sizeof(*(stack->data)));    \
  }                                                                                                                    \
                                                                                                                       \
  type *at(stack_type *stack, size_t i) {                                                                              \
    return (i < stack->size) ? &(stack->data[i]) : ((i >= -(stack->size)) ? &(stack->data[i + stack->size]) : NULL);   \
  }                                                                                                                    \
                                                                                                                       \
  void push(stack_type *stack, type data) {                                                                            \
    if (stack->size >= stack->capacity) {                                                                              \
      resize(stack, stack->size * 1.5 + 1);                                                                            \
    }                                                                                                                  \
    stack->data[stack->size++] = data;                                                                                 \
  }                                                                                                                    \
                                                                                                                       \
  void pop(stack_type *stack) {                                                                                        \
    if (stack->size <= stack->capacity / 3) {                                                                          \
      resize(stack, stack->capacity / 2);                                                                              \
    }                                                                                                                  \
    stack->size--;                                                                                                     \
  }

#define declare_template_stack_c(type, stack)                                                                          \
  declare_template_stack_internal(type, stack, new_##stack, free_##stack, resize_##stack, at_##stack, push_##stack,    \
                                  pop_##stack)

#define define_template_stack_c(type, stack)                                                                           \
  define_template_stack_internal(type, stack, new_##stack, free_##stack, resize_##stack, at_##stack, push_##stack,     \
                                 pop_##stack)

#define declare_template_stack_cpp(type, stack)                                                                        \
  declare_template_stack_internal(type, stack, new_##stack, free, resize, at, push, pop)

#define define_template_stack_cpp(type, stack)                                                                         \
  define_template_stack_internal(type, stack, new_##stack, free, resize, at, push, pop)

#ifndef __cplusplus

#define declare_template_stack declare_template_stack_c
#define define_template_stack define_template_stack_c

#else

#define declare_template_stack declare_template_stack_cpp
#define define_template_stack define_template_stack_cpp

#endif

#endif // STACK_TEMPLATE
