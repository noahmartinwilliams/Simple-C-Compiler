#! /bin/bash

X=0;
Y=$(./test38-for-loop | while read LINE;
do
	X=$(($X + 1))
	echo "$X"
done  | tail -n 1 )

[ "$Y" = "99" ] && exit 0 || exit 2
