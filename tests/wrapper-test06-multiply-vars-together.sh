#! /bin/bash

./test06-multiply-vars-together
RET="$?"

[ "$RET" = 30 ] && exit 0 || exit 1
