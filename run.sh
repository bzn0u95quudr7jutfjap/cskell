#!/bin/sh

sh compile.sh && {
  ./build/run_tests
  for i in $(ls -1 examples/*.c) ; do
    i="$(basename "$i")"
    echo "=========================================================="
    echo "$i"
    echo "=========================================================="
    diff <(./build/cskell examples/"$i") tests/"$i" --context=3
  done
}
#sh compile.sh && ./build/cskell src/main.c a
