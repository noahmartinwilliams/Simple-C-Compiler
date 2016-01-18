#! /bin/bash

./test11-less-than-test
RET=$?
[ "$RET" = "1" ] && exit 0 || exit 1
