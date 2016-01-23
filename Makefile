INCLUDE=./include
include include/config.mk

main2: main phase-4-backend/libx64-backend.so
	echo "done"

main: main.c comp.tab.o globals.o lex.yy.o handle.a generator.a optimization-globals.o loader.o exported.o
	$(CMB) -ldl
	cp $@ cc

exported.o: phase-4-backend/*
	$(MAKE) -C phase-4-backend/ ../exported.o

main_debug:
	$(MAKE) DEBUG=1 main2

loader.o: phase-4-backend/*
	$(MAKE) -C phase-4-backend/ ../loader.o

phase-4-backend/libx64-backend.so: phase-4-backend/* phase-4-backend/x64/*
	$(MAKE) -C phase-4-backend/ libx64-backend.so

generator.a: phase-3-generator/* 
	$(MAKE) -C phase-3-generator/ ../generator.a

handle.a: phase-2-parser/* 
	$(MAKE) -C phase-2-parser/ ../handle.a

test: main_debug
	cp main tests/cc
	$(MAKE) -s -C tests/ test

test%: main_debug
	cp main tests/cc
	$(MAKE) -s -C tests/ $@

comp.y: phase-2-parser/*
	$(MAKE) -C phase-2-parser/ ../comp.y

comp.tab.c include/comp.tab.h: comp.y
	$(YACC) $^
	mv comp.tab.h include/

lex.yy.o: phase-1-lexer/*
	$(MAKE) -C phase-1-lexer/ ../lex.yy.o

.PHONY:
clena: 
	$(MAKE) clean
clean:
	rm lex.yy.c 2>/dev/null || true
	rm comp.tab.c 2>/dev/null || true
	rm include/comp.tab.h 2>/dev/null || true
	rm *.o 2>/dev/null || true
	rm main 2>/dev/null || true
	rm *.a 2>/dev/null || true
	rm comp.output 2>/dev/null || true
	rm comp.y 2>/dev/null || true
	rm cc 2>/dev/null || true
	rm *.so 2>/dev/null || true
	rm *.so 2>/dev/null || true
	$(MAKE) -C tests/ clean
	$(MAKE) -C phase-3-generator/ clean
	$(MAKE) -C phase-2-parser/ clean
	$(MAKE) -C phase-4-backend/ clean
