#! /bin/bash

STR=$(./test30 )
[ "$STR" = "hello" ] && exit 0 || exit 1
