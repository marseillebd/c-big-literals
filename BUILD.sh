#!/bin/sh
set -e

here="$(dirname "$(realpath "$0")")"
cd "$here"

mkExe='gcc'
mkObj='gcc -c'
flags_language='-std=c11 -pedantic -Wall -Wimplicit-fallthrough -Werror'
flags_optimize='-O2'
flags_link=''
flags_include='-I src'

memcheck=''
# memcheck='valgrind'
# memcheck='valgrind --leak-check=full'

difftool='diff'
if command -v icdiff 2>&1 >/dev/null; then difftool=icdiff; fi

# core translation units
mkdir -p build/core
$mkObj $flags_language $flags_optimize $flags_include \
  -o build/core/bn.o \
  src/core/bn.c $flags_link

$mkObj $flags_language $flags_optimize $flags_include \
  -o build/core/bz.o \
  src/core/bz.c $flags_link

# basic translation units
mkdir -p build/basic
$mkObj $flags_language $flags_optimize $flags_include \
  -o build/basic/bn.o \
  src/basic/bn.c $flags_link

mkdir -p build/basic
$mkObj $flags_language $flags_optimize $flags_include \
  -o build/basic/bz.o \
  src/basic/bz.c $flags_link

# tests
$mkExe $flags_language $flags_include \
  -o test/run_bn_ \
  test/test_bn_.c build/core/bn.o
$memcheck ./test/run_bn_ > test/actual_bn_.txt
if ! diff -q test/expected_bn_.txt test/actual_bn_.txt; then
  $difftool test/expected_bn_.txt test/actual_bn_.txt
  exit 1
fi

$mkExe $flags_language $flags_include \
  -o test/run_bz \
  test/test_bz.c build/core/bn.o build/basic/bn.o build/basic/bz.o
$memcheck ./test/run_bz > test/actual_bz.txt
if ! diff -q test/expected_bz.txt test/actual_bz.txt; then
  $difftool test/expected_bz.txt test/actual_bz.txt
  exit 1
fi
