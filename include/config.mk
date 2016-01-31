OPT=-O -fPIC -rdynamic
AR=ar crT $@ $^
ifdef DEBUG
YACC=bison -d --report=all --verbose --debug
CC=gcc -I $(INCLUDE) $(OPT) -ggdb -D DEBUG
LEX=flex -s -d
else
YACC=bison -d
CC=gcc -I $(INCLUDE) $(OPT)
LEX=flex -s
endif
CMP=$(CC) -c $< -o $@
CMB=$(CC) $^ -o $@
LD=ld
SHARE=$(LD) -share $^ -o $@

.PHONY:
clean_intern:
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

