#! /bin/bash

./test02-declare-variable
RET="$?"
[ "$RET" = 2 ] && exit 0 || exit 1
