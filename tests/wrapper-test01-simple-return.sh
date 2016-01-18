#! /bin/bash

./test01-simple-return
RET=$?
[ "$RET" = "1" ] && exit 0 || exit 1
