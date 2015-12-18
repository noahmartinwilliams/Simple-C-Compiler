CC=./cc

test: 
	ls source-test*.c | while read NAME; do N=$$(echo $$NAME | sed 's/source-\(.*\)\.c/\1/g'); make $$N || exit 1; done || exit 1;
	echo "done"


test%: source-test%.c cc
	$(CC) $@-output.s $<
	as $@-output.s -o output.o
	gcc output.o -o $@
	./run-test.sh $@

clean:
	rm test* 2>/dev/null || true
	rm test*-output.s 2>/dev/null || true
	rm comp.output || true
	rm *.o 2>/dev/null || true
	rm *.s 2>/dev/null || true
	rm cc 2>/dev/null || true
