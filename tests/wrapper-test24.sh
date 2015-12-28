#! /bin/bash

./test24
RET=$?
[ "$RET" = "5" ] && exit 0 || exit 1
