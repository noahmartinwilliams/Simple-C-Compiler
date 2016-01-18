#! /bin/bash

RET=$(./test54-for-loops3)
[ "$RET" = "0123456789" ] && exit 0 || exit 1
