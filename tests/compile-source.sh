#! /bin/bash

NAME=$(echo "$2" | sed 's/source-//g')
if [ ! -f "compile-$NAME.sh" ] ;
then
	set -e
	./cc $2-output.s $1 ../phase-4-backend/libx64-backend.so
	as $2-output.s -o output.o
	gcc output.o -o $2
else
	set -e
	./compile-$NAME.sh
fi
