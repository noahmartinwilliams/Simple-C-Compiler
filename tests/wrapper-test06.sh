#! /bin/bash

./test06
RET="$?"

[ "$RET" = 30 ] && exit 0 || exit 1
