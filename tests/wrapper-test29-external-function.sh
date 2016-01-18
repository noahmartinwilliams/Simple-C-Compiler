#! /bin/bash

RET=$(./test29-external-function)
[ "$RET" = "hello" ] && exit 0 || exit 1

