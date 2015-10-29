CC=gcc -I ./include
CMP=$(CC) -c $^ -o $@
CMB=$(CC) $^ -o $@
YACC=bison -d --verbose --debug
LEX=flex

main: comp.tab.c print-stuff.o print-tree.o globals.o lex.yy.o handle-types.o generator.o free-stuff.o
	$(CMB)

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
