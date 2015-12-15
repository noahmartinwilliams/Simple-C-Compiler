CC=./cc

test: 
	ls source-test*.c | while read NAME; do N=$$(echo $$NAME | sed 's/source-\(.*\)\.c/\1/g'); make $$N; done
	echo "done"


test%: source-test%.c cc
	$(CC) $@-output.s $<
	as $@-output.s -o output.o
	gcc output.o -o $@
	[ -f ./wrapper-$@ ] && ( ./wrapper-$@ && (tput setaf 2; echo "[$@ passed]";  tput sgr0; ) || (tput setaf 1; echo "[$@ failed]"; tput sgr0; ) ) \
	|| ( ./$@ && (tput setaf 2; echo "[$@ passed]";  tput sgr0; ) || (tput setaf 1; echo "[$@ failed]"; tput sgr0; )  )

clean:
	rm test* 2>/dev/null || true
	rm test*-output.s 2>/dev/null || true
	rm comp.output || true
	rm *.o 2>/dev/null || true
	rm *.s 2>/dev/null || true
	rm cc 2>/dev/null || true
