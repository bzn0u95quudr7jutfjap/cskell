#!/bin/sh

cd "$(dirname "$0")"

test_tokenizer(){
  mkdir -p build/tests/

  clang -o build/tests/test_tokenizer \
    -I inc -include cskell.h -include stdbool.h -include int.h \
    src/{io.c,token.c,test_tokenizer.c}

  ./build/tests/test_tokenizer
}

run_test(){
  echo "$1"
  diff  --side-by-side --color <(test_tokenizer <<< "$2") <(cat <<< "$3")
  printf "\n%d\n\n" $?
}

run_test "token_prova" "$(cat << EOF
EOF
)" "$(cat << EOF
EOF
)"

run_test "token_assegnazione" "$(cat << EOF
int a = 12;
EOF
)" "$(cat << EOF
word,int
word,a
operator_binary,=
number,12
end_of_line,;
EOF
)"

run_test "token_variabile_strana" "$(cat << EOF
int _a1_bc3;
EOF
)" "$(cat << EOF
word,int
word,_a1_bc3
end_of_line,;
EOF
)"

run_test "token_tipo_strano" "$(cat << EOF
_uint32_t a;
EOF
)" "$(cat << EOF
word,_uint32_t
word,a
end_of_line,;
EOF
)"

