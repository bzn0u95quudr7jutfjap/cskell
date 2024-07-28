#include "stack_string.h"
#include "stack.h"

define_template_stack_type(String, Stack_String);

void free_Stack_String(Stack_String *stack) {
  if (stack == NULL) {
    return;
  }
  for (size_t i = 0; i < stack->size; i++) {
    free_String(at_Stack_String(stack, i));
  }
  stack->capacity = 0;
  stack->size = 0;
  free(stack->data);
  stack->data = NULL;
}
