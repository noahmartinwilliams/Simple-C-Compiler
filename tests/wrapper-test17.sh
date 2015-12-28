#! /bin/bash

./test17
RET="$?"
[ "$RET" = 100 ] && exit 0 || exit 1
