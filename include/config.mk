CC=gcc -I $(INCLUDE) -ggdb
AR=ar crT $@ $^
CMP=$(CC) -c $^ -o $@
CMB=$(CC) $^ -o $@
YACC=bison -d --report=all
LEX=flex
LD=ld

%.o: %.c
	$(CMP)
