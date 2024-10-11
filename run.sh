#!/bin/sh

#sh compile.sh && ./build/run_tests
sh compile.sh && ./build/haskellinator src/main.c a
