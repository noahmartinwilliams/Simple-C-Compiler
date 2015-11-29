INCLUDE=./include
include include/config.mk

main: comp.tab.c globals.o lex.yy.o handle.a generator.a
	$(CMB)

generator.a: generator/* generator/backend/*
	$(MAKE) -C generator/ ../generator.a

handle.a: handle-types.o handle-exprs.o handle-statems.o  handle-funcs.o handle-vars.o print-tree.o
	$(AR)

test: main
	cp main tests/cc
	$(MAKE) -C tests/ test


comp.tab.c include/comp.tab.h: comp.y
	$(YACC) $^
	mv comp.tab.h include/

lex.yy.c: comp.l
	$(LEX) $^

clean:
	rm lex.yy.c 2>/dev/null || true
	rm comp.tab.c 2>/dev/null || true
	rm include/comp.tab.h 2>/dev/null || true
	rm *.o 2>/dev/null || true
	rm main 2>/dev/null || true
	rm *.a 2>/dev/null || true
	rm comp.output || true
	$(MAKE) -C tests/ clean
	$(MAKE) -C generator/ clean
