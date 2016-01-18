#! /bin/bash

./test03-addition-and-subtraction
RET="$?"
[ "$RET" = "2" ] && exit 0 || exit 1
