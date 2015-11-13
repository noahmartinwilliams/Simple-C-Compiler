CC=gcc -I $(INCLUDE)
AR=ar cr $@ $^
CMP=$(CC) -c $^ -o $@
CMB=$(CC) $^ -o $@
YACC=bison -d --verbose  --debug
LEX=flex
LD=ld

%.o: %.c
	$(CMP)
