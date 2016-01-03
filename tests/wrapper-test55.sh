#! /bin/bash

RET=$(./test55)
[ "$RET" = "0123456789" ] && exit 0 || exit 1
