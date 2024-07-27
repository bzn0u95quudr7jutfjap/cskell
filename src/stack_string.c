#include "stack_string.h"
#include "stack.h"

DEFINE_STACK(String, Stack_String);

void Stack_String_free(Stack_String *stack) {
  if (stack == NULL) {
    return;
  }
  for (size_t i = 0; i < stack->size; i++) {
    String_free(at(stack, i));
  }
  stack->capacity = 0;
  stack->size = 0;
  free(stack->data);
  stack->data = NULL;
}
