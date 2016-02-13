#! /bin/bash

TMP=$(mktemp)
trap "rm $TMP" EXIT
cpp -I ./include source-test76-preprocessor.c >$TMP
OUT=$(./cc test76-preprocessor-output.s $TMP $(./get-arc-name.sh) 2>&1)
( echo $OUT | grep '.*./include/test_header.h:5:.*' >/dev/null ) && exit 0;
exit 1;
