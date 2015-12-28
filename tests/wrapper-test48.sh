#! /bin/bash

TMP=$(mktemp)
TMP2=$(mktemp)
trap "rm $TMP $TMP2" EXIT

./test48 >$TMP
seq 0 98 > $TMP2

cmp $TMP $TMP2 && exit 0 || exit 1
