#! /bin/bash

OUT=$(./test53)
[ "$OUT" = 0123456789 ] && exit 0 || exit 1
