#! /bin/bash

TMP=$(mktemp)
trap "rm $TMP" EXIT
./test43-do-while-loop > $TMP

[ "$(cat $TMP | head -n 1)" = "here" ] || exit 1
[ "$(cat $TMP | wc -l)" = 100 ] || exit 2
