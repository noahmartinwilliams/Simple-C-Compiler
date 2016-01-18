#! /bin/bash

./test09-scope
RET=$?
[ "$RET" = "1" ] && exit 0 || exit 1
