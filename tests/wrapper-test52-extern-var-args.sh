#! /bin/bash

RET=$(./test52-extern-var-args)
[ "$RET" = "0123456789" ] && exit 0 || exit 1
