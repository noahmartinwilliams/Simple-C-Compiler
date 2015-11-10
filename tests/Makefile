CC=./cc

test: test01 test02
	echo "done"
	$(MAKE) clean

test%: test%.c
	$(CC) output.s <$^
	as output.s -o output.o
	ld output.o -o $@
	./$@-wrapper

clean:
	rm test01 || true
	rm test02 || true
	rm *.o || true
	rm *.s || true
	rm cc || true
