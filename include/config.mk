AR=ar crT $@ $^
ifdef DEBUG
YACC=bison -d --report=all --verbose --debug
CC=gcc -I $(INCLUDE) -ggdb -D DEBUG
else
YACC=bison -d --report=all --verbose
CC=gcc -I $(INCLUDE)
endif
CMP=$(CC) -c $^ -o $@
CMB=$(CC) $^ -o $@
LEX=flex
LD=ld

%.o: %.c $(INCLUDE)types.h
	$(CMP)
