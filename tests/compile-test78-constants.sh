#! /bin/bash

TMP=$(mktemp)
trap "rm $TMP" EXIT
cpp -I ./include source-test78-constants.c >$TMP
./cc test78-constants-output.s $TMP $(./get-arc-name.sh)
as test78-constants-output.s -o output.o
gcc output.o -o test78-constants
./test78-constants
[ "$?" != 0 ] && exit 1;

cpp -DDEBUG=1 -I ./include source-test78-constants.c >$TMP
ARC=$(./get-arc-name.sh)
./cc test78-constants-output.s $TMP $ARC 2>/dev/null
[ "$?" = 0 ] && exit 1;

exit 0;
