INCLUDE=../include
include ../include/config.mk

M4=m4 -I .

ifdef DEBUG
../handle.a: handle-types.o handle-exprs.o handle-statems.o handle-funcs.o handle-vars.o print-tree.o
	$(AR)
else
../handle.a: handle-types.o handle-exprs.o handle-statems.o handle-funcs.o handle-vars.o 
	$(AR)
endif

../comp.y: cc.y statements.m4 for.m4 expressions.m4 functions.m4 variables.m4 types.m4 ../include/types.h 
	$(M4) $< >$@

.PHONY:
clean:
	rm *.o 2>/dev/null || true
