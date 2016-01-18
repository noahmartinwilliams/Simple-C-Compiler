#! /bin/bash

./test12-sequence-of-scopes
RET=$?
[ "$RET" = "3" ] && exit 0 || exit 1
