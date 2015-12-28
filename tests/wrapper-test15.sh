#! /bin/bash

./test15
RET=$?
[ "$RET" = "50" ] && exit 0 || exit 1
