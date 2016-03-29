OPT=-fPIC -rdynamic 
AR=ar crT $@ $^
ifdef DEBUG
YACC=bison -d --report=all --verbose --debug
CC=gcc -I $(INCLUDE) $(OPT) -ggdb -DDEBUG
LEX=flex -d
else
YACC=bison -d
CC=gcc -I $(INCLUDE) $(OPT)
LEX=flex -s
endif
LD=ld
CMP=$(CC) -c $< -o $@
CMB=$(LD) -r $^ -o $@ 
CMBPROG=$(CC) $^ -o $@
SHARE=$(LD) -share $^ -o $@
