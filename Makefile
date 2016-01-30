INCLUDE=./include
include include/config.mk


main: main.c globals.o handle.a lex.yy.o generator.a optimization-globals.o loader.o exported.o
	$(CMB) -ldl
	cp $@ cc

exported.o: phase-4-backend/*
	$(MAKE) -C phase-4-backend/ ../exported.o


loader.o: phase-4-backend/*
	$(MAKE) -C phase-4-backend/ ../loader.o

phase-4-backend/libx64-backend.so: phase-4-backend/* phase-4-backend/x64/*
	$(MAKE) -C phase-4-backend/ libx64-backend.so

generator.a: phase-3-generator/* 
	$(MAKE) -C phase-3-generator/ ../generator.a

handle.a: phase-2-parser/*
	$(MAKE) -C phase-2-parser/ ../handle.a

lex.yy.o: phase-1-lexer/* handle.a
	$(MAKE) -C phase-1-lexer/ ../lex.yy.o

%.o: %.c
	$(CMP)

.PHONY:
main2: main phase-4-backend/libx64-backend.so
	echo "done"

.PHONY:
test: main_debug
	cp main tests/cc
	$(MAKE) -s -C tests/ test

.PHONY:
main_debug:
	$(MAKE) DEBUG=1 main2

.PHONY:
test%: main_debug
	cp main tests/cc
	$(MAKE) -s -C tests/ $@

.PHONY:
clena: 
	$(MAKE) clean

.PHONY:
clean: clean_intern
	$(MAKE) -C tests/ clean
	$(MAKE) -C phase-1-lexer/ clean
	$(MAKE) -C phase-2-parser/ clean
	$(MAKE) -C phase-3-generator/ clean
	$(MAKE) -C phase-4-backend/ clean
