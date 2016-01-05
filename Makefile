INCLUDE=./include
include include/config.mk

main_debug:
	$(MAKE) DEBUG=1 main

main: comp.tab.c globals.o lex.yy.o handle.a generator.a optimization-globals.o
	$(CMB)

generator.a: generator/* generator/backend/*
	$(MAKE) -C generator/ ../generator.a

ifdef DEBUG
handle.a: handle-types.o handle-exprs.o handle-statems.o handle-funcs.o handle-vars.o print-tree.o
	$(AR)
else
handle.a: handle-types.o handle-exprs.o handle-statems.o handle-funcs.o handle-vars.o
	$(AR)
endif

test: main_debug
	cp main tests/cc
	$(MAKE) -s -C tests/ test

test%: main_debug
	cp main tests/cc
	$(MAKE) -s -C tests/ $@

comp.y: parser/*
	$(MAKE) -C parser/ ../comp.y

comp.tab.c include/comp.tab.h: comp.y
	$(YACC) $^
	mv comp.tab.h include/

lex.yy.c: comp.l include/types.h
	$(LEX) $^

clena: 
	$(MAKE) clean
clean:
	rm lex.yy.c 2>/dev/null || true
	rm comp.tab.c 2>/dev/null || true
	rm include/comp.tab.h 2>/dev/null || true
	rm *.o 2>/dev/null || true
	rm main 2>/dev/null || true
	rm *.a 2>/dev/null || true
	rm comp.output || true
	rm comp.y || true
	$(MAKE) -C tests/ clean
	$(MAKE) -C generator/ clean
