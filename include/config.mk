OPT=-O
AR=ar crT $@ $^
ifdef DEBUG
YACC=bison -d --report=all --verbose --debug
CC=gcc -I $(INCLUDE) $(OPT) -ggdb -D DEBUG
else
YACC=bison -d
CC=gcc -I $(INCLUDE) $(OPT)
endif
CMP=$(CC) -c $^ -o $@
CMB=$(CC) $^ -o $@
LEX=flex
LD=ld

%.o: %.c $(INCLUDE)types.h
	$(CMP)
