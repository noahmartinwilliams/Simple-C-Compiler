#! /bin/bash
RET=$([ -f ./wrapper-$1 ] && ( ./wrapper-$1; echo $? ) || echo "no")

if [ "$RET" = 0 ];
then
	tput setaf 2
	echo "[$1 passed]"
	tput sgr0

	exit 0;

elif [ "$RET" = "no" ];
then
	./$1
	RET=$?
	if [ "$RET" = 0 ];
	then
		tput setaf 2
		echo "[$1 passed]"
		tput sgr0
	else 
		tput setaf 1
		echo "[$1 failed]"
		tput sgr0
		
		exit 1;
	fi
else
	tput setaf 1
	echo -n "$1 failed"
	tput sgr0
	
	exit 1;
fi
