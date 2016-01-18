#! /bin/bash

TMP=$(mktemp)
trap "rm $TMP" EXIT

./test47-var-args > $TMP
X=0
cat $TMP | while read LINE;
do
	if [ "$X" != "$LINE" ];
	then
		exit 1;
	fi
	X=$(($X + 1));
done

[ "$(cat $TMP | wc -l)" = 99 ] && exit 0 || exit 2
