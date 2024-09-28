#!/bin/bash

DIR="build/"

cmake -B "$DIR" && cmake --build "$DIR"
