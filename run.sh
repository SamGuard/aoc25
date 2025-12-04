#!/bin/bash

set -e

day=$1
in=$2

# Make the bin
gcc -o "bin/$day" "./$day.c" -Wall -Wextra -lm -g

./bin/"$day" "$in"