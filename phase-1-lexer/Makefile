INCLUDE=../include
include ../include/config.mk

../lex.yy.c: comp.l ../include/types.h
	$(LEX) $^
	mv lex.yy.c ..

