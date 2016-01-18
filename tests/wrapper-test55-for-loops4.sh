#! /bin/bash

RET=$(./test55-for-loops4)
[ "$RET" = "0123456789" ] && exit 0 || exit 1
