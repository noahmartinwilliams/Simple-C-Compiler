CC=./cc

test: test01 test02 test03 test04 test05 test06
	echo "done"

test%: test%.c cc
	$(CC) $@-output.s <$^
	as $@-output.s -o output.o
	ld output.o -o $@
	./$@-wrapper && (tput setaf 2; echo "[$@ passed]";  tput sgr0; ) || (tput setaf 1; echo "[$@ failed]"; tput sgr0; )

clean:
	rm test01 2>/dev/null || true
	rm test02 2>/dev/null || true
	rm test03 2>/dev/null || true
	rm test04 2>/dev/null || true
	rm test05 2>/dev/null || true
	rm test*-output.s 2>/dev/null || true
	rm comp.output || true
	rm *.o 2>/dev/null || true
	rm *.s 2>/dev/null || true
	rm cc 2>/dev/null || true
