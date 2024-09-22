#!/bin/bash

DIR="build/"

cmake . -B"$DIR" &&
make -C "$DIR" && {
  ./"$DIR"/run_tests
}
