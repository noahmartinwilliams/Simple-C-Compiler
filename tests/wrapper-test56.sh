#! /bin/bash

RET=$(./test56)
[ "$RET" = "0123456789" ] && exit 0 || exit 1
