#! /bin/bash

NAME=$(echo "$2" | sed 's/source-//g')
if [ ! -f "compile-$NAME.sh" ] ;
then
	set -e
	TMP=$(mktemp)
	trap "rm $TMP" EXIT
	cpp -I ./include $1 >$TMP
	./cc $2-output.s $TMP $(./get-arc-name.sh)
	as $2-output.s -o output.o
	gcc output.o -o $2
else
	set -e
	./compile-$NAME.sh
fi
