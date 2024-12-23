#!/bin/sh

sh compile.sh && {
  ./build/src/tests/run_tests || exit 1
  find examples -type f -name '*.c' -! -name '*.output.c' | while IFS= read i ; do
    echo "=========================================================="
    echo "$i"
    echo "=========================================================="
    diff <(./build/cskell "$i") "$i.output.c" --color --side-by-side --suppress-common-lines || exit 1
  done
}
#sh compile.sh && ./build/cskell src/main.c a

echo "=========================================================="
