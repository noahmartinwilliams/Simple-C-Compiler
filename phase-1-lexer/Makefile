INCLUDE=../include
include ../include/config.mk

../lex.yy.o: lex.yy.c
	$(CMP)

lex.yy.c: comp.l $(INCLUDE)/*
	$(LEX) $<

.PHONY:
clean:
	rm lex.yy.c 2>/dev/null || true
