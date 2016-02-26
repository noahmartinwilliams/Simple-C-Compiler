INCLUDE=../include
include ../include/config.mk

M4=m4 -I .

comp.y: cc.y statements.m4 expressions.m4 functions.m4 variables.m4 types.m4 ../include/*
	$(M4) $< >$@

ifdef DEBUG
../handle.a: handle-types.o handle-exprs.o handle-statems.o handle-funcs.o handle-vars.o print-tree.o comp.tab.o handle-consts.o
	$(AR)
else
../handle.a: handle-types.o handle-exprs.o handle-statems.o handle-funcs.o handle-vars.o comp.tab.o handle-consts.o
	$(AR)
endif


comp.tab.c: comp.y
	$(YACC) $^
	mv comp.tab.h ../include/

%.o: %.c
	$(CMP)

.PHONY:
clean: clean_intern
	echo "phase 2 clean"
