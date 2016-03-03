INCLUDE=../include
include ../include/config.mk

M4=m4 -I .

comp.tab.c: comp.y
	$(YACC) $^
	mv comp.tab.h ../include/

comp.y: cc.y statements.m4 expressions.m4 functions.m4 variables.m4 types.m4 ../include/*
	$(M4) $< >$@

ifdef DEBUG
../handle.a: types.o exprs.o statems.o funcs.o vars.o print-tree.o comp.tab.o consts.o
	$(AR)
else
../handle.a: types.o exprs.o statems.o funcs.o vars.o comp.tab.o consts.o
	$(AR)
endif

%.o: %.c
	$(CMP)

.PHONY:
clean: 
	rm comp.tab.c 2>/dev/null || true
	rm comp.y 2>/dev/null || true
	rm *.o 2>/dev/null || true
	rm lex.yy.c 2>/dev/null || true
	rm *.so 2>/dev/null || true
	rm comp.output 2>/dev/null || true
	rm *.s 2>/dev/null || true
	rm main 2>/dev/null || true
	rm include/comp.tab.h 2>/dev/null || true
	rm cc 2>/dev/null || true
	rm *.a 2>/dev/null || true

