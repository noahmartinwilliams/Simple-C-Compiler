CC=./cc

test: test01 test02 test03 test04 test05 test06 test07 test08 test09 test10 test11 test12 test13 test14 test15 test16 test17 test18 test19 test20 test21 testb
	echo "done"

testb: test22 test23 test24 test25 test26 test27 test28 test29 test30 test31
	echo "done" >/dev/null

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
