#! /bin/bash

./test10-equality-test
RET=$?
[ "$RET" = "1" ] && exit 0 || exit 1
