#! /bin/bash

./test08-else-statements
RET=$?
[ "$RET" = "1" ] && exit 0 || exit 1
