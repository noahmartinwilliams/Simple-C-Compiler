#! /bin/bash

TMP=$(mktemp)
trap "rm $TMP" EXIT

cpp -I ./include source-test64-local-function-pointer.c >$TMP
./cc output.s $TMP $(./get-arc-name.sh)
as output.s -o output.o
gcc output.o -o test64-local-function-pointer
./test64-local-function-pointer

[ "$?" != 0 ] && exit 1

cpp -D DEBUG -I ./include source-test64-local-function-pointer.c >$TMP
ARC=$(./get-arc-name.sh)
./cc output.s $TMP $ARC 2>/dev/null

[ "$?" = 0 ] && exit 1

exit 0
