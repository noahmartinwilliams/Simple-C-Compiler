#! /bin/bash

NAME=$(echo "$2" | sed 's/source-//g')

display() {
	RET=$1
	if [ "$RET" = 0 ];
	then
		tput setaf 2
		echo "[$2 passed]"
		tput sgr0

		exit 0;

	elif [ "$RET" = "no" ];
	then
		./$2
		RET=$?
		if [ "$RET" = 0 ];
		then
			tput setaf 2
			echo "[$2 passed]"
			tput sgr0
		else 
			tput setaf 1
			echo "[$2 failed]"
			tput sgr0
			
			exit 1;
		fi
	else
		tput setaf 1
		echo "[$2 failed]"
		tput sgr0
		
		exit 1;
	fi
}
if [ ! -f "compile-$NAME.sh" ] ;
then
	TMP=$(mktemp)
	trap "rm $TMP" EXIT
	cpp -I ./include $1 >$TMP
	./cc $2-output.s $TMP $(./get-arc-name.sh)
	as $2-output.s -o output.o
	gcc output.o -o $2


	RET=$([ -f ./wrapper-$2.sh ] && ( ./wrapper-$2.sh; echo $? ) || echo "no")
	display $RET $2

else
	set -e
	./compile-$NAME.sh
	RET2="$?"
	RET=$([ -f ./wrapper-$2.sh ] && ( ./wrapper-$2.sh; echo $? ) || echo "$RET2")
	display $RET $2

fi
