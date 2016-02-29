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
