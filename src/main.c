#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
  if (!(argc == 2 || argc == 3)) {
    printf("Uso invalido\n\n$ %s <INPUT_FILE> [OUTPUT_FILE]\n\n"
           "Se OUTPUT_FILE non è specificato viene usato stdout\n\n",
           argv[0]);
    return 1;
  }
  if (argc < 2) {
    exit(EXIT_FAILURE);
  }
  CodeTokens codetokens = {};
  filegetcontents(argv[1], &codetokens);
  tokenizer(&codetokens);
  formatter(&codetokens);
  if (argc < 3) {
    fileputcontents_fd(stdout, &codetokens);
  } else {
    fileputcontents(argv[2], &codetokens);
  }
  return 0;
}
