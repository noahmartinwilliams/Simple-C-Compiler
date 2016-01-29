INCLUDE=../include
include ../include/config.mk

../lex.yy.o: lex.yy.c
	$(CMP)

lex.yy.c: comp.l $(INCLUDE)/*
	$(LEX) $<

.PHONY:
clean: clean_intern
	echo "phase 1 clean"
