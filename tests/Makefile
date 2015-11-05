CC=./cc

test: test01
	echo "done"

test%: test%.c
	$(CC) output.s <$^
	as output.s -o output.o
	ld output.o -o $@
	./$@-wrapper

clean:
	rm *.o
	rm *.s
	rm cc
	rm test01
