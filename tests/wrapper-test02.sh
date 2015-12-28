#! /bin/bash

./test02
RET="$?"
[ "$RET" = 2 ] && exit 0 || exit 1
