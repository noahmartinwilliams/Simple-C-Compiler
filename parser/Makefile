M4=m4 -I .

../comp.y: cc.y statements.m4 for.m4 expressions.m4 functions.m4 variables.m4 types.m4 ../include/types.h 
	$(M4) $< >$@
