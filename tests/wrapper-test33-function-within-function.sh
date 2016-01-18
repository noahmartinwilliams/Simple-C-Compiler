#! /bin/bash

RET=$(./test33-function-within-function)
[ "$RET" = "hello world" ] && exit 0 || exit 1
