#! /bin/bash

./test04-division
RET="$?"
[ "$RET" = 3 ] && exit 0 || exit 1
