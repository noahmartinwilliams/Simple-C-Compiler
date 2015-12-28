#! /bin/bash

L=$(./test39 | wc -l)
[ "$L" = 99 ] && exit 0 || exit 1
