#!/bin/bash

DIR="build/"

CC=clang CXX=clang cmake -B "$DIR" && cmake --build "$DIR"
