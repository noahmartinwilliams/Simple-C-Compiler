#! /bin/bash

NAME=$(./get-arc-name.sh)
OUT=$(./cc output.s source-test79-constant-assign.c $NAME 2>&1)
RET="$?"
ERROR='error: can not assign to constant.'
ERROR=$(echo "$ERROR" | sed 's/ /[[:space:]]*/g')
if echo $OUT | grep "$ERROR" 2>/dev/null >/dev/null;
then
	exit 0;
else
	exit 1;
fi
