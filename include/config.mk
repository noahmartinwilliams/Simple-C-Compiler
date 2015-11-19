CC=gcc -I $(INCLUDE) -ggdb
AR=ar cr $@ $^
CMP=$(CC) -c $^ -o $@
CMB=$(CC) $^ -o $@
YACC=bison -d --verbose 
LEX=flex
LD=ld

%.o: %.c
	$(CMP)
