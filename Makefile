CC=gcc -I ./include
AR=ar cr $@ $^
CMP=$(CC) -c $^ -o $@
CMB=$(CC) $^ -o $@
YACC=bison -d --verbose 
LEX=flex

main: comp.tab.c globals.o lex.yy.o generator.o handle.a
	$(CMB)

handle.a: handle-types.o handle-exprs.o handle-statems.o handle-registers.o handle-funcs.o handle-vars.o print-tree.o
	$(AR)

test: main
	cp main tests/cc
	$(MAKE) -C tests/ test

%.o: %.c
	$(CMP)

comp.tab.c include/comp.tab.h: comp.y
	$(YACC) $^
	mv comp.tab.h include/

lex.yy.c: comp.l
	$(LEX) $^

clean:
	rm lex.yy.c || true
	rm comp.tab.c || true
	rm include/comp.tab.h || true
	rm *.o || true
	rm main || true
	rm *.a || true
	$(MAKE) -C tests/ clean
