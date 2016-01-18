#! /bin/bash

./test05-if-statements
RET="$?"
[ "$RET" = 1 ] && exit 0 || exit 1
