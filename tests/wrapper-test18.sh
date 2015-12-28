#! /bin/bash

./test18
RET="$?"
[ "$RET" = 2 ] && exit 0 || exit 1
