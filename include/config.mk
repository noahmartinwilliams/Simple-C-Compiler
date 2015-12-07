CC=gcc -I $(INCLUDE) -ggdb
AR=ar crT $@ $^
CMP=$(CC) -c $^ -o $@
CMB=$(CC) $^ -o $@
YACC=bison -d --report=all --verbose --debug
LEX=flex
LD=ld

%.o: %.c
	$(CMP)
