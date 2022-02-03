#!/bin/sh
set -e

here="$(dirname "$(realpath "$0")")"
cd "$here"

mkExe='gcc'
mkObj='gcc -c'
flags_language='-std=c11 -pedantic -Wall -Werror'
flags_optimize='-O2'
flags_link=''
flags_include='-I src'

memcheck=''
# memcheck='valgrind'

difftool='diff'
if command -v icdiff 2>&1 >/dev/null; then difftool=icdiff; fi

mkdir -p build/core
$mkObj $flags_language $flags_optimize $flags_include \
  -o build/core/bn.o \
  src/core/bn.c $flags_link
mkdir -p build/basic
$mkObj $flags_language $flags_optimize $flags_include \
  -o build/basic/bn.o \
  src/basic/bn.c $flags_link

$mkExe $flags_language $flags_include \
  -o test/run \
  test/main.c build/core/bn.o
$memcheck ./test/run > test/actual.txt
if ! diff -q test/expected.txt test/actual.txt; then
  $difftool test/expected.txt test/actual.txt
  exit 1
fi
