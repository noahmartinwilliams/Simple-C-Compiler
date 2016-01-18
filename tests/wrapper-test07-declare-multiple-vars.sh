#! /bin/bash

./test07-declare-multiple-vars
RET="$?"
[ "$RET" = 3 ] && exit 0 || exit 1
