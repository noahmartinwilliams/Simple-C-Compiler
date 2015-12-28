#! /bin/bash

./test20
RET="$?"
[ "$RET" = 1 ] && exit 0 || exit 1
