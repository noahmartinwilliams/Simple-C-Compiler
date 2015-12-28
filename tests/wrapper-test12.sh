#! /bin/bash

./test12
RET=$?
[ "$RET" = "3" ] && exit 0 || exit 1
